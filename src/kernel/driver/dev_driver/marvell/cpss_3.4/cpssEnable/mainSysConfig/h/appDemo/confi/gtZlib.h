/*******************************************************************************
*         Copyright 2004, MARVELL SEMICONDUCTOR ISRAEL, LTD.                   *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL.                      *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
*                                                                              *
* MARVELL COMPRISES MARVELL TECHNOLOGY GROUP LTD. (MTGL) AND ITS SUBSIDIARIES, *
* MARVELL INTERNATIONAL LTD. (MIL), MARVELL TECHNOLOGY, INC. (MTI), MARVELL    *
* SEMICONDUCTOR, INC. (MSI), MARVELL ASIA PTE LTD. (MAPL), MARVELL JAPAN K.K.  *
* (MJKK), MARVELL SEMICONDUCTOR ISRAEL. (MSIL),  MARVELL TAIWAN, LTD. AND      *
* SYSKONNECT GMBH.                                                             *
********************************************************************************
* gtZlib.h
*
* DESCRIPTION:
*       Encapsulates the zlib library API.
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*
*******************************************************************************/
#ifndef __gtZlibh
#define __gtZlibh

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <appDemo/confi/zconf.h>
#include <appDemo/confi/zlib.h>
#include <cpss/extServices/os/gtOs/gtGenTypes.h>

/*******************************************************************************
* gtCompress
*
* DESCRIPTION:
*   Compresses the source buffer into the destination buffer. The level
*   parameter has the same meaning as in deflateInit.  sourceLen is the byte
*   length of the source buffer. Upon entry, destLen is the total size of the
*   destination buffer, which must be at least 0.1% larger than sourceLen plus
*   12 bytes. Upon exit, destLen is the actual size of the compressed buffer.
*
* INPUTS:
*       dest      - pointer to destionation buffer
*       destLen   - destination buffer length
*       source    - source buffer
*       sourceLen - source buffer length
*
* OUTPUTS:
*       destLen   - actual size of compressed buffer
*
* RETURNS:
*       GT_OK   - if succeeded,
*       GT_FAIL - if failed
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS   gtCompress
(
    IN      GT_U8   *dest,
    INOUT   GT_U32  *destLen,
    IN      GT_U8   *source,
    IN      GT_U32  sourceLen
);


/*******************************************************************************
* gtUncompress
*
* DESCRIPTION:
*     Decompresses the source buffer into the destination buffer.  sourceLen is
*   the byte length of the source buffer. Upon entry, destLen is the total
*   size of the destination buffer, which must be large enough to hold the
*   entire uncompressed data. (The size of the uncompressed data must have
*   been saved previously by the compressor and transmitted to the decompressor
*   by some mechanism outside the scope of this compression library.)
*   Upon exit, destLen is the actual size of the compressed buffer.
*   This function can be used to decompress a whole file at once if the
*   input file is mmap'ed.
*
* INPUTS:
*       dest      - pointer to destionation buffer
*       destLen   - destination buffer length
*       source    - source buffer
*       sourceLen - source buffer length
*
* OUTPUTS:
*       destLen   - actual size of compressed buffer
*
* RETURNS:
*       GT_OK   - if succeeded,
*       GT_FAIL - if failed
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS   gtUncompress
(
    IN      GT_U8   *dest,
    INOUT   GT_U32  *destLen,
    IN      GT_U8   *source,
    IN      GT_U32  sourceLen
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __confiExecuterh */
