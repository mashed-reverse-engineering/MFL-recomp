/* UserDataObjectStreamRead (MFL.exe 00554ac0). Adapted from codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32, src/rw/plugin/userdata/rpusrdat.c @e578672
 *, not byte-identical, changes: cut down to this function and the private declarations it uses,
 * UserDataListStreamRead / UserDataStreamRead inlined by hand (MFL.exe has them inlined), int and real arrays read with one
 * RwStreamReadInt32 call as in MFL.exe, release build macros expanded (RWFUNCTION / RWASSERT dropped, RWRETURN -> return), conditional
 * compilation resolved for the release D3D9 build, the SDK includes replaced by rw37_slim.h, data and functions MFL.exe names declared
 * extern at their addresses. Generated, then hand-checked; compared with MFL.exe by differential emulation (tier 2). */
#include "rw37_slim.h"
/* ---- SDK declarations only this row uses (generated together with rw37_slim.h: do not edit between the markers) ---- */
extern RwUInt32
RwStreamRead(RwStream * stream,
             void *buffer,
             RwUInt32 length);
#define RwStreamReadInt(_stream,_ints,_numBytes) RwStreamReadInt32(_stream, _ints, _numBytes)
extern RwStream *RwStreamReadInt32(RwStream *stream, RwInt32 *ints,
                                   RwUInt32 numBytes);
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
#define OBJECTGETUSERDATALIST(object,offset) ((RpUserDataList *)(((RwUInt8 *)object) + offset))
typedef struct RpUserDataList RpUserDataList;
struct RpUserDataList
{
    RwInt32             numElements;
    RpUserDataArray    *userData;
};
RW_LAYOUT_ASSERT(rw37_priv_0, sizeof(RpUserDataArray) == 0x10);      /* an_codfh_rpusrdat.c */
RW_LAYOUT_ASSERT(rw37_priv_1, RW_OFFSETOF(RpUserDataArray, data) == 0xc);      /* an_codfh_rpusrdat.c */
RW_LAYOUT_ASSERT(rw37_own_0, RW_OFFSETOF(RpUserDataArray, format) == 4 && RW_OFFSETOF(RpUserDataArray, numElements) == 8 && RW_OFFSETOF(RpUserDataList, userData) == 4);      /* MFL.exe 0x554b8d &format [esi+4], 0x554ba4 &numElements [esi+8]; 0x554b15 list->userData [list+4], 0x554b04 numElements [list+0] */

/* TIER2: unvalidated authority=diffemu */
RwStream * UserDataObjectStreamRead(RwStream * stream, RwInt32 __RWUNUSED__ binaryLength, void *object, RwInt32 offset, RwInt32 __RWUNUSED__ size)
{
    RpUserDataList     *list;

    list = OBJECTGETUSERDATALIST(object, offset);

    /* UserDataListStreamRead(list, stream) and UserDataStreamRead, static in rpusrdat.c, inlined in MFL.exe.  A failed entry makes the
     * stream NULL for the entries after it (the original behaviour); the function returns its stream argument either way. */
    if (list != NULL)
    {
        RwStream           *s = stream;
        RwInt32             i, numElements;

        if (RwStreamReadInt(s, &numElements, sizeof(RwInt32)))
        {
            list->numElements = numElements;

            list->userData = (RpUserDataArray *)
                RwMalloc(sizeof(RpUserDataArray) * list->numElements,
                         rwID_USERDATAPLUGIN | rwMEMHINTDUR_EVENT);

            for (i = 0; i < list->numElements; ++i)
            {
                RpUserDataArray    *userData = &list->userData[i];
                RwInt32             j, length;
                RwChar            **charData;

                do
                {
                    if (!RwStreamReadInt(s, &length, sizeof(RwInt32)))
                    {
                        s = (RwStream *) NULL;
                        break;
                    }

                    if (length > 0)
                    {
                        userData->name = (RwChar *) RwMalloc(sizeof(RwChar) * length,
                                             rwID_USERDATAPLUGIN | rwMEMHINTDUR_EVENT);
                        if (userData->name == NULL
                            || !RwStreamRead(s, userData->name, sizeof(RwChar) * length))
                        {
                            s = (RwStream *) NULL;
                            break;
                        }
                    }

                    if (!RwStreamReadInt(s, (RwInt32 *) &userData->format, sizeof(RwInt32))
                        || !RwStreamReadInt(s, &userData->numElements, sizeof(RwInt32)))
                    {
                        s = (RwStream *) NULL;
                        break;
                    }

                    switch (userData->format)
                    {
                        case rpINTUSERDATA:
                        case rpREALUSERDATA:
                            /* MFL.exe reads the whole array with one RwStreamReadInt32 call, for reals too (the codfh source: one call
                             * per element, RwStreamReadReal for reals) */
                            userData->data =
                                RwMalloc(sizeof(RwInt32) * userData->numElements,
                                         rwID_USERDATAPLUGIN | rwMEMHINTDUR_EVENT);
                            if (userData->data == NULL
                                || !RwStreamReadInt(s, (RwInt32 *) userData->data,
                                                    sizeof(RwInt32) * userData->numElements))
                            {
                                s = (RwStream *) NULL;
                            }
                            break;
                        case rpSTRINGUSERDATA:
                            userData->data =
                                RwMalloc(sizeof(RwChar *) * userData->numElements,
                                         rwID_USERDATAPLUGIN | rwMEMHINTDUR_EVENT);
                            if (userData->data == NULL)
                            {
                                s = (RwStream *) NULL;
                                break;
                            }

                            charData = (RwChar **) userData->data;

                            for (j = 0; j < userData->numElements; ++j)
                            {
                                if (!RwStreamReadInt(s, &length, sizeof(RwInt32)))
                                {
                                    s = (RwStream *) NULL;
                                    break;
                                }

                                if (length > 0)
                                {
                                    charData[j] =
                                        (RwChar *) RwMalloc(sizeof(RwChar) * length,
                                                            rwID_USERDATAPLUGIN |
                                                            rwMEMHINTDUR_EVENT);
                                    if (charData[j] == NULL
                                        || !RwStreamRead(s, charData[j], sizeof(RwChar) * length))
                                    {
                                        s = (RwStream *) NULL;
                                        break;
                                    }
                                }
                                else
                                {
                                    charData[j] = (RwChar *) NULL;
                                }
                            }
                            break;
                        default:
                            s = (RwStream *) NULL;
                            break;
                    }
                }
                while (0);
            }
        }
    }

    return (stream);
}
