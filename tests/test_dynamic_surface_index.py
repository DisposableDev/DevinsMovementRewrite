"""Dynamic query publication, bounded storage and cursor lifetime under sanitizers."""
from pathlib import Path
import os
import re
import subprocess
import tempfile
import unittest

from regression_support import Source

ROOT = Path(__file__).resolve().parents[1]


class DynamicSurfaceIndexTests(unittest.TestCase):
    def test_publication_and_storage(self):
        query = (ROOT / "src/engine/surface_query.h").read_text()
        query = re.sub(r'^#include[^\n]*\n', '', query, flags=re.M)
        loading = Source(ROOT / "src/engine/surface_load.c")
        definitions = (Source(ROOT / "src/engine/surface_load.h").declaration("enum", "SpatialPartitions")
                       + loading.declaration("struct", "CellCoords")
                       + loading.declaration("struct", "DynamicWallTail")
                       + Source(ROOT / "include/macros.h").macros("ALIGN"))
        functions = "#if DYNAMIC_SURFACE_INDEX\n" + '\n'.join(loading.function(name) for name in (
            "extend_surface_query_bounds", "invalidate_dynamic_surface_index",
            "build_dynamic_surface_index_cell")) + "\n#endif\n"
        functions += '\n'.join(loading.function(name) for name in (
            "alloc_surface_node", "add_surface_to_cell", "lower_cell_index", "upper_cell_index",
            "add_surface", "publish_dynamic_surface_cell", "clear_dynamic_surfaces"))
        code = r'''
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
typedef int32_t s32;
typedef uint32_t u32;
typedef uint16_t u16;
typedef uint8_t u8;
typedef int16_t s16;
typedef float f32;
typedef s16 Vec3s[3];
typedef f32 Vec3f[3];
#define STATIC_SURFACE_INDEX 0
#define COLLISION_PROFILING 0
#define NUM_CELLS 8
#define LEVEL_BOUNDARY_MAX 2048
#define CELL_SIZE 512
#define TRUE 1
#define FALSE 0
#define ALWAYS_INLINE inline __attribute__((always_inline))
#define MIN(a,b) ((a)<(b)?(a):(b))
#define MAX(a,b) ((a)>(b)?(a):(b))
#define bzero(p,n) memset(p,0,n)
#define PUPPYPRINT_GET_COLLISION_SNAPSHOT() u32 first = 0
#define profiler_collision_update(first) ((void)(first))
#define persistent_surfaces_begin_frame() ((void)0)
#define NORMAL_FLOOR_THRESHOLD 0.01f
#define NORMAL_CEIL_THRESHOLD -0.01f
#define SURFACE_IS_NEW_WATER(type) ((type) == 100)
#define TIME_STOP_ACTIVE 1
struct Surface {
    int id, type;
    s16 lowerY, upperY;
    Vec3s vertex1, vertex2, vertex3;
    struct { f32 x,y,z; } normal;
};
struct SurfaceNode { struct SurfaceNode *next; struct Surface *surface; };
/* DEFINITIONS */
static struct SurfaceNode *gStaticSurfacePartition[NUM_CELLS][NUM_CELLS][NUM_SPATIAL_PARTITIONS];
static struct SurfaceNode *gDynamicSurfacePartition[NUM_CELLS][NUM_CELLS][NUM_SPATIAL_PARTITIONS];
/* QUERY */
#if DYNAMIC_SURFACE_INDEX
struct DynamicSurfaceIndex gDynamicSurfaceIndex = { .generation = 1 };
static union { struct Surface *alignment; u8 bytes[DYNAMIC_SURFACE_INDEX_BYTES]; } sDynamicSurfaceIndexData;
#endif
static union { struct SurfaceNode alignment; u8 bytes[131072]; } dynamicPool;
static void *gDynamicSurfacePool = dynamicPool.bytes;
static void *gDynamicSurfacePoolEnd = dynamicPool.bytes;
static void *gCurrStaticSurfacePoolEnd = dynamicPool.bytes;
static u32 gTimeStopState, gSurfacesAllocated, gSurfaceNodesAllocated;
static u32 gNumStaticSurfaces, gNumStaticSurfaceNodes, referencesCleared;
static struct DynamicWallTail sDynamicWallTails[16];
static struct CellCoords sCellsUsed[NUM_CELLS * NUM_SPATIAL_PARTITIONS];
static u32 sNumCellsUsed;
static s32 sClearAllCells, sWaterCellMapReady;
static u8 sWaterCellMayContainWater[NUM_CELLS][NUM_CELLS];
static void clear_dynamic_surface_references(void) { referencesCleared++; }
static void min_max_3i(s32 a,s32 b,s32 c,s32 *low,s32 *high) {
    *low=MIN(a,MIN(b,c)); *high=MAX(a,MAX(b,c));
}
/* FUNCTIONS */
static void reset(void) {
    gTimeStopState = 0;
    sClearAllCells = TRUE;
    clear_dynamic_surfaces();
}
static struct Surface make_surface(int id, int height, f32 normalY) {
    struct Surface s = {0};
    s.id = id;
    s.lowerY = height - 5;
    s.upperY = height + 5;
    s.normal.y = normalY;
    s.vertex1[0] = s.vertex1[2] = 100;
    s.vertex2[0] = s.vertex2[2] = 150;
    s.vertex3[0] = 180;
    s.vertex3[2] = 100;
    s.vertex1[1] = s.vertex2[1] = s.vertex3[1] = height;
    return s;
}
static struct SurfaceQueryIterator warm(int x,int z,int partition) {
    for(int query=0;query<3;query++) (void)surface_query_cell(x,z,partition,SURFACE_QUERY_DYNAMIC);
    return surface_query_cell(x,z,partition,SURFACE_QUERY_DYNAMIC);
}
static void compare(struct SurfaceQueryIterator scan, struct SurfaceNode *head) {
    for (struct SurfaceNode *node=head; node!=NULL; node=node->next) {
        assert(surface_query_next(&scan)==node->surface);
    }
    assert(surface_query_next(&scan)==NULL);
    assert(surface_query_next(&scan)==NULL);
}
static void test_publication_and_clear(void) {
    reset();
    struct Surface surfaces[9];
    for (int i=0;i<8;i++) {
        surfaces[i]=make_surface(i,100-i,1);
        add_surface(&surfaces[i],TRUE);
    }
    struct SurfaceNode *head=gDynamicSurfacePartition[4][4][SPATIAL_PARTITION_FLOORS];
#if DYNAMIC_SURFACE_INDEX
    u32 presenceGeneration = gDynamicSurfaceIndex.generation;
#endif
    for (int i=0;i<10;i++) {
        assert(!surface_query_cell_is_empty(4,4,SPATIAL_PARTITION_FLOORS,SURFACE_QUERY_DYNAMIC));
        assert(surface_query_cell_is_empty(-1,NUM_CELLS,-1,SURFACE_QUERY_EMPTY));
    }
#if DYNAMIC_SURFACE_INDEX
    assert(!gDynamicSurfaceIndex.active && gDynamicSurfaceIndex.used==0
           && gDynamicSurfaceIndex.generation==presenceGeneration);
#endif
    struct SurfaceQueryIterator first=surface_query_cell(4,4,SPATIAL_PARTITION_FLOORS,SURFACE_QUERY_DYNAMIC);
#if DYNAMIC_SURFACE_INDEX
    assert(first.remaining==-1 && gDynamicSurfaceIndex.used==0);
#endif
    compare(first,head);
    struct SurfaceQueryIterator packed=warm(4,4,SPATIAL_PARTITION_FLOORS);
#if DYNAMIC_SURFACE_INDEX
    assert(packed.remaining==8 && gDynamicSurfaceIndex.used>0);
    u32 used=gDynamicSurfaceIndex.used, generation=gDynamicSurfaceIndex.generation;
#endif
    compare(packed,head);
    // Time stop retains the geometry, pool and valid index instead of rebuilding.
    void *poolEnd=gDynamicSurfacePoolEnd;
    u32 cleared=referencesCleared;
    gTimeStopState=TIME_STOP_ACTIVE;
    clear_dynamic_surfaces();
    assert(gDynamicSurfacePoolEnd==poolEnd && referencesCleared==cleared);
#if DYNAMIC_SURFACE_INDEX
    assert(gDynamicSurfaceIndex.used==used && gDynamicSurfaceIndex.generation==generation);
#endif
    compare(warm(4,4,SPATIAL_PARTITION_FLOORS),head);
    // A lower-priority same-frame addition retains the head but changes its tail.
    surfaces[8]=make_surface(8,0,1);
    add_surface(&surfaces[8],TRUE);
    assert(gDynamicSurfacePartition[4][4][SPATIAL_PARTITION_FLOORS]==head);
#if DYNAMIC_SURFACE_INDEX
    assert(gDynamicSurfaceIndex.used==0 && gDynamicSurfaceIndex.generation!=generation);
#endif
    compare(warm(4,4,SPATIAL_PARTITION_FLOORS),head);
    gTimeStopState=0;
    clear_dynamic_surfaces();
    assert(surface_query_is_empty(warm(4,4,SPATIAL_PARTITION_FLOORS)));
    assert(gDynamicSurfacePoolEnd==gDynamicSurfacePool);
    // Reused node addresses and a fresh owner must supersede the earlier index.
    for (int i=0;i<8;i++) {
        surfaces[i]=make_surface(100+i,200+i,1);
        add_surface(&surfaces[i],TRUE);
    }
    compare(warm(4,4,SPATIAL_PARTITION_FLOORS),gDynamicSurfacePartition[4][4][SPATIAL_PARTITION_FLOORS]);
    reset();
}
#if DYNAMIC_SURFACE_INDEX
static void make_list(struct Surface *surfaces,struct SurfaceNode *nodes,int count,int firstId) {
    for(int i=0;i<count;i++) {
        surfaces[i]=make_surface(firstId+i,0,1);
        nodes[i]=(struct SurfaceNode){i+1<count?&nodes[i+1]:NULL,&surfaces[i]};
    }
}
static void test_independent_cursors_and_conflicts(void) {
    reset();
    struct Surface surfaces[24]; struct SurfaceNode nodes[24];
    make_list(surfaces,nodes,8,0);
    make_list(surfaces+8,nodes+8,8,8);
    make_list(surfaces+16,nodes+16,8,16);
    gDynamicSurfacePartition[0][0][0]=nodes;
    gDynamicSurfacePartition[0][1][0]=nodes+8;
    // Cells 0 and 33 share a bucket. A conflict cannot evict borrowed storage.
    gDynamicSurfacePartition[1][0][1]=nodes+16;
    struct SurfaceQueryIterator a=warm(0,0,0), b=warm(1,0,0), independent=a;
    assert(a.remaining==8 && b.remaining==8);
    struct SurfaceQueryIterator conflict=warm(0,1,1);
    assert(conflict.remaining==-1);
    compare(conflict,nodes+16);
    for(int i=0;i<8;i++) {
        assert(surface_query_next(&a)==&surfaces[i]);
        assert(surface_query_next(&b)==&surfaces[i+8]);
    }
    compare(independent,nodes);
    // Generation rollover cannot make an old cache slot valid again.
    gDynamicSurfaceIndex.generation=UINT32_MAX;
    invalidate_dynamic_surface_index();
    assert(gDynamicSurfaceIndex.generation==1 && gDynamicSurfaceIndex.used==0);
    for(int i=0;i<DYNAMIC_SURFACE_INDEX_CELLS;i++) assert(gDynamicSurfaceIndex.cells[i].generation==0);
    reset();
}
static void test_capacity_and_groups(void) {
    reset();
    struct Surface surfaces[16]; struct SurfaceNode nodes[16];
    make_list(surfaces,nodes,16,0);
    gDynamicSurfacePartition[0][0][0]=nodes;
    memset(sDynamicSurfaceIndexData.bytes,0xA5,sizeof(sDynamicSurfaceIndexData.bytes));
    size_t needed=8*sizeof(struct Surface *)+sizeof(struct SurfaceQueryBounds);
    u32 start=(DYNAMIC_SURFACE_INDEX_BYTES-needed)/sizeof(struct Surface *)*sizeof(struct Surface *);
    gDynamicSurfaceIndex.used=start;
    nodes[7].next=NULL;
    struct SurfaceQueryIterator a=warm(0,0,0);
    assert(a.remaining==8 && gDynamicSurfaceIndex.used==start+needed);
    for(u32 i=0;i<start;i++) assert(sDynamicSurfaceIndexData.bytes[i]==0xA5);
    for(size_t i=start+needed;i<DYNAMIC_SURFACE_INDEX_BYTES;i++) assert(sDynamicSurfaceIndexData.bytes[i]==0xA5);
    gDynamicSurfacePartition[0][1][0]=nodes+8;
    struct SurfaceQueryIterator exhausted=warm(1,0,0);
    assert(exhausted.remaining==-1 && gDynamicSurfaceIndex.used==start+needed);
    compare(exhausted,nodes+8);
    compare(a,nodes);
    reset();
    gDynamicSurfacePartition[0][0][0]=nodes;
    nodes[7].next=&nodes[8];
    for(int i=8;i<16;i++) {
        memset(surfaces[i].vertex1,0,sizeof(Vec3s));
        memset(surfaces[i].vertex2,0,sizeof(Vec3s));
        memset(surfaces[i].vertex3,0,sizeof(Vec3s));
    }
    nodes[15].surface=&surfaces[8]; // Preserve duplicate identities within a group.
    a=warm(0,0,0);
    const Vec3f minimum={-1,-10,-1}, maximum={1,10,1};
    for(int i=8;i<16;i++) assert(surface_query_next_in_bounds(&a,minimum,maximum)==nodes[i].surface);
    assert(surface_query_next_in_bounds(&a,minimum,maximum)==NULL);
    reset();
    // A sparse list never allocates, even on repeated searches.
    nodes[6].next=NULL;
    gDynamicSurfacePartition[0][0][0]=nodes;
    a=warm(0,0,0);
    assert(a.remaining==-1 && gDynamicSurfaceIndex.used==0);
    compare(a,nodes);
    reset();
}
#endif
static void test_stacked_wall_publication(void) {
    reset();
    struct Surface surfaces[163];
    for (int i=0;i<160;i++) {
        surfaces[i]=make_surface(i,400-i,0);
        // Repeated append hits and direct-map conflicts must both retain order.
        add_surface_to_cell(TRUE,(i/8%2)*4,(i/16%2)*4,&surfaces[i]);
    }
    for (int z=0;z<=4;z+=4) {
        for (int x=0;x<=4;x+=4) {
            struct SurfaceNode *node=gDynamicSurfacePartition[z][x][SPATIAL_PARTITION_WALLS];
            for (int i=0;i<160;i++) {
                if ((i/8%2)*4==x && (i/16%2)*4==z) {
                    assert(node!=NULL && node->surface==&surfaces[i]);
                    node=node->next;
                }
            }
            assert(node==NULL);
        }
    }
    // A persistent mesh can append its own nodes between ordinary publications.
    struct SurfaceNode extra[2];
    for (int i=0;i<2;i++) {
        surfaces[160+i]=make_surface(160+i,0,0);
        extra[i].surface=&surfaces[160+i];
        extra[i].next=i==0 ? &extra[1] : NULL;
    }
    publish_dynamic_surface_cell(4,4,SPATIAL_PARTITION_WALLS,extra);
    surfaces[162]=make_surface(162,0,0);
    add_surface_to_cell(TRUE,4,4,&surfaces[162]);
    struct SurfaceNode *tail=gDynamicSurfacePartition[4][4][SPATIAL_PARTITION_WALLS];
    while (tail->surface!=&surfaces[160]) tail=tail->next;
    assert(tail->next==&extra[1] && extra[1].next->surface==&surfaces[162]);
    assert(extra[1].next->next==NULL);
    gTimeStopState=TIME_STOP_ACTIVE;
    clear_dynamic_surfaces();
    assert(gDynamicSurfacePartition[4][4][SPATIAL_PARTITION_WALLS]!=NULL);
    gTimeStopState=0;
    clear_dynamic_surfaces();
    // Reusing the pool cannot leave a remembered tail attached to the old list.
    add_surface_to_cell(TRUE,4,4,&surfaces[162]);
    tail=gDynamicSurfacePartition[4][4][SPATIAL_PARTITION_WALLS];
    assert(tail->surface==&surfaces[162] && tail->next==NULL);
    reset();
}
int main(void) {
    test_publication_and_clear();
    test_stacked_wall_publication();
#if DYNAMIC_SURFACE_INDEX
    test_independent_cursors_and_conflicts();
    test_capacity_and_groups();
#endif
    puts("dynamic index: publication, time stop, ordering, storage reuse and fallback preserved");
    return 0;
}
'''
        code = code.replace('/* QUERY */', query).replace('/* DEFINITIONS */', definitions)
        code = code.replace('/* FUNCTIONS */', functions)
        with tempfile.TemporaryDirectory(prefix="dynamic-surface-index-") as directory:
            path = Path(directory)
            (path / "test.c").write_text(code)
            for enabled in (1, 0):
                subprocess.run([
                    "cc", f"-DDYNAMIC_SURFACE_INDEX={enabled}", "-std=c99", "-O1",
                    "-Wall", "-Wextra", "-Werror", "-fno-strict-aliasing",
                    "-fsanitize=address,undefined", "-fno-omit-frame-pointer",
                    str(path / "test.c"), "-o", str(path / "test"),
                ], check=True)
                subprocess.run([str(path / "test")], check=True,
                               env={**os.environ, "ASAN_OPTIONS": "detect_leaks=0"})


if __name__ == "__main__":
    unittest.main()
