/* MODULE NAME:  L_base64.h
* PURPOSE:
*           This package provides a function to encode/decode use
*           Base64 Content-Transfer-Encoding algorithm.
*
* NOTES:
*
* History:
*       Date          -- Modifier,  Reason
*     2003-06-16      -- Isiah , created.
*
* Copyright(C)      Accton Corporation, 2003
*/

#ifndef L_BASE64_H

#define L_BASE64_H

/* INCLUDE FILE DECLARATIONS
 */
#include "sys_type.h"

/* NAMING CONSTANT DECLARATIONS
 */

/* MACRO FUNCTION DECLARATIONS
 */

/* DATA TYPE DECLARATIONS
 */
enum
{
    L_BASE64_DECODE = 0,
    L_BASE64_ENCODE
};

/* EXPORTED SUBPROGRAM SPECIFICATIONS
 */
/* FUNCTION NAME:  L_Base64
 * PURPOSE:
 *          This function to encode or decode data use
 *          Base64 Content-Transfer-Encoding algorithm.
 *          It always transforms the input data into the output data.
 *          If the encode_type argument is non-zero(L_BASE64_ENCODE), the
 *          input(unencoded data) is encrypted to the output(encoded data).
 *          If the encode_type is zero(L_BASE64_DECODE), the input(encoded data)
 *          is decoded into the output(unencoded data).
 *          The encoded data are consistently only about 33 percent larger than
 *          the unencoded data.
 *
 * INPUT:
 *          UI8_T   *in_data    --  input data.
 *          UI32_T  *in_len     --  input data length.
 *          UI32_T  transfer    --  tpye of transfer.(L_BASE64_ENCODE or L_BASE64_DECODE)
 *
 * OUTPUT:
 *          UI8_T   **out_data  --  output data(Dynamic alloc in L_Base64()).
 *          UI32_T  *out_len    --  output data length.
 *
 * RETURN:
 *          BOOL_T  --  TRUE    --  success.
 *                      FALSE   --  failure.
 *
 * NOTES:
 *          The caller must free memory resource of out_data, if L_Base64() return TRUE.
 *
 */
BOOL_T L_Base64(UI8_T *in_data, UI32_T in_len, UI8_T **out_data, UI32_T *out_len, UI32_T transfer);





#endif /* #ifndef L_BASE64_H */

