"""Reconstruct native geometry ranges and deterministic project derivatives."""
import itertools
import struct


def reconstruct_actor_array(decoded_segment, spec):
    """Accept one decoded original segment and return the final big-endian bytes."""
    size = spec['bytes']
    transform = spec.get('transform', {})
    if 'bounds_ranges' in transform:
        points = []
        for start, length in transform['bounds_ranges']:
            if start < 0 or length <= 0 or length % 16 or start + length > len(decoded_segment):
                raise ValueError('Invalid native bounds source range')
            points.extend(struct.unpack_from('>3h', decoded_segment, offset)
                          for offset in range(start, start + length, 16))
        if not points or size != 128 or transform.get('bounds_padding') != 1:
            raise ValueError('Invalid terrain bounds specification')
        bounds = [(min(point[axis] for point in points) - 1,
                   max(point[axis] for point in points) + 1) for axis in range(3)]
        result = bytearray().join(struct.pack('>6h4B', x, y, z, 0, 0, 0, 0, 0, 0, 0)
                                  for x, y, z in itertools.product(*bounds))
    elif 'vertex_offsets' in transform:
        offsets = transform['vertex_offsets']
        if len(offsets) * 16 != size:
            raise ValueError('Vertex permutation size mismatch')
        if any(offset < 0 or offset + 16 > len(decoded_segment) for offset in offsets):
            raise ValueError('Vertex source outside original actor segment')
        result = bytearray().join(decoded_segment[offset:offset + 16] for offset in offsets)
    else:
        offset = spec['source']['offset']
        if offset < 0 or size < 0 or offset + size > len(decoded_segment):
            raise ValueError('Actor source range outside original segment')
        result = bytearray(decoded_segment[offset:offset + size])
    if 'grid_quantization' in transform:
        grid = transform['grid_quantization']
        if grid not in (128, 256, 512) or len(result) % 16:
            raise ValueError('Invalid ship LOD quantization')
        for offset in range(0, len(result), 16):
            position = struct.unpack_from('>3h', result, offset)
            struct.pack_into('>3h', result, offset,
                             *(round(value / grid) * grid for value in position))
    if 'position_translation' in transform or 'coordinate_plane_shift' in transform:
        if len(result) % 16:
            raise ValueError('Invalid transformed vertex array size')
        translation = transform.get('position_translation', [0, 0, 0])
        if len(translation) != 3 or any(not isinstance(value, int) for value in translation):
            raise ValueError('Invalid vertex position translation')
        plane = transform.get('coordinate_plane_shift')
        if plane and plane['axis'] not in (0, 1, 2):
            raise ValueError('Invalid coordinate plane axis')
        for offset in range(0, len(result), 16):
            position = list(struct.unpack_from('>3h', result, offset))
            if plane and position[plane['axis']] == plane['at']:
                position[plane['axis']] += plane['delta']
            position = [value + shift for value, shift in zip(position, translation)]
            struct.pack_into('>3h', result, offset, *position)
    for offset, value in spec.get('edits', []):
        if not 0 <= offset < size or not 0 <= value <= 255:
            raise ValueError('Invalid project actor edit')
        result[offset] = value
    return bytes(result)
