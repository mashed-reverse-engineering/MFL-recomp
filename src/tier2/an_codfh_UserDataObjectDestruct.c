/* UserDataObjectDestruct (MFL.exe 00554720). Adapted from codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32, src/rw/plugin/userdata/rpusrdat.c @e578672
 *, not byte-identical, changes: cut down to this function and the private declarations it uses,
 * UserDataListDestruct / UserDataDestruct inlined by hand (MFL.exe has them inlined), name and data pointers cleared after freeing as in
 * MFL.exe, release build macros expanded (RWFUNCTION / RWASSERT dropped, RWRETURN -> return), conditional compilation resolved for the
 * release D3D9 build, the SDK includes replaced by rw37_slim.h, data and functions MFL.exe names declared extern at their addresses.
 * Generated, then hand-checked; compared with MFL.exe by differential emulation (tier 2). */
#include "rw37_slim.h"

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
/* UserDataDestruct (static in rpusrdat.c, inlined in MFL.exe 0x554754..0x5547c3). MFL.exe's version also clears the name and data
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
RW_LAYOUT_ASSERT(rw37_own_0, RW_OFFSETOF(RpUserDataArray, format) == 4 && RW_OFFSETOF(RpUserDataArray, numElements) == 8 && RW_OFFSETOF(RpUserDataList, userData) == 4);      /* MFL.exe 0x554773 format [esi+4] == 3, 0x554779 numElements [esi+8]; 0x554731 list->userData [object+offset+4], 0x554745 numElements [object+offset] */

/* TIER2: unvalidated authority=diffemu */
void * UserDataObjectDestruct(void *object, RwInt32 offset, RwInt32 __RWUNUSED__ size)
{
    RpUserDataList  *userDataList;
    RwInt32          i;

    userDataList = OBJECTGETUSERDATALIST(object, offset);

    /* UserDataListDestruct(userDataList), inlined in MFL.exe */
    if (userDataList->userData != NULL)
    {
        for (i = 0; i < userDataList->numElements; ++i)
        {
            USERDATADESTRUCT(&userDataList->userData[i]);
        }

        RwFree(userDataList->userData);
    }

    userDataList->userData    = (RpUserDataArray *) NULL;
    userDataList->numElements = 0;

    return (object);
}
