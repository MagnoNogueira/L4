/* MODULE NAME:  L_base64.c
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

/* INCLUDE FILE DECLARATIONS
 */
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "sys_type.h"
#include "sys_module.h"
#include "l_mm.h"
#include "cmnlib_type.h"
#include "l_base64.h"

/* NAMING CONSTANT DECLARATIONS
 */

/* MACRO FUNCTION DECLARATIONS
 */
#define Assert(Cond) if (!(Cond)) return(-1)

/* DATA TYPE DECLARATIONS
 */

/* LOCAL SUBPROGRAM DECLARATIONS
 */
static int b64_ntop(UI8_T const *src, UI32_T srclength, UI8_T *target, UI32_T targsize);

static int b64_pton(UI8_T const *src,UI8_T *target, UI32_T targsize);

/* STATIC VARIABLE DECLARATIONS
 */
static const char Base64[] =
	"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
static const char Pad64 = '=';

/* EXPORTED SUBPROGRAM BODIES
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
 *          UI8_T   *in_data    --  input data.(MUST BE null-terminated)
 *          UI32_T  in_len      --  input data length.
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
BOOL_T L_Base64(UI8_T *in_data, UI32_T in_len, UI8_T **out_data, UI32_T *out_len, UI32_T transfer)
{
    /* LOCAL CONSTANT DECLARATIONS
    */

    /* LOCAL VARIABLES DECLARATIONS
    */
    UI8_T   *target_data = NULL;
    UI32_T  buf_len;

    /* BODY */
    *out_len = 0;
    *out_data = NULL;
    if( in_data == NULL )
    {
        return FALSE;
    }

    if( in_len <= 0 )
    {
        return FALSE;
    }

    if( transfer == L_BASE64_ENCODE )
    {
        buf_len = ((in_len+2)/3)*4 + 1; /* 3-byte group to 4-byte group */
    }
    else
    {
        buf_len = ((in_len+3)/4)*3 + 1; /* 4-byte group to 3-byte group */
    }

    target_data = (UI8_T *)L_MM_Malloc(buf_len, L_MM_USER_ID2(SYS_MODULE_CMNLIB, CMNLIB_TYPE_TRACE_ID_L_BASE64));
    if( target_data == NULL )
    {
        return FALSE;
    }

    if( transfer == L_BASE64_ENCODE )
    {
        *out_len = b64_ntop(in_data, in_len, target_data, buf_len);
        if((int)*out_len  == -1 )
        {
            L_MM_Free(target_data);
            *out_len = 0;
            *out_data = NULL;
            return FALSE;
        }
        else
        {
            *out_data = target_data;
            return TRUE;
        }
    }
    else if( transfer == L_BASE64_DECODE )
    {
    	UI32_T  len;
	    UI8_T   *encoded, *p;

    	/* copy the 'readonly' source */
    	len = strlen((char *)in_data) + 1;
	    encoded = (UI8_T *)L_MM_Malloc(len, L_MM_USER_ID2(SYS_MODULE_CMNLIB, CMNLIB_TYPE_TRACE_ID_L_BASE64));
	    if( encoded == NULL )
	    {
                L_MM_Free(target_data);
	        return FALSE;
	    }
    	strncpy((char *)encoded, (char *)in_data, len);
    	/* skip whitespace and data */
        for (p = encoded; (*p == ' ') || (*p == '\t'); p++)
    	{
	    	;
	    }
        for (; (*p != '\0') && (*p != ' ') && (*p != '\t'); p++)
    	{
	    	;
	    }
    	/* and remove trailing whitespace because __b64_pton needs this */
	    *p = '\0';
    	*out_len = b64_pton(encoded, target_data, buf_len);
        L_MM_Free(encoded);
    	if((int)*out_len == -1 )
    	{
            L_MM_Free(target_data);
            *out_len = 0;
            *out_data = NULL;
    	    return FALSE;
    	}
    	else
    	{
            *out_data = target_data;
            return TRUE;
    	}
    }
    else
    {
        return FALSE;
    }
}



/* (From RFC1521 and draft-ietf-dnssec-secext-03.txt)
   The following encoding technique is taken from RFC 1521 by Borenstein
   and Freed.  It is reproduced here in a slightly edited form for
   convenience.

   A 65-character subset of US-ASCII is used, enabling 6 bits to be
   represented per printable character. (The extra 65th character, "=",
   is used to signify a special processing function.)

   The encoding process represents 24-bit groups of input bits as output
   strings of 4 encoded characters. Proceeding from left to right, a
   24-bit input group is formed by concatenating 3 8-bit input groups.
   These 24 bits are then treated as 4 concatenated 6-bit groups, each
   of which is translated into a single digit in the base64 alphabet.

   Each 6-bit group is used as an index into an array of 64 printable
   characters. The character referenced by the index is placed in the
   output string.

                         Table 1: The Base64 Alphabet

      Value Encoding  Value Encoding  Value Encoding  Value Encoding
          0 A            17 R            34 i            51 z
          1 B            18 S            35 j            52 0
          2 C            19 T            36 k            53 1
          3 D            20 U            37 l            54 2
          4 E            21 V            38 m            55 3
          5 F            22 W            39 n            56 4
          6 G            23 X            40 o            57 5
          7 H            24 Y            41 p            58 6
          8 I            25 Z            42 q            59 7
          9 J            26 a            43 r            60 8
         10 K            27 b            44 s            61 9
         11 L            28 c            45 t            62 +
         12 M            29 d            46 u            63 /
         13 N            30 e            47 v
         14 O            31 f            48 w         (pad) =
         15 P            32 g            49 x
         16 Q            33 h            50 y

   Special processing is performed if fewer than 24 bits are available
   at the end of the data being encoded.  A full encoding quantum is
   always completed at the end of a quantity.  When fewer than 24 input
   bits are available in an input group, zero bits are added (on the
   right) to form an integral number of 6-bit groups.  Padding at the
   end of the data is performed using the '=' character.

   Since all base64 input is an integral number of octets, only the
         -------------------------------------------------
   following cases can arise:

       (1) the final quantum of encoding input is an integral
           multiple of 24 bits; here, the final unit of encoded
	   output will be an integral multiple of 4 characters
	   with no "=" padding,
       (2) the final quantum of encoding input is exactly 8 bits;
           here, the final unit of encoded output will be two
	   characters followed by two "=" padding characters, or
       (3) the final quantum of encoding input is exactly 16 bits;
           here, the final unit of encoded output will be three
	   characters followed by one "=" padding character.
   */

static int b64_ntop(UI8_T const *src, UI32_T srclength, UI8_T *target, UI32_T targsize)
{
	UI32_T datalength = 0;
	UI8_T input[3];
	UI8_T output[4];
	UI32_T i;

	while (2 < srclength) {
		input[0] = *src++;
		input[1] = *src++;
		input[2] = *src++;
		srclength -= 3;

		output[0] = input[0] >> 2;
		output[1] = ((input[0] & 0x03) << 4) + (input[1] >> 4);
		output[2] = ((input[1] & 0x0f) << 2) + (input[2] >> 6);
		output[3] = input[2] & 0x3f;
		Assert(output[0] < 64);
		Assert(output[1] < 64);
		Assert(output[2] < 64);
		Assert(output[3] < 64);

		if (datalength + 4 > targsize)
			return (-1);
		target[datalength++] = Base64[output[0]];
		target[datalength++] = Base64[output[1]];
		target[datalength++] = Base64[output[2]];
		target[datalength++] = Base64[output[3]];
	}

	/* Now we worry about padding. */
	if (0 != srclength) {
		/* Get what's left. */
		input[0] = input[1] = input[2] = '\0';
		for (i = 0; i < srclength; i++)
			input[i] = *src++;
	
		output[0] = input[0] >> 2;
		output[1] = ((input[0] & 0x03) << 4) + (input[1] >> 4);
		output[2] = ((input[1] & 0x0f) << 2) + (input[2] >> 6);
		Assert(output[0] < 64);
		Assert(output[1] < 64);
		Assert(output[2] < 64);

		if (datalength + 4 > targsize)
			return (-1);
		target[datalength++] = Base64[output[0]];
		target[datalength++] = Base64[output[1]];
		if (srclength == 1)
			target[datalength++] = Pad64;
		else
			target[datalength++] = Base64[output[2]];
		target[datalength++] = Pad64;
	}
	if (datalength >= targsize)
		return (-1);
	target[datalength] = '\0';	/* Returned value doesn't count \0. */
	return (datalength);
}



/* skips all whitespace anywhere.
   converts characters, four at a time, starting at (or after)
   src from base - 64 numbers into three 8 bit bytes in the target area.
   it returns the number of data bytes stored at the target, or -1 on error.
 */

static int b64_pton(UI8_T const *src,UI8_T *target, UI32_T targsize)
{
	int tarindex, state, ch;
	char *pos;

	state = 0;
	tarindex = 0;

	while ((ch = *src++) != '\0') {
		if (isspace(ch))	/* Skip whitespace anywhere. */
			continue;

		if (ch == Pad64)
			break;

		pos = strchr(Base64, ch);
		if (pos == 0) 		/* A non-base64 character. */
			return (-1);

		switch (state) {
		case 0:
			if (target) {
				if ((UI32_T)tarindex >= targsize)
					return (-1);
				target[tarindex] = (pos - Base64) << 2;
			}
			state = 1;
			break;
		case 1:
			if (target) {
				if ((UI32_T)tarindex + 1 >= targsize)
					return (-1);
				target[tarindex]   |=  (pos - Base64) >> 4;
				target[tarindex+1]  = ((pos - Base64) & 0x0f)
							<< 4 ;
			}
			tarindex++;
			state = 2;
			break;
		case 2:
			if (target) {
				if ((UI32_T)tarindex + 1 >= targsize)
					return (-1);
				target[tarindex]   |=  (pos - Base64) >> 2;
				target[tarindex+1]  = ((pos - Base64) & 0x03)
							<< 6;
			}
			tarindex++;
			state = 3;
			break;
		case 3:
			if (target) {
				if ((UI32_T)tarindex >= targsize)
					return (-1);
				target[tarindex] |= (pos - Base64);
			}
			tarindex++;
			state = 0;
			break;
		default:
			abort();
		}
	}

	/*
	 * We are done decoding Base-64 chars.  Let's see if we ended
	 * on a byte boundary, and/or with erroneous trailing characters.
	 */

	if (ch == Pad64) {		/* We got a pad char. */
		ch = *src++;		/* Skip it, get next. */
		switch (state) {
		case 0:		/* Invalid = in first position */
		case 1:		/* Invalid = in second position */
			return (-1);

		case 2:		/* Valid, means one byte of info */
			/* Skip any number of spaces. */
			for ((void)NULL; ch != '\0'; ch = *src++)
				if (!isspace(ch))
					break;
			/* Make sure there is another trailing = sign. */
			if (ch != Pad64)
				return (-1);
			ch = *src++;		/* Skip the = */
			/* Fall through to "single trailing =" case. */
			/* FALLTHROUGH */

		case 3:		/* Valid, means two bytes of info */
			/*
			 * We know this char is an =.  Is there anything but
			 * whitespace after it?
			 */
			for ((void)NULL; ch != '\0'; ch = *src++)
				if (!isspace(ch))
					return (-1);

			/*
			 * Now make sure for cases 2 and 3 that the "extra"
			 * bits that slopped past the last full byte were
			 * zeros.  If we don't check them, they become a
			 * subliminal channel.
			 */
			if (target && target[tarindex] != 0)
				return (-1);
		}
	} else {
		/*
		 * We ended by seeing the end of the string.  Make sure we
		 * have no partial bytes lying around.
		 */
		if (state != 0)
			return (-1);
	}

	return (tarindex);
}

