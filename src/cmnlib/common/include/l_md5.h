#ifndef _L_MD5_H
#define _L_MD5_H

#include <sys_type.h>
/* Digests a string and prints the result.
 */
/*  FUNCTION NAME : L_MD5_MDString
 *  PURPOSE:
 *      Encode a string by MD5.
 *  INPUT:
 *      string -- input string for digest
 *      len    -- input string length
 *
 *  OUTPUT:
 *      digest -- output digest string.
 *
 *  RETURN:
 *      None
 *
 *  NOTES:
 *      None.
 */
void L_MD5_MDString (UI8_T *digest, UI8_T *string, UI32_T len);

/*  FUNCTION NAME : L_MD5_HMAC_MD5
 *  PURPOSE:
 *      Get digest, a 16 octet signature of type HMAC-MD5 created from the date.
 *  INPUT:
 *      data        -- input date for digest
 *      data_len    -- input date length
 *      key         -- input key for digest
 *      key_len     -- input key length
 *
 *  OUTPUT:
 *      digest -- output digest (16 bytes).
 *
 *  RETURN:
 *      None
 *
 *  NOTES:
 *      None.
 */
void L_MD5_HMAC_MD5(UI8_T *data, UI32_T data_len, UI8_T *key, UI32_T key_len, UI8_T *digest);

#endif
