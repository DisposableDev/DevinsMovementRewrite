"""Audit JRB render-only bounds, ordered terrain commands and native poses."""
from pathlib import Path
import math
import hashlib
import json
import re
import runpy
import unittest

try:
    from .source_asset import read_text as read_source
except ImportError:
    from source_asset import read_text as read_source

ROOT = Path(__file__).resolve().parents[1]


def arrays(text):
    """Map named unsized Vtx/Gfx arrays to (kind, source body) in the native format."""
    return {name: (kind, body) for kind, name, body in re.findall(
        r"\b(Vtx|Gfx)\s+(\w+)\[\]\s*=\s*\{(.*?)\n\};", text, re.S)}


def points(body):
    """Extract decimal XYZ triples from native Vtx rows, discarding other attributes."""
    return [tuple(map(int, row)) for row in re.findall(
        r"\{\{\{\s*(-?\d+),\s*(-?\d+),\s*(-?\d+)\}", body)]


def display_list_events(text, root):
    """Expand lists into ordered command and full-vertex triangle events."""
    model_arrays = arrays(text)
    vertex_cache = {}
    events = []

    # Nested calls share one vertex cache; each root traversal starts with a fresh cache.
    def visit_display_list(display_list_name):
        for operation, arguments in re.findall(
                r"(gs\w+)\((.*?)\),", model_arrays[display_list_name][1], re.S):
            fields = [re.sub(r"\s+", "", field) for field in arguments.split(",")]
            if operation == "gsSPDisplayList":
                visit_display_list(fields[0])
            elif operation == "gsSPVertex":
                vertex_rows = re.findall(
                    r"\{\{\{.*?\}\}\}", re.sub(r"\s+", "", model_arrays[fields[0]][1]))
                vertex_count, cache_offset = int(fields[1]), int(fields[2])
                if len(vertex_rows) < vertex_count or vertex_count + cache_offset > 32:
                    raise ValueError("vertex load exceeds array")
                for row_index, row in enumerate(vertex_rows[:vertex_count]):
                    vertex_cache[row_index + cache_offset] = tuple(
                        int(value, 0) for value in re.findall(r"0x[\da-fA-F]+|-?\d+", row))
            elif operation in ("gsSP1Triangle", "gsSP2Triangles"):
                for field_offset in range(0, len(fields), 4):
                    triangle = tuple(
                        vertex_cache[int(vertex)] for vertex in fields[field_offset:field_offset + 3])
                    events.append(("triangle", triangle, int(fields[field_offset + 3], 0)))
            elif operation != "gsSPEndDisplayList":
                events.append((operation, tuple(fields)))

    visit_display_list(root)
    return events


class JrbAssets(unittest.TestCase):
    def test_native_animated_bounds(self):
        bounds_adapter = runpy.run_path(str(ROOT / "tools/generate_physics_lab_bounds.py"))["Bounds"]()
        for actor, layout_name, radius_limit in (
                ("seaweed", "seaweed_geo", 300), ("clam_shell", "clam_shell_geo", 448)):
            geometry_radius, shadow_radius, _ = bounds_adapter.measure(
                bounds_adapter.layout(layout_name))
            self.assertLessEqual(geometry_radius, radius_limit)
            self.assertEqual(shadow_radius, 0)
            self.assertNotRegex(bounds_adapter.arrays[layout_name][1], r"GEO_(?:ASM|HELD_OBJECT)")
            switch_callbacks = re.findall(
                r"GEO_SWITCH_CASE\([^,]+,\s*(\w+)\)", bounds_adapter.arrays[layout_name][1])
            self.assertEqual(switch_callbacks, ["geo_switch_seaweed_lod"] if actor == "seaweed" else [])
            for animation_file in sorted((ROOT / f"actors/{actor}/anims").glob("anim_*.inc.c")):
                animation_source = read_source(animation_file)
                values_body = re.search(
                    r"static const s16 \w+\[\] = \{(.*?)\};", animation_source, re.S)[1]
                index_body = re.search(
                    r"static const u16 \w+\[\] = \{(.*?)\};", animation_source, re.S)[1]
                animation_values = [int(v, 0) for v in re.findall(r"0x[\da-fA-F]+|-?\d+", values_body)]
                animation_indices = [int(v, 0) for v in re.findall(r"0x[\da-fA-F]+|\d+", index_body)]
                # The first three index pairs are root XYZ translation channels.
                # Layout bounds assume these animation translations remain zero.
                for channel_count, channel_start in zip(
                        animation_indices[:6:2], animation_indices[1:6:2]):
                    self.assertTrue(
                        all(v == 0
                            for v in animation_values[channel_start:channel_start + channel_count]),
                        animation_file)

    def test_seaweed_far_pose_and_materials(self):
        model_arrays = arrays(read_source(ROOT / "actors/seaweed/model.inc.c"))
        animation_source = read_source(ROOT / "actors/seaweed/anims/anim_0600A4BC.inc.c")
        values_body = re.search(r"static const s16 \w+\[\] = \{(.*?)\};", animation_source, re.S)[1]
        index_body = re.search(r"static const u16 \w+\[\] = \{(.*?)\};", animation_source, re.S)[1]
        animation_values = [int(v, 0) for v in re.findall(r"0x[\da-fA-F]+|-?\d+", values_body)]
        animation_indices = [int(v, 0) for v in re.findall(r"0x[\da-fA-F]+|\d+", index_body)]
        joint_origin_x = joint_origin_y = angle_radians = 0
        expected_commands = []

        def flatten_commands(display_list_name):
            commands = []
            for command in model_arrays[display_list_name][1].strip().splitlines():
                command = command.strip()
                nested_list = re.fullmatch(r"gsSPDisplayList\((\w+)\),", command)
                if nested_list:
                    commands.extend(flatten_commands(nested_list[1]))
                elif command != "gsSPEndDisplayList(),":
                    commands.append(command)
            return commands

        for joint, offset, suffix, display_suffix in (
                (2, 238, "0600A000", "0600A088"),
                (3, 317, "06009F08", "06009F90"),
                (4, 314, "06009E10", "06009E98")):
            # Translate along the accumulated angle before adding this joint rotation.
            joint_origin_x += offset * math.cos(angle_radians)
            joint_origin_y += offset * math.sin(angle_radians)
            channel_count, channel_start = animation_indices[(3 + joint * 3 + 2) * 2:][:2]
            # Sample frame 20, clamped to this joint's Z-rotation channel length.
            angle_radians += (
                animation_values[channel_start + min(20, channel_count - 1)] * math.tau / 65536)
            native_vertex_array = "seaweed_seg6_vertex_" + suffix
            far_vertex_array = "seaweed_far_vertex_" + suffix
            expected_positions = [
                (round(joint_origin_x + x * math.cos(angle_radians) - y * math.sin(angle_radians)),
                 round(joint_origin_y + x * math.sin(angle_radians) + y * math.cos(angle_radians)), z)
                for x, y, z in points(model_arrays[native_vertex_array][1])]
            self.assertEqual(points(model_arrays[far_vertex_array][1]), expected_positions)
            # UVs, normal/color bytes, triangle order and every material command stay native.
            vertex_pattern = r"\{\{\{\s*-?\d+,\s*-?\d+,\s*-?\d+\}"
            self.assertEqual(re.sub(vertex_pattern, "POSITION", model_arrays[native_vertex_array][1]),
                             re.sub(vertex_pattern, "POSITION", model_arrays[far_vertex_array][1]))
            expected_commands.extend(
                command.replace(native_vertex_array, far_vertex_array)
                for command in flatten_commands("seaweed_seg6_dl_" + display_suffix))
        self.assertEqual(flatten_commands("seaweed_far_upper_dl"), expected_commands)

    def test_ship_native_geometry_and_materials(self):
        # Fixed event hashes retain full vertices, winding and material order.
        # Cache slots and single/paired triangle spelling do not affect these events.
        native_source = read_source(ROOT / "levels/jrb/wooden_ship/model.inc.c")
        expected_digests = {
            "jrb_seg7_dl_07008FD8": "b939f06962ba79e07443b74842ce27618ab9c9fb292752537475b9f2540f7cf9",
            "jrb_seg7_dl_070090B0": "11fe8666b2dff8aa6e1a5cc9d30a8d7e3dea011b8e338d3d96225ff7117489e0",
            "jrb_seg7_dl_07009A58": "29c0ce5f9f418bd3cf379db09dce0f3f3b52ea3967017d8fa298c684fa764ddf",
            "jrb_seg7_dl_07009B30": "b16ebd78382436e72d232ecef0f962d0ad4ce4c22904014eaf9ad34abfde373d",
            "jrb_seg7_dl_0700A608": "fd49cb80be77ef0b6188d9a871737620d702c9a063cbcfc7706c5702f63a637f",
            "jrb_seg7_dl_0700AC68": "345669b7cfbc1c10f7f64541e67757b118272dca169bb9a8a749e350be805664",
        }
        for root, digest in expected_digests.items():
            events = display_list_events(native_source, root)
            actual_digest = hashlib.sha256(
                json.dumps(events, separators=(",", ":")).encode()).hexdigest()
            self.assertEqual(actual_digest, digest, root)
        vertex_load_counts = [
            int(count) for count in re.findall(r"gsSPVertex\(\w+,\s*(\d+),\s*0\)", native_source)]
        self.assertTrue(all(0 < count <= 32 for count in vertex_load_counts))
        self.assertLess(len(vertex_load_counts), 34)

    def test_ship_far_geometry_and_materials(self):
        native_arrays = arrays(read_source(ROOT / "levels/jrb/wooden_ship/model.inc.c"))
        lod_arrays = arrays(read_source(ROOT / "levels/jrb/wooden_ship/far_model.inc.c"))

        def vertex_rows(body, grid_spacing):
            vertices = []
            pattern = (r"\{\{\{\s*(-?\d+),\s*(-?\d+),\s*(-?\d+)\},\s*0,"
                       r"\s*\{\s*(-?\d+),\s*(-?\d+)\},\s*\{(.*?)\}\}\}")
            for x, y, z, u, v, colors in re.findall(pattern, re.sub(r"\s+", "", body)):
                position = tuple(int(value) for value in (x, y, z))
                # False disables XYZ rounding; UV and color/normal values stay unchanged.
                if grid_spacing:
                    position = tuple(round(value / grid_spacing) * grid_spacing for value in position)
                vertices.append(
                    position + (int(u), int(v))
                    + tuple(int(value.strip(), 0) for value in colors.split(",")))
            return vertices

        def lod_events(model_arrays, root, grid_spacing):
            draw_events = []
            vertex_cache = {}

            def visit_display_list(display_list_name):
                commands = re.findall(r"(gs\w+)\((.*?)\),", model_arrays[display_list_name][1], re.S)
                for operation, arguments in commands:
                    fields = [re.sub(r"\s+", " ", field.strip())
                              for field in arguments.split(",")]
                    if operation == "gsSPDisplayList":
                        visit_display_list(fields[0])
                    elif operation == "gsSPVertex":
                        # Unlike the general walker, this adapter requires slot-zero loads.
                        self.assertEqual(int(fields[2]), 0)
                        vertices = vertex_rows(model_arrays[fields[0]][1], grid_spacing)
                        self.assertGreaterEqual(len(vertices), int(fields[1]))
                        # Each load replaces the cache instead of retaining other slots.
                        vertex_cache.clear()
                        vertex_cache.update(enumerate(vertices[:int(fields[1])]))
                    elif operation in ("gsSP1Triangle", "gsSP2Triangles"):
                        for field_offset in range(0, len(fields), 4):
                            triangle = tuple(
                                vertex_cache[int(field)]
                                for field in fields[field_offset:field_offset + 3])
                            # Quantized expectations omit repeated XYZ; raw inputs keep all triangles.
                            if not grid_spacing or len({vertex[:3] for vertex in triangle}) == 3:
                                draw_events.append(
                                    ("triangle", triangle, int(fields[field_offset + 3], 0)))
                    elif operation != "gsSPEndDisplayList":
                        draw_events.append((operation, tuple(fields)))

            visit_display_list(root)
            return draw_events

        ship_layout_source = read_source(ROOT / "levels/jrb/wooden_ship/geo.inc.c")
        for variant, grid in (("medium", 128), ("far", 256), ("very_far", 512)):
            native_triangle_count = lod_triangle_count = 0
            for suffix in ("07008FD8", "070090B0", "07009A58", "07009B30", "0700A608", "0700AC68"):
                expected_events = lod_events(native_arrays, "jrb_seg7_dl_" + suffix, grid)
                actual_events = lod_events(lod_arrays, f"jrb_ship_{variant}_dl_" + suffix, False)
                self.assertEqual(actual_events, expected_events, (variant, suffix))
                lod_triangle_count += sum(event[0] == "triangle" for event in actual_events)
                native_triangle_count += sum(
                    event[0] == "triangle" for event in
                    lod_events(native_arrays, "jrb_seg7_dl_" + suffix, False))
                self.assertIn(f"jrb_ship_{variant}_dl_" + suffix, ship_layout_source)
            self.assertLess(lod_triangle_count, native_triangle_count, variant)
        self.assertEqual(ship_layout_source.count("GEO_SWITCH_CASE(4, geo_switch_jrb_ship_lod)"), 6)
        self.assertEqual(
            ship_layout_source.count(
                "GEO_ASM(GEO_TRANSPARENCY_MODE_NORMAL, geo_update_layer_transparency)"), 2)

    def test_occluders_are_native_opaque_triangles(self):
        model_arrays = {}
        for part in range(2, 6):
            model_arrays.update(arrays(read_source(ROOT / f"levels/jrb/areas/1/{part}/model.inc.c")))
        native_triangles = []
        vertex_cache = {}

        # XYZ reconstruction only; these terrain roots are submitted on opaque layers.
        # One vertex cache spans nested lists and all four root traversals.
        def collect_triangles(display_list_name):
            for operation, arguments in re.findall(
                    r"(gsSP\w+)\((.*?)\)", model_arrays[display_list_name][1]):
                fields = [field.strip() for field in arguments.split(",")]
                if operation == "gsSPDisplayList":
                    collect_triangles(fields[0])
                elif operation == "gsSPVertex":
                    for row_index, vertex in enumerate(
                            points(model_arrays[fields[0]][1])[:int(fields[1])]):
                        vertex_cache[row_index + int(fields[2])] = vertex
                elif operation in ("gsSP1Triangle", "gsSP2Triangles"):
                    for field_offset in range(0, len(fields), 4):
                        native_triangles.append(tuple(
                            vertex_cache[int(field)]
                            for field in fields[field_offset:field_offset + 3]))

        for root in ("07004940", "07004C78", "070058C8", "070069B0"):
            collect_triangles("jrb_seg7_dl_" + root)
        renderer_source = read_source(ROOT / "src/game/rendering_graph_node.c")
        occluder_source = re.search(
            r"sJrbOccluderVertices\[\]\[3\] = \{(.*?)\n\};", renderer_source, re.S)[1]
        # Parse numbers independently of whitespace and the formatter's nesting.
        numbers = [int(float(value)) for value in re.findall(r"(-?\d+\.0)f", occluder_source)]
        occluder_vertices = [
            tuple(numbers[coordinate_offset:coordinate_offset + 3])
            for coordinate_offset in range(0, len(numbers), 3)]
        self.assertEqual(len(occluder_vertices), 9)
        for triangle_offset in range(0, len(occluder_vertices), 3):
            triangle = tuple(occluder_vertices[triangle_offset:triangle_offset + 3])
            self.assertIn(triangle, native_triangles)
            self.assertEqual(len({vertex[1] for vertex in triangle}), 1)
            # Horizontal occluder triangles must retain upward-facing winding.
            a, b, c = triangle
            self.assertGreater((b[2] - a[2]) * (c[0] - a[0]) - (b[0] - a[0]) * (c[2] - a[2]), 0)
        # Check the rock's model-space mesh against its renderer occlusion radius.
        rock_arrays = arrays(read_source(ROOT / "levels/jrb/rock/model.inc.c"))
        for kind, vertex_source in rock_arrays.values():
            if kind == "Vtx":
                self.assertTrue(all(
                    sum(axis * axis for axis in vertex) < 1100 ** 2
                    for vertex in points(vertex_source)))

    def test_terrain_bounds_and_order(self):
        model_arrays = {}
        for part in range(2, 7):
            model_arrays.update(arrays(read_source(ROOT / f"levels/jrb/areas/1/{part}/model.inc.c")))
        generated_source = read_source(ROOT / "levels/jrb/areas/1/terrain_culling.inc.c")
        model_arrays.update(arrays(generated_source))
        terrain_generator = runpy.run_path(str(ROOT / "tools/generate_jrb_terrain.py"))
        self.assertEqual(terrain_generator["generate"]()[0], generated_source)

        # Validate each section recursively; drawing-event equality is checked separately.
        def check_section_bounds(display_list_name):
            commands = model_arrays[display_list_name][1].strip().splitlines()
            command_index = 0
            while command_index < len(commands):
                line = commands[command_index].strip()
                if (command_index + 3 < len(commands)
                        and "_bounds, 8, 0)" in commands[command_index + 1]):
                    self.assertEqual(line, "gsSPClearGeometryMode(G_LIGHTING),")
                    self.assertEqual(
                        commands[command_index + 2].strip(), "gsSPSetGeometryMode(G_LIGHTING),")
                    self.assertEqual(commands[command_index + 3].strip(), "gsSPCullDisplayList(0, 7),")
                    bounds_array_name = re.search(r"gsSPVertex\((\w+)", commands[command_index + 1])[1]
                    bounds_vertices = points(model_arrays[bounds_array_name][1])
                    self.assertEqual(len(bounds_vertices), 8)
                    for vertex_array, count in re.findall(
                            r"gsSPVertex\((\w+),\s*(\d+),\s*0\)",
                            "\n".join(commands[command_index + 4:])):
                        for point in points(model_arrays[vertex_array][1])[:int(count)]:
                            for axis in range(3):
                                self.assertGreater(point[axis], min(p[axis] for p in bounds_vertices))
                                self.assertLess(point[axis], max(p[axis] for p in bounds_vertices))
                    command_index += 4
                    continue
                child = re.fullmatch(r"gsSPDisplayList\((\w+)\),", line)
                if child:
                    check_section_bounds(child[1])
                command_index += 1

        for suffix in terrain_generator["ROOTS"]:
            display_list_name = "jrb_seg7_dl_" + suffix
            # Compare actual ordered vertices, attributes and material events;
            # legal vertex/triangle batching changes command spelling.
            combined_source = "\n".join(
                f"{kind} {key}[] = {{\n{body}\n}};"
                for key, (kind, body) in model_arrays.items())
            drawing_source = combined_source
            for key, (kind, body) in model_arrays.items():
                if kind == "Gfx" and "_bounds, 8, 0)" in body:
                    stripped = re.sub(r"    gsSPClearGeometryMode\(G_LIGHTING\),\n"
                                      r"    gsSPVertex\(\w+_bounds, 8, 0\),\n"
                                      r"    gsSPSetGeometryMode\(G_LIGHTING\),\n"
                                      r"    gsSPCullDisplayList\(0, 7\),\n", "", body)
                    drawing_source = drawing_source.replace(body, stripped)
            check_section_bounds(display_list_name + "_culled")
            self.assertEqual(
                display_list_events(drawing_source, display_list_name),
                display_list_events(drawing_source, display_list_name + "_culled"), display_list_name)

    def test_portal_masks(self):
        generated_source = read_source(ROOT / "levels/jrb/areas/1/terrain_culling.inc.c")
        model_arrays = arrays(generated_source)
        tunnel_sections = {
            f"jrb_seg7_dl_{part}_section_0_0" for part in
            ("070044C8", "07004658", "07004B88", "07004BF0", "070065A8", "07006980")}
        cave_sections = tunnel_sections | {
            "jrb_seg7_dl_070041F0_section_1_0", "jrb_seg7_dl_07006508_section_0_0",
            "jrb_seg7_dl_070065A8_section_1_0", "jrb_seg7_dl_070065A8_section_1_1"}
        from itertools import product
        for variant, hidden_sections, camera_regions in (
                ("cave", cave_sections, [((4700, 5600), (-2900, -2000), (-7500, -3100))]),
                ("tunnel", tunnel_sections, [
                    ((4700, 5600), (-4100, -2400), (-3100, -1900)),
                    ((4700, 5600), (-4800, -3200), (-1900, -800))])):
            triangle_count = 0
            for display_list_name in hidden_sections:
                section_vertices = [
                    point for vertex_array, count in re.findall(
                        r"gsSPVertex\((\w+),\s*(\d+)", model_arrays[display_list_name][1])
                    for point in points(model_arrays[vertex_array][1])[:int(count)]]
                section_bounds = [
                    (min(p[a] for p in section_vertices) - 1,
                     max(p[a] for p in section_vertices) + 1) for a in range(3)]
                self.assertGreaterEqual(section_bounds[2][0], 775)
                triangle_count += sum(
                    1 if op == "gsSP1Triangle" else 2 for op in
                    re.findall(r"gsSP(?:1Triangle|2Triangles)(?=\()",
                               model_arrays[display_list_name][1]))
                for region in camera_regions:
                    opening_points = []
                    for camera in product(*region):
                        for target in product(*section_bounds):
                            t = (575 - camera[2]) / (target[2] - camera[2])
                            opening_points.append(tuple(
                                camera[a] + (target[a] - camera[a]) * t for a in (0, 1)))
                    # All corner rays must clear the same edge of the padded opening.
                    self.assertTrue(
                        any(all(sign * point[axis] > sign * edge for point in opening_points)
                            for axis, sign, edge in ((0, -1, 4490), (0, 1, 5810),
                                                     (1, -1, -5250), (1, 1, -1800))),
                        display_list_name)
            self.assertEqual(triangle_count, 172 if variant == "cave" else 74)

            def check_parent(display_list_name):
                expected_body = model_arrays[display_list_name][1]
                for child in re.findall(r"gsSPDisplayList\((\w+)\)", expected_body):
                    if child in hidden_sections:
                        expected_body = expected_body.replace(f"    gsSPDisplayList({child}),\n", "")
                    elif "_section_" not in child:
                        check_parent(child)
                        expected_body = expected_body.replace(
                            child, child.removesuffix("_culled") + "_" + variant)
                actual_body = model_arrays[display_list_name.removesuffix("_culled") + "_" + variant][1]
                self.assertEqual(re.sub(r"\s+", "", actual_body), re.sub(r"\s+", "", expected_body))

            for suffix in ("07004940", "07004C78", "070069B0"):
                check_parent("jrb_seg7_dl_" + suffix + "_culled")

    def test_camera_corridor_inside_native_cavity(self):
        # Clip every native floor/ceiling triangle to the camera corridor's XZ
        # rectangle. Heights are affine, so checking polygon corners covers its
        # entire intersection, including slopes between the sampled trial views.
        native_source = "\n".join(read_source(ROOT / f"levels/jrb/areas/1/{part}/model.inc.c")
                           for part in range(2, 7))
        native_triangles = [
            event[1] for suffix in ("070058C8", "07007570")
            for event in display_list_events(native_source, "jrb_seg7_dl_" + suffix)
            if event[0] == "triangle"]
        for zmin, zmax, sloping in ((-7500, -3100, False), (-3100, -800, True)):
            # Indices 0/1 accumulate XZ area for upward floors/downward ceilings.
            area_by_facing = [0.0, 0.0]
            for triangle in native_triangles:
                polygon = [tuple(vertex[:3]) for vertex in triangle]
                a, b, c = polygon
                normal_y = (b[2] - a[2]) * (c[0] - a[0]) - (b[0] - a[0]) * (c[2] - a[2])
                if normal_y == 0:
                    continue
                for axis, edge, sign in ((0, 4950, 1), (0, 5350, -1), (2, zmin, 1), (2, zmax, -1)):
                    clipped_polygon = []
                    for start, end in zip(polygon, polygon[1:] + polygon[:1]):
                        inside_start = sign * (start[axis] - edge) >= 0
                        inside_end = sign * (end[axis] - edge) >= 0
                        if inside_start:
                            clipped_polygon.append(start)
                        if inside_start != inside_end:
                            t = (edge - start[axis]) / (end[axis] - start[axis])
                            clipped_polygon.append(tuple(
                                start[i] + t * (end[i] - start[i]) for i in range(3)))
                    polygon = clipped_polygon
                if len(polygon) < 3:
                    continue
                area = abs(sum(start[0] * end[2] - end[0] * start[2]
                               for start, end in zip(polygon, polygon[1:] + polygon[:1]))) / 2
                if area < 0.001:
                    continue
                area_by_facing[normal_y < 0] += area
                for _, height, z in polygon:
                    minimum_camera_y = -5050 - 0.6 * z if sloping else -2900
                    maximum_camera_y = -4550 - 0.6 * z if sloping else -2300
                    self.assertGreater(
                        minimum_camera_y - height if normal_y > 0 else height - maximum_camera_y, 1.0)
            for area in area_by_facing:
                self.assertGreaterEqual(area, 400 * (zmax - zmin) - 0.01)



if __name__ == "__main__":
    unittest.main()
