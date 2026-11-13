/* UserDataObjectCopy (MFL.exe 00554810). Adapted from codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32, src/rw/plugin/userdata/rpusrdat.c @e578672
 *, not byte-identical, changes: cut down to this function and the private declarations it uses,
 * UserDataListCopy / UserDataListDestruct / UserDataDestruct / UserDataCopy / RpUserDataGetFormatSize inlined by hand (MFL.exe has them
 * inlined), name and data pointers cleared after freeing as in MFL.exe, release build macros expanded (RWFUNCTION / RWASSERT dropped,
 * RWRETURN -> return), conditional compilation resolved for the release D3D9 build, the SDK includes replaced by rw37_slim.h, data and
 * functions MFL.exe names declared extern at their addresses. Generated, then hand-checked; compared with MFL.exe by differential emulation
 * (tier 2). */
#include "rw37_slim.h"
/* ---- SDK declarations only this row uses (generated together with rw37_slim.h: do not edit between the markers) ---- */
enum RwCorePluginID
{
    rwID_NAOBJECT = MAKECHUNKID(rwVENDORID_CORE, 0x00),
    rwID_STRUCT = MAKECHUNKID(rwVENDORID_CORE, 0x01),
    rwID_STRING = MAKECHUNKID(rwVENDORID_CORE, 0x02),
    rwID_EXTENSION = MAKECHUNKID(rwVENDORID_CORE, 0x03),

    rwID_CAMERA = MAKECHUNKID(rwVENDORID_CORE, 0x05),

    rwID_TEXTURE = MAKECHUNKID(rwVENDORID_CORE, 0x06),

    rwID_MATERIAL = MAKECHUNKID(rwVENDORID_CORE, 0x07),

    rwID_MATLIST = MAKECHUNKID(rwVENDORID_CORE, 0x08),
    rwID_ATOMICSECT = MAKECHUNKID(rwVENDORID_CORE, 0x09),
    rwID_PLANESECT = MAKECHUNKID(rwVENDORID_CORE, 0x0A),

    rwID_WORLD = MAKECHUNKID(rwVENDORID_CORE, 0x0B),

    rwID_SPLINE = MAKECHUNKID(rwVENDORID_CORE, 0x0C),

    rwID_MATRIX = MAKECHUNKID(rwVENDORID_CORE, 0x0D),

    rwID_FRAMELIST = MAKECHUNKID(rwVENDORID_CORE, 0x0E),

    rwID_GEOMETRY = MAKECHUNKID(rwVENDORID_CORE, 0x0F),

    rwID_CLUMP = MAKECHUNKID(rwVENDORID_CORE, 0x10),

    rwID_LIGHT = MAKECHUNKID(rwVENDORID_CORE, 0x12),

    rwID_UNICODESTRING = MAKECHUNKID(rwVENDORID_CORE, 0x13),

    rwID_ATOMIC = MAKECHUNKID(rwVENDORID_CORE, 0x14),

    rwID_TEXTURENATIVE = MAKECHUNKID(rwVENDORID_CORE, 0x15),

    rwID_TEXDICTIONARY = MAKECHUNKID(rwVENDORID_CORE, 0x16),

    rwID_ANIMDATABASE = MAKECHUNKID(rwVENDORID_CORE, 0x17),

    rwID_IMAGE = MAKECHUNKID(rwVENDORID_CORE, 0x18),

    rwID_SKINANIMATION = MAKECHUNKID(rwVENDORID_CORE, 0x19),
    rwID_GEOMETRYLIST = MAKECHUNKID(rwVENDORID_CORE, 0x1A),

    rwID_ANIMANIMATION = MAKECHUNKID(rwVENDORID_CORE, 0x1B),

    rwID_HANIMANIMATION = MAKECHUNKID(rwVENDORID_CORE, 0x1B),

    rwID_TEAM = MAKECHUNKID(rwVENDORID_CORE, 0x1C),

    rwID_CROWD = MAKECHUNKID(rwVENDORID_CORE, 0x1D),

    rwID_DMORPHANIMATION = MAKECHUNKID(rwVENDORID_CORE, 0x1E),

    rwID_RIGHTTORENDER = MAKECHUNKID(rwVENDORID_CORE, 0x1f),

    rwID_MTEFFECTNATIVE = MAKECHUNKID(rwVENDORID_CORE, 0x20),

    rwID_MTEFFECTDICT = MAKECHUNKID(rwVENDORID_CORE, 0x21),

    rwID_TEAMDICTIONARY = MAKECHUNKID(rwVENDORID_CORE, 0x22),

    rwID_PITEXDICTIONARY = MAKECHUNKID(rwVENDORID_CORE, 0x23),

    rwID_TOC = MAKECHUNKID(rwVENDORID_CORE, 0x24),

    rwID_PRTSTDGLOBALDATA = MAKECHUNKID(rwVENDORID_CORE, 0x25),

    rwID_ALTPIPE = MAKECHUNKID(rwVENDORID_CORE, 0x26),
    rwID_PIPEDS = MAKECHUNKID(rwVENDORID_CORE, 0x27),
    rwID_PATCHMESH = MAKECHUNKID(rwVENDORID_CORE, 0x28),

    rwID_CHUNKGROUPSTART = MAKECHUNKID(rwVENDORID_CORE, 0x29),
    rwID_CHUNKGROUPEND = MAKECHUNKID(rwVENDORID_CORE, 0x2A),

    rwID_UVANIMDICT = MAKECHUNKID(rwVENDORID_CORE, 0x2B),

    rwID_COLLTREE = MAKECHUNKID(rwVENDORID_CORE, 0x2C),

    rwID_ENVIRONMENT = MAKECHUNKID(rwVENDORID_CORE, 0x2D),

    rwID_COREPLUGINIDMAX = MAKECHUNKID(rwVENDORID_CORE, 0x2E),
    rwCOREPLUGINIDFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};
typedef enum RwCorePluginID RwCorePluginID ;
#define rwstrcpy RWSRCGLOBAL(stringFuncs).vecStrcpy
#define rwstrlen RWSRCGLOBAL(stringFuncs).vecStrlen
#define rwstrdup(_result,_string) do { _result = ((RwChar*)NULL); if (((RwChar*)NULL) != (_string)) { _result = (RwChar *) RwMalloc( (rwstrlen(_string) + 1) * sizeof (RwChar), rwID_STRING | rwMEMHINTDUR_EVENT); if (((RwChar*)NULL) != (_result)) { rwstrcpy(_result, _string); } } } while (0)
#define rwID_USERDATAPLUGIN MAKECHUNKID(rwVENDORID_CRITERIONTK, 0x1f)
/* ---- end of the generated declarations ---- */

enum RpUserDataFormat
{
    rpNAUSERDATAFORMAT = 0,
    rpINTUSERDATA,          
    rpREALUSERDATA,         
    rpSTRINGUSERDATA,       
    rpUSERDATAFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};
typedef enum RpUserDataFormat RpUserDataFormat;
typedef struct RpUserDataArray RpUserDataArray;
struct RpUserDataArray
{
    RwChar              *name;          
    RpUserDataFormat    format;         
    RwInt32             numElements;    
    void                *data;          
};
extern RwInt32          RpUserDataGetFormatSize(RpUserDataFormat format);
#define OBJECTGETUSERDATALIST(object,offset) ((RpUserDataList *)(((RwUInt8 *)object) + offset))
#define OBJECTGETCONSTUSERDATALIST(object,offset) ((const RpUserDataList *)(((const RwUInt8 *)object) + offset))
typedef struct RpUserDataList RpUserDataList;
struct RpUserDataList
{
    RwInt32             numElements;
    RpUserDataArray    *userData;
};
/* UserDataDestruct (static in rpusrdat.c, inlined in MFL.exe 0x554848..0x5548af). MFL.exe's version also clears the name and data
 * pointers after freeing them (the codfh source does not). */
#define USERDATADESTRUCT(_userData)                                     \
MACRO_START                                                             \
{                                                                       \
    if ((_userData)->name != NULL)                                      \
    {                                                                   \
        RwFree((_userData)->name);                                      \
        (_userData)->name = (RwChar *)NULL;                             \
    }                                                                   \
                                                                        \
    if ((_userData)->format == rpSTRINGUSERDATA)                        \
    {                                                                   \
        RwInt32             _i;                                         \
        RwChar            **_charData = (RwChar **) (_userData)->data;  \
                                                                        \
        for (_i = 0; _i < (_userData)->numElements; ++_i)               \
        {                                                               \
            if (_charData[_i] != NULL)                                  \
            {                                                           \
                RwFree(_charData[_i]);                                  \
            }                                                           \
        }                                                               \
    }                                                                   \
                                                                        \
    if ((_userData)->data != NULL)                                      \
    {                                                                   \
        RwFree((_userData)->data);                                      \
        (_userData)->data = NULL;                                       \
    }                                                                   \
}                                                                       \
MACRO_STOP

RW_LAYOUT_ASSERT(rw37_priv_0, sizeof(RpUserDataArray) == 0x10);      /* an_codfh_rpusrdat.c */
RW_LAYOUT_ASSERT(rw37_priv_1, RW_OFFSETOF(RpUserDataArray, data) == 0xc);      /* an_codfh_rpusrdat.c */
RW_LAYOUT_ASSERT(rw37_own_0, RW_OFFSETOF(RpUserDataArray, format) == 4 && RW_OFFSETOF(RpUserDataArray, numElements) == 8 && RW_OFFSETOF(RpUserDataList, userData) == 4);      /* MFL.exe 0x554958 format [esi+4], 0x55495e numElements [esi+8]; 0x55481f list->userData [object+offset+4], 0x5548fd numElements [object+offset] */

/* TIER2: unvalidated authority=diffemu */
void * UserDataObjectCopy(void *destinationObject, const void *sourceObject, RwInt32 offset, RwInt32 __RWUNUSED__ size)
{
    RpUserDataList     *dstList;
    const RpUserDataList *srcList;
    RwInt32             i;

    dstList = OBJECTGETUSERDATALIST(destinationObject, offset);
    srcList = OBJECTGETCONSTUSERDATALIST(sourceObject, offset);

    /* UserDataListCopy(dstList, srcList) with UserDataListDestruct, UserDataCopy and RpUserDataGetFormatSize, all inlined in MFL.exe */
    if (dstList->userData != NULL)
    {
        for (i = 0; i < dstList->numElements; ++i)
        {
            USERDATADESTRUCT(&dstList->userData[i]);
        }

        RwFree(dstList->userData);
    }
    dstList->userData    = (RpUserDataArray *) NULL;
    dstList->numElements = 0;

    dstList->numElements = srcList->numElements;

    if (dstList->numElements > 0)
    {
        dstList->userData = (RpUserDataArray *)
            RwMalloc(sizeof(RpUserDataArray) * dstList->numElements,
                     rwID_USERDATAPLUGIN | rwMEMHINTDUR_EVENT);

        for (i = 0; i < dstList->numElements; ++i)
        {
            RpUserDataArray    *dstUserData = &dstList->userData[i];
            RpUserDataArray    *srcUserData = &srcList->userData[i];
            RwInt32             dataSize;

            dstUserData->format = srcUserData->format;
            dstUserData->numElements = srcUserData->numElements;

            if (srcUserData->name != NULL)
            {
                rwstrdup(dstUserData->name, srcUserData->name);
            }

            if (srcUserData->data != NULL)
            {
                switch (dstUserData->format)      /* RpUserDataGetFormatSize */
                {
                    case rpINTUSERDATA:
                    case rpREALUSERDATA:
                    case rpSTRINGUSERDATA:
                        dataSize = 4;
                        break;
                    default:
                        dataSize = 0;
                        break;
                }
                dataSize = dstUserData->numElements * dataSize;

                dstUserData->data = RwMalloc(dataSize,
                    rwID_USERDATAPLUGIN | rwMEMHINTDUR_EVENT);

                if (dstUserData->format == rpSTRINGUSERDATA)
                {
                    RwInt32             j;
                    RwChar            **srcCharData =
                        (RwChar **) srcUserData->data;
                    RwChar            **dstCharData =
                        (RwChar **) dstUserData->data;

                    for (j = 0; j < dstUserData->numElements; ++j)
                    {
                        if (srcCharData[j] == NULL)
                        {
                            dstCharData[j] = (RwChar *) NULL;
                        }
                        else
                        {
                            rwstrdup(dstCharData[j], srcCharData[j]);
                        }
                    }
                }
                else
                {
                    memcpy(dstUserData->data, srcUserData->data, dataSize);
                }
            }
        }
    }

    return (destinationObject);
}
