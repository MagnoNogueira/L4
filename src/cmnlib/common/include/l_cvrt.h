#ifndef _L_CVRT_H
#define _L_CVRT_H

#include "sys_type.h"
#include "sys_cpnt.h"

/* EXPORTED MACRO FUNCTIONS DECLARATION
 */
 
/* Macro for Port List operations
  *
  * Users have to be sure the correctness of input parameters: 
  *    such as port_number must be > 0.
  * Note that port#1, #9, #17,.. are presented by bit#7 of their corresponding bytes
  *               port#2,#10,#18,... are presented by bit#6 of their corresponding bytes
  *               ...
  *               port#8,#16,#24,... are presented by bit#0 of their corresponding bytes
  */
#define L_CVRT_BYTE_INDEX_OF_PORTLIST(port_number)                ((port_number-1) >> 3)
#define L_CVRT_BIT_INDEX_IN_BYTE_OF_PORTLIST(port_number)   (7 - ((port_number-1) & 7))

#define L_CVRT_IS_MEMBER_OF_PORTLIST(port_list, port_number) \
        ((port_list[L_CVRT_BYTE_INDEX_OF_PORTLIST(port_number)] & (1 << L_CVRT_BIT_INDEX_IN_BYTE_OF_PORTLIST(port_number))) != 0)
#define L_CVRT_IS_MEMBER_OF_PORTLIST_BY_BYTE_BIT_INDEX(port_list, byte_index, bit_index) \
        ((port_list[byte_index] & (1 << bit_index)) != 0)
        
#define L_CVRT_ADD_MEMBER_TO_PORTLIST(port_list, port_number) \
        (port_list[L_CVRT_BYTE_INDEX_OF_PORTLIST(port_number)] |= (1 << L_CVRT_BIT_INDEX_IN_BYTE_OF_PORTLIST(port_number)))
#define L_CVRT_ADD_MEMBER_TO_PORTLIST_BY_BYTE_BIT_INDEX(port_list, byte_index, bit_index) \
        (port_list[byte_index] |= (1 << bit_index))
        
#define L_CVRT_DEL_MEMBER_FROM_PORTLIST(port_list, port_number) \
	(port_list[L_CVRT_BYTE_INDEX_OF_PORTLIST(port_number)] &= (~(1 << L_CVRT_BIT_INDEX_IN_BYTE_OF_PORTLIST(port_number))))
#define L_CVRT_DEL_MEMBER_FROM_PORTLIST_BY_BYTE_BIT_INDEX(port_list, byte_index, bit_index) \
	(port_list[byte_index] &= (~(1 << bit_index)))


#define L_BYTE_FLIP(X)     L_CVRT_ByteFlip(X)

#define L_ALIGN(addr,boundary) (  ( (UI32_T)(addr) + (boundary) - 1) & ~((boundary) - 1))

/*
 * If OM is UI32 but MIB is UI16,
 * there are four solutions for this (from earlier to more time-consuming) -- assuming SNMP shall have done hton() before sending out packets.
 * 1. keep current design and databse size (lower layer UI32; core layer UI16), core layer shall get the right UI16 by shift >>16
 * 2. both lower layer and corelayer use UI32, SNMP gets the first two bytes to send out after hton()
 * 3. both lower layer and corelayer use UI16
 * 4. use byte array instead of integer...
 * The 4th solution, using byte array, is the preferred solution -- just like our port list which can support larger bitmap
 *
 * For UI32 case:
 * The definition: #define L_CVRT_SNMP_BIT_VALUE_32(N) (1<<(31-(N))) is in host order's view to move bit#0 to bit#31, bit#1 to bit#30,..
 *    --> in big endian system: from low byte to high byte becomes bit#0,1,2,3,4,...
 *        in little endian system: from high byte to low byte becomes bit#0,1,2,3,4..
 * using hton32() will let the lowest byte(bit#0,1,..) be sent out first.
 *
 * Extra note: 
 * Network order is big endian. 
 *          This means the most significant byte of an integer field is placed at a lower wire byte address 
 *          and transmitted/received in front of the least significant byte.
 */
 
#if (SYS_CPNT_SNMP_BITS_FROM_LEFT == TRUE)
#define L_CVRT_SNMP_BIT_VALUE_32(N) (1<<(31-(N)))
#else
#define L_CVRT_SNMP_BIT_VALUE_32(N) (1<<(N))
#endif

/* Convert between pointer and offset according to base address.
 *
 *  int  L_CVRT_GET_OFFSET (void *base_p, void *ptr)
 *  void  *L_CVRT_GET_PTR (void *base_p, int offset)
 */
#define L_CVRT_GET_OFFSET(base_p, ptr) ((UI32_T)(ptr) - (UI32_T)(base_p))
#define L_CVRT_GET_PTR(base_p, offset) ((void *)((UI8_T *)(base_p) + (int)(offset)))


/* EXPORTED FUNCTIONS DECLARATION
 */


/* Byte flip convertion, bit 0 <-> bit 7; bit 1 <-> bit 6 etc
 */
unsigned char L_CVRT_ByteFlip (unsigned char v);


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
                                        UI8_T  match_value );


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
                                        UI8_T  match_value );

/*----------------------------------------------------------------------------------
 * Function name: L_CVRT_Plist_In_Octect_Mask
 *----------------------------------------------------------------------------------
 * Purpose: get plist value of number of ports in the octect
 * Input  : nbr_of_member         --- number of ports in the octect
 * Output : none
 * Return : plist value of number of ports in the octect
 * Note   : valid nbr_of_member is 0~8. otherwise -> return 0
 *--------------------------------------------------------------------------------*/
UI8_T L_CVRT_Plist_In_Octect_Mask(UI32_T nbr_of_member);

/*------------------------------------------------------------------------------
* Function : L_CVRT_8bitsInt_to_8bitsFloat
*------------------------------------------------------------------------------
* Purpose: This function calculate the 8 bits float point format
* INPUT  : value - the value to convert
* OUTPUT :None
* RETURN : converted value
* NOTES  : <=128 is value itself.
*------------------------------------------------------------------------------*/
UI8_T L_CVRT_8bitsInt_to_8bitsFloat ( UI16_T value);


/*------------------------------------------------------------------------------
* Function : L_CVRT_16bitsInt_to_16BitsFloat
*------------------------------------------------------------------------------
* Purpose: This function calculate the 16 bits float point format
* INPUT  : value - the value to convert
* OUTPUT :None
* RETURN : converted value
* NOTES  :<=0x8000 is value itself.
*------------------------------------------------------------------------------*/
UI16_T L_CVRT_16bitsInt_to_16BitsFloat(UI16_T value);

#endif
