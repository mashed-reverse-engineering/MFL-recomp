/* rpusrdat plugin attach + stream writers (MFL.exe 0x00554470, 0x00554d10, 0x00554f20): RenderWare Graphics 3.7 (Criterion Software),
 * user data plugin: RpUserDataPluginAttach, UserDataObjectStreamWrite, UserDataObjectGetSize (the two callbacks are rw_trap rows,
 * ported so the attach does not reference int3 code).
 * Adapted from codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32, src/rw/plugin/userdata/rpusrdat.c @e578672,
 * not byte-identical, changes: RWFUNCTION / RWRETURN expanded (release build), sizeof(RpUserDataList) written as its exe value 8.
 * The world sector checks test the geometry offsets again, as in the SDK and the exe.  UserDataListStreamWrite / GetSize /
 * UserDataStreamWrite / GetSize inlined (as in the exe), with the 3.7 stream format of the exe: no count word when no array is
 * streamed (and then no list size either), int / real arrays written in one call.  Verified by tools/diffemu (tier 2). */
#include "mfl_tier2.h"

#define RW_OFFSETOF(type, member) ((char *)&((type *)0)->member - (char *)0)      /* pointer difference: no pointer-to-int cast */
#define RW_LAYOUT_ASSERT(name, cond) typedef char name[(sizeof(void *) != 4 || (cond)) ? 1 : -1]      /* cl 13.00 has no _Static_assert; the offsets are the 32-bit exe's */

#define TRUE 1
#define FALSE 0
#define rwID_USERDATAPLUGIN 0x11f
#define rpUSERDATALISTSIZE 8                /* sizeof(RpUserDataList): numElements, userData */

typedef enum RpUserDataFormat {
    rpNAUSERDATAFORMAT = 0,
    rpINTUSERDATA,
    rpREALUSERDATA,
    rpSTRINGUSERDATA
} RpUserDataFormat;
typedef struct RpUserDataArray {
    RwChar *name;
    RpUserDataFormat format;
    RwInt32 numElements;
    void *data;
} RpUserDataArray;
typedef struct RpUserDataList {
    RwInt32 numElements;
    RpUserDataArray *userData;
} RpUserDataList;
RW_LAYOUT_ASSERT(rw_assert_userdata_array, sizeof(RpUserDataArray) == 0x10 && RW_OFFSETOF(RpUserDataArray, data) == 0xc);
RW_LAYOUT_ASSERT(rw_assert_userdata_list, sizeof(RpUserDataList) == rpUSERDATALISTSIZE);

/*007d2128*/ extern void *RwEngineInstance;
/*007da774*/ extern RwInt32 userDataGeometryOffset;
/*007da778*/ extern RwInt32 userDataGeometryStreamOffset;
/*007da77c*/ extern RwInt32 userDataWorldSectorOffset;
/*007da780*/ extern RwInt32 userDataWorldSectorStreamOffset;
/*007da784*/ extern RwInt32 userDataFrameOffset;
/*007da788*/ extern RwInt32 userDataFrameStreamOffset;
/*007da78c*/ extern RwInt32 userDataCameraOffset;
/*007da790*/ extern RwInt32 userDataCameraStreamOffset;
/*007da794*/ extern RwInt32 userDataLightOffset;
/*007da798*/ extern RwInt32 userDataLightStreamOffset;
/*007da79c*/ extern RwInt32 userDataMaterialOffset;
/*007da7a0*/ extern RwInt32 userDataMaterialStreamOffset;
/*007da7a4*/ extern RwInt32 userDataTextureOffset;
/*007da7a8*/ extern RwInt32 userDataTextureStreamOffset;
/*004d2f20*/ extern RwInt32 RwEngineRegisterPlugin(RwInt32 size, RwUInt32 pluginID, void *initCB, void *termCB);
/*004f9110*/ extern RwInt32 RpGeometryRegisterPlugin(RwInt32 size, RwUInt32 pluginID, void *constructCB, void *destructCB, void *copyCB);
/*004f9140*/ extern RwInt32 RpGeometryRegisterPluginStream(RwUInt32 pluginID, void *readCB, void *writeCB, void *getSizeCB);
/*00500b40*/ extern RwInt32 RpWorldSectorRegisterPlugin(RwInt32 size, RwUInt32 pluginID, void *constructCB, void *destructCB, void *copyCB);
/*00500b70*/ extern RwInt32 RpWorldSectorRegisterPluginStream(RwUInt32 pluginID, void *readCB, void *writeCB, void *getSizeCB);
/*004d1830*/ extern RwInt32 RwFrameRegisterPlugin(RwInt32 size, RwUInt32 pluginID, void *constructCB, void *destructCB, void *copyCB);
/*005d5a60*/ extern RwInt32 RwFrameRegisterPluginStream(RwUInt32 pluginID, void *readCB, void *writeCB, void *getSizeCB);
/*004d1e50*/ extern RwInt32 RwCameraRegisterPlugin(RwInt32 size, RwUInt32 pluginID, void *constructCB, void *destructCB, void *copyCB);
/*005d5920*/ extern RwInt32 RwCameraRegisterPluginStream(RwUInt32 pluginID, void *readCB, void *writeCB, void *getSizeCB);
/*004f4cf0*/ extern RwInt32 RpLightRegisterPlugin(RwInt32 size, RwUInt32 pluginID, void *constructCB, void *destructCB, void *copyCB);
/*004f4d20*/ extern RwInt32 RpLightRegisterPluginStream(RwUInt32 pluginID, void *readCB, void *writeCB, void *getSizeCB);
/*004f8260*/ extern RwInt32 RpMaterialRegisterPlugin(RwInt32 size, RwUInt32 pluginID, void *constructCB, void *destructCB, void *copyCB);
/*004f8290*/ extern RwInt32 RpMaterialRegisterPluginStream(RwUInt32 pluginID, void *readCB, void *writeCB, void *getSizeCB);
#define RwTextureRegisterPlugin FUN_004d5f30      /* symbols.txt names of 0x4d5f30 / 0x4df1e0 (the overlay / link resolve through symbols.txt + aliases.txt) */
#define RwTextureRegisterPluginStream FUN_004df1e0
/*004d5f30*/ extern RwInt32 RwTextureRegisterPlugin(RwInt32 size, RwUInt32 pluginID, void *constructCB, void *destructCB, void *copyCB);
/*004df1e0*/ extern RwInt32 RwTextureRegisterPluginStream(RwUInt32 pluginID, void *readCB, void *writeCB, void *getSizeCB);
/*004dc8b0*/ extern RwStream *RwStreamWriteInt32(RwStream *stream, const void *ints, RwUInt32 numBytes);
/*004dc820*/ extern RwStream *RwStreamWriteReal(RwStream *stream, const void *reals, RwUInt32 numBytes);
/*004dbfc0*/ extern RwStream *RwStreamWrite(RwStream *stream, const void *buffer, RwUInt32 length);
/*005546e0*/ extern void *UserDataOpen(void *instance, RwInt32 offset, RwInt32 size);
/*005546f0*/ extern void *UserDataClose(void *instance, RwInt32 offset, RwInt32 size);
/*00554700*/ extern void *UserDataObjectConstruct(void *object, RwInt32 offset, RwInt32 size);
/*00554720*/ extern void *UserDataObjectDestruct(void *object, RwInt32 offset, RwInt32 size);
/*00554810*/ extern void *UserDataObjectCopy(void *dstObject, const void *srcObject, RwInt32 offset, RwInt32 size);
/*00554ac0*/ extern RwStream *UserDataObjectStreamRead(RwStream *stream, RwInt32 binaryLength, void *object, RwInt32 offsetInObject, RwInt32 sizeInObject);
RwStream *UserDataObjectStreamWrite(RwStream *stream, RwInt32 binaryLength, const void *object, RwInt32 offsetInObject, RwInt32 sizeInObject);
RwInt32 UserDataObjectGetSize(const void *object, RwInt32 offsetInObject, RwInt32 sizeInObject);

#define OBJECTGETCONSTUSERDATALIST(object, offset) ((const RpUserDataList *)((const RwUInt8 *)(object) + (offset)))

/* UserDataStreamWrite (inlined in the exe) */
#define USERDATA_STREAM_WRITE(userData, stream)                                                                 \
    do {                                                                                                        \
        RwInt32 j, length;                                                                                      \
        RwChar **charData;                                                                                      \
                                                                                                                \
        length = ((userData)->name != NULL) ? (RwInt32)rwstrlen((userData)->name) + 1 : 0;                      \
        if (!RwStreamWriteInt32((stream), &length, sizeof(RwInt32))                                             \
            || (length > 0 && !RwStreamWrite((stream), (userData)->name, sizeof(RwChar) * length))              \
            || !RwStreamWriteInt32((stream), &(userData)->format, sizeof(RwInt32))                              \
            || !RwStreamWriteInt32((stream), &(userData)->numElements, sizeof(RwInt32)))                        \
        {                                                                                                       \
            (stream) = NULL;                                                                                    \
            break;                                                                                              \
        }                                                                                                       \
        switch ((userData)->format)                                                                             \
        {                                                                                                       \
            case rpINTUSERDATA:                                                                                 \
                if (!RwStreamWriteInt32((stream), (userData)->data, sizeof(RwInt32) * (userData)->numElements)) \
                {                                                                                               \
                    (stream) = NULL;                                                                            \
                }                                                                                               \
                break;                                                                                          \
            case rpREALUSERDATA:                                                                                \
                if (!RwStreamWriteReal((stream), (userData)->data, sizeof(RwReal) * (userData)->numElements))   \
                {                                                                                               \
                    (stream) = NULL;                                                                            \
                }                                                                                               \
                break;                                                                                          \
            case rpSTRINGUSERDATA:                                                                              \
                charData = (RwChar **)(userData)->data;                                                         \
                for (j = 0; j < (userData)->numElements; ++j)                                                   \
                {                                                                                               \
                    length = (charData[j] != NULL) ? (RwInt32)rwstrlen(charData[j]) + 1 : 0;                    \
                    if (!RwStreamWriteInt32((stream), &length, sizeof(RwInt32))                                 \
                        || (length > 0 && !RwStreamWrite((stream), charData[j], sizeof(RwChar) * length)))      \
                    {                                                                                           \
                        (stream) = NULL;                                                                        \
                        break;                                                                                  \
                    }                                                                                           \
                }                                                                                               \
                break;                                                                                          \
            default:                                                                                            \
                (stream) = NULL;                                                                                \
                break;                                                                                          \
        }                                                                                                       \
    } while (0)

/* TIER2: unvalidated 2026-11-09 authority=diffemu */
RwStream *UserDataObjectStreamWrite(RwStream *stream, RwInt32 binaryLength, const void *object, RwInt32 offsetInObject, RwInt32 sizeInObject)
{
    const RpUserDataList *list = OBJECTGETCONSTUSERDATALIST(object, offsetInObject);
    RwStream *out = stream;
    RwUInt32 numElementsToStream = 0;
    RwInt32 i;

    /* UserDataListStreamWrite: its result is dropped, the object writer returns the stream it was given */
    if ((list != NULL) && (list->numElements > 0))
    {
        for (i = 0; i < list->numElements; ++i)
        {
            if (list->userData[i].format != rpNAUSERDATAFORMAT)
            {
                numElementsToStream++;
            }
        }

        if (numElementsToStream > 0 && RwStreamWriteInt32(out, &numElementsToStream, sizeof(RwInt32)))
        {
            for (i = 0; i < list->numElements; ++i)
            {
                if (list->userData[i].format != rpNAUSERDATAFORMAT)
                {
                    USERDATA_STREAM_WRITE(&list->userData[i], out);
                }
            }
        }
    }
    return stream;
}

/* TIER2: unvalidated 2026-11-09 authority=diffemu */
RwInt32 UserDataObjectGetSize(const void *object, RwInt32 offsetInObject, RwInt32 sizeInObject)
{
    const RpUserDataList *list = OBJECTGETCONSTUSERDATALIST(object, offsetInObject);
    RwInt32 size = 0;
    RwInt32 i, j;

    /* UserDataListGetSize */
    if ((list != NULL) && (list->numElements > 0))
    {
        for (i = 0; i < list->numElements; ++i)
        {
            const RpUserDataArray *userData;
            RwInt32 elementSize = 0;
            RwChar **charData;

            if (list->userData[i].format == rpNAUSERDATAFORMAT)
            {
                continue;
            }

            /* UserDataGetSize(&list->userData[i]) */
            userData = &list->userData[i];
            if (userData != NULL)
            {
                elementSize = sizeof(RwInt32);
                if (userData->name != NULL)
                {
                    elementSize += sizeof(RwChar) * ((RwInt32)rwstrlen(userData->name) + 1);
                }
                elementSize += 2 * sizeof(RwInt32);

                switch (userData->format)
                {
                    case rpINTUSERDATA:
                        elementSize += sizeof(RwInt32) * userData->numElements;
                        break;
                    case rpREALUSERDATA:
                        elementSize += sizeof(RwReal) * userData->numElements;
                        break;
                    case rpSTRINGUSERDATA:
                        charData = (RwChar **)userData->data;
                        for (j = 0; j < userData->numElements; ++j)
                        {
                            elementSize += sizeof(RwInt32);
                            if (charData[j] != NULL)
                            {
                                elementSize += sizeof(RwChar) * ((RwInt32)rwstrlen(charData[j]) + 1);
                            }
                        }
                        break;
                    default:
                        break;
                }
            }
            size += elementSize;
        }

        /* the count word, only when some array is streamed */
        if (size > 0)
        {
            size += sizeof(RwInt32);
        }
    }
    return size;
}

/* TIER2: validated 2026-11-12 authority=diffemu */
RwBool RpUserDataPluginAttach(void)
{
    RwInt32 offset;

    /* Register Engine plugin */
    offset = RwEngineRegisterPlugin(0, rwID_USERDATAPLUGIN, (void *)UserDataOpen, (void *)UserDataClose);
    if (offset < 0)
    {
        return FALSE;
    }

    /* Register RpGeometry plugin */
    userDataGeometryOffset = RpGeometryRegisterPlugin(rpUSERDATALISTSIZE, rwID_USERDATAPLUGIN, (void *)UserDataObjectConstruct, (void *)UserDataObjectDestruct, (void *)UserDataObjectCopy);
    if (userDataGeometryOffset < 0)
    {
        return FALSE;
    }
    userDataGeometryStreamOffset = RpGeometryRegisterPluginStream(rwID_USERDATAPLUGIN, (void *)UserDataObjectStreamRead, (void *)UserDataObjectStreamWrite, (void *)UserDataObjectGetSize);
    if (userDataGeometryStreamOffset < 0)
    {
        return FALSE;
    }

    /* Register RpWorldSector plugin */
    userDataWorldSectorOffset = RpWorldSectorRegisterPlugin(rpUSERDATALISTSIZE, rwID_USERDATAPLUGIN, (void *)UserDataObjectConstruct, (void *)UserDataObjectDestruct, (void *)UserDataObjectCopy);
    if (userDataGeometryOffset < 0)
    {
        return FALSE;
    }
    userDataWorldSectorStreamOffset = RpWorldSectorRegisterPluginStream(rwID_USERDATAPLUGIN, (void *)UserDataObjectStreamRead, (void *)UserDataObjectStreamWrite, (void *)UserDataObjectGetSize);
    if (userDataGeometryStreamOffset < 0)
    {
        return FALSE;
    }

    /* Register RwFrame plugin */
    userDataFrameOffset = RwFrameRegisterPlugin(rpUSERDATALISTSIZE, rwID_USERDATAPLUGIN, (void *)UserDataObjectConstruct, (void *)UserDataObjectDestruct, (void *)UserDataObjectCopy);
    if (userDataFrameOffset < 0)
    {
        return FALSE;
    }
    userDataFrameStreamOffset = RwFrameRegisterPluginStream(rwID_USERDATAPLUGIN, (void *)UserDataObjectStreamRead, (void *)UserDataObjectStreamWrite, (void *)UserDataObjectGetSize);
    if (userDataFrameStreamOffset < 0)
    {
        return FALSE;
    }

    /* Register RwCamera plugin */
    userDataCameraOffset = RwCameraRegisterPlugin(rpUSERDATALISTSIZE, rwID_USERDATAPLUGIN, (void *)UserDataObjectConstruct, (void *)UserDataObjectDestruct, (void *)UserDataObjectCopy);
    if (userDataCameraOffset < 0)
    {
        return FALSE;
    }
    userDataCameraStreamOffset = RwCameraRegisterPluginStream(rwID_USERDATAPLUGIN, (void *)UserDataObjectStreamRead, (void *)UserDataObjectStreamWrite, (void *)UserDataObjectGetSize);
    if (userDataCameraStreamOffset < 0)
    {
        return FALSE;
    }

    /* Register RpLight plugin */
    userDataLightOffset = RpLightRegisterPlugin(rpUSERDATALISTSIZE, rwID_USERDATAPLUGIN, (void *)UserDataObjectConstruct, (void *)UserDataObjectDestruct, (void *)UserDataObjectCopy);
    if (userDataLightOffset < 0)
    {
        return FALSE;
    }
    userDataLightStreamOffset = RpLightRegisterPluginStream(rwID_USERDATAPLUGIN, (void *)UserDataObjectStreamRead, (void *)UserDataObjectStreamWrite, (void *)UserDataObjectGetSize);
    if (userDataLightStreamOffset < 0)
    {
        return FALSE;
    }

    /* Register RpMaterial plugin */
    userDataMaterialOffset = RpMaterialRegisterPlugin(rpUSERDATALISTSIZE, rwID_USERDATAPLUGIN, (void *)UserDataObjectConstruct, (void *)UserDataObjectDestruct, (void *)UserDataObjectCopy);
    if (userDataMaterialOffset < 0)
    {
        return FALSE;
    }
    userDataMaterialStreamOffset = RpMaterialRegisterPluginStream(rwID_USERDATAPLUGIN, (void *)UserDataObjectStreamRead, (void *)UserDataObjectStreamWrite, (void *)UserDataObjectGetSize);
    if (userDataMaterialStreamOffset < 0)
    {
        return FALSE;
    }

    /* Register RwTexture plugin */
    userDataTextureOffset = RwTextureRegisterPlugin(rpUSERDATALISTSIZE, rwID_USERDATAPLUGIN, (void *)UserDataObjectConstruct, (void *)UserDataObjectDestruct, (void *)UserDataObjectCopy);
    if (userDataTextureOffset < 0)
    {
        return FALSE;
    }
    userDataTextureStreamOffset = RwTextureRegisterPluginStream(rwID_USERDATAPLUGIN, (void *)UserDataObjectStreamRead, (void *)UserDataObjectStreamWrite, (void *)UserDataObjectGetSize);
    if (userDataTextureStreamOffset < 0)
    {
        return FALSE;
    }
    return TRUE;
}
