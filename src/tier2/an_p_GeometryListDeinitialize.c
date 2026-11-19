/* GeometryListDeinitialize (MFL.exe 004f7150): the clump stream reader's geometry list cleanup (RenderWare 3.7 baclump.c name).
 * Written from the MFL.exe disassembly (lane P W8); RenderWare SDK types from rw37_slim.h. Compared with MFL.exe by differential
 * emulation (tier 2). */
#include "rw37_slim.h"

extern RwBool RpGeometryDestroy(RpGeometry *geometry);

typedef struct rpGeometryList rpGeometryList;
struct rpGeometryList
{
    RpGeometry        **geometries;
    RwInt32             numGeoms;
};
RW_LAYOUT_ASSERT(p_gl_0, sizeof(rpGeometryList) == 8 && RW_OFFSETOF(rpGeometryList, numGeoms) == 4);      /* MFL.exe 0x4f7158 [esi+4] count, 0x4f715f [esi] array */

/* TIER2: unvalidated 2026-11-14 authority=diffemu */
rpGeometryList * GeometryListDeinitialize(rpGeometryList * geomList)
{
    RwInt32 i;

    for (i = 0; i < geomList->numGeoms; i++)
    {
        RpGeometryDestroy(geomList->geometries[i]);
    }

    if (geomList->geometries)
    {
        RwFree(geomList->geometries);
        geomList->geometries = (RpGeometry **)NULL;
    }

    return (geomList);
}
