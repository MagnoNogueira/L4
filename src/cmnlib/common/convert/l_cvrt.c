#include "l_cvrt.h"

const static UI8_T plist_in_octect_mask[] = {
    0,                  /* 0 port */
    BIT_7,
    BIT_7 | BIT_6,
    BIT_7 | BIT_6 | BIT_5,
    BIT_7 | BIT_6 | BIT_5 | BIT_4,
    BIT_7 | BIT_6 | BIT_5 | BIT_4 | BIT_3,
    BIT_7 | BIT_6 | BIT_5 | BIT_4 | BIT_3 | BIT_2,
    BIT_7 | BIT_6 | BIT_5 | BIT_4 | BIT_3 | BIT_2 | BIT_1,
    BIT_7 | BIT_6 | BIT_5 | BIT_4 | BIT_3 | BIT_2 | BIT_1 | BIT_0};

/* Byte flip convertion, bit 0 <-> bit 7; bit 1 <-> bit 6 etc
 */
unsigned char L_CVRT_ByteFlip (unsigned char v)
{
   /* NAMING CONSTANT
    */
   static const int byte_flip [] = {0, 8, 4, 12, 2, 10, 6, 14,
                                    1, 9, 5, 13, 3, 11, 7, 15};

   int hn;

   hn = v>> 4;
   return (unsigned char) ((byte_flip [(v & 15)] << 4) | byte_flip[hn]);
} /* L_CVRT_ByteFlip */


/*----------------------------------------------------------------------------------
 * Function name: L_CVRT_convert_portMap_2_portList
 *----------------------------------------------------------------------------------
 * Purpose: converts byte list to bit list, by putting value bit '1' in bit list
 *          if the corresponding byte in byte list has value 'match_value'
 *          , otherwise putting bit value '0'
 * Input  : byte_p         --- pointer to byte list
 *          nbr            --- item number of byte_p
 *          match_value    --- value to be compared
 * Output : bit_p          --- pointer to bit list
 * Return : none
 * Note   : 1. byte_p points to a buffer of length (in byte) 'nbr'
 *          2. bite_p points to a buffer of length (in byte) '(nbr+7)/8'
 *          3. caller should prepare buffers for both byte_p and bit_p
 *--------------------------------------------------------------------------------*/
void L_CVRT_convert_portMap_2_portList( UI8_T  *byte_p,
                                        UI8_T  *bit_p,
                                        UI32_T nbr,
                                        UI8_T  match_value )
{
   UI32_T   i;
   UI8_T    byte_value;
   int      NbrOfByte;
   int      byte, bit;

   NbrOfByte = (nbr+7)/8;

   for ( i=0, byte=0; byte < NbrOfByte; byte++, bit_p++ )
   {
      byte_value = 0;
      for(bit=0; bit<8; bit++, i++, byte_p++)
      {
         byte_value <<= 1;
         if ( i<nbr && (*byte_p == match_value ) )
            byte_value |= 1;
      }

      *bit_p = byte_value;
   } /* end for i */

} /* L_CVRT_convert_portMap_2_portList */


/*----------------------------------------------------------------------------------
 * Function name: L_CVRT_convert_portList_2_portMap
 *----------------------------------------------------------------------------------
 * Purpose: converts bit list to byte list, by putting value 'match_value' in byte
 *          list, if the corresponding bit in bit list has value '1'
 * Input  : bit_p         --- pointer to byte list
 *          nbr           --- item number of bit_p
 *          match_value   --- value to be put in byte list
 * Output : byte_p        --- pointer to bit list
 * Return : none
 * Note   : 1. bit_p points to a buffer of length (in byte) '(nbr+7)/8'
 *          2. byte_p points to a buffer of length (in byte) 'nbr'
 *          3. caller should prepare buffers for both byte_p and bit_p
 *--------------------------------------------------------------------------------*/
void L_CVRT_convert_portList_2_portMap( UI8_T  *bit_p,
                                        UI8_T  *byte_p,
                                        UI32_T nbr,
                                        UI8_T  match_value )
{
   UI32_T   i;
   UI8_T    byte_value;

   for (i=0; i < nbr; i++)
   {
      byte_value = bit_p[i/8]; // retrieve the target byte

      if ( byte_value & (0x01 << (7-(i % 8))) )
         byte_p[i] = match_value;
   } /* end for i */

} /* L_CVRT_convert_portList_2_portMap */

/*----------------------------------------------------------------------------------
 * Function name: L_CVRT_Plist_In_Octect_Mask
 *----------------------------------------------------------------------------------
 * Purpose: get plist value of number of ports in the octect
 * Input  : nbr_of_member         --- number of ports in the octect
 * Output : none
 * Return : plist value of number of ports in the octect
 * Note   : valid nbr_of_member is 0~8. otherwise -> return 0
 *--------------------------------------------------------------------------------*/

UI8_T L_CVRT_Plist_In_Octect_Mask(UI32_T nbr_of_member)
{
   if (nbr_of_member >= 9)
      return 0;
   else
      return plist_in_octect_mask[nbr_of_member];
} /* L_CVRT_Plist_In_Octect_Mask */




/*------------------------------------------------------------------------------
* Function : L_CVRT_8bitsInt_to_8bitsFloat
*------------------------------------------------------------------------------
* Purpose: This function calculate the 8 bits float point format
* INPUT  : value - the value to convert
* OUTPUT :None
* RETURN : converted value
* NOTES  : <=128 is value itself.
*------------------------------------------------------------------------------*/
UI8_T L_CVRT_8bitsInt_to_8bitsFloat ( UI16_T value)
{

    UI8_T code =0, exp =0;
    UI8_T mant = value;

    if(mant < 0x80)
        return mant;

    while (mant> 0x1f)
    {
    	exp ++;
    	mant = mant>> 1;
    }
    mant -= 0x10;
    exp  -= 3;

    code = 0x80 +   (exp << 4) + mant;

    return code;
}/*End of L_CVRT_8bitsInt_to_8bitsFloat*/


/*------------------------------------------------------------------------------
* Function : L_CVRT_16bitsInt_to_16BitsFloat
*------------------------------------------------------------------------------
* Purpose: This function calculate the 16 bits float point format
* INPUT  : value - the value to convert
* OUTPUT :None
* RETURN : converted value
* NOTES  :<=0x8000 is value itself.
*------------------------------------------------------------------------------*/
UI16_T L_CVRT_16bitsInt_to_16BitsFloat(UI16_T value)
{

    UI16_T code, mant = value;
    UI8_T  exp=0;

    if(mant < 0x8000)
        return mant;

    while (mant> 0x1fff)
    {
    	exp ++;
    	mant = mant>> 1;
    }
    mant -= 0x1000;
    exp -= 3;

    code = 0x8000 +   (exp << 12) + mant;

    return code;
}/*End of L_CVRT_16bitsInt_to_16BitsFloat*/











/*******************
 * end of L_cvrt.c *
 *******************/
