/* MODULE NAME: l_bitmap.h
 * PURPOSE:
 *  This module provides operations related to bit map manipulation.
 *
 * NOTES:
 *
 * REASON:
 * Description:
 * CREATOR:      Ryan
 * HISTORY
 *    11/3/2005 - Ryan, Created
 *    11/3/2005 - Charlie Chen, Modified
 *
 * Copyright(C)      Accton Corporation, 2005
 */
#ifndef L_BITMAP_H
#define L_BITMAP_H

/* INCLUDE FILE DECLARATIONS
 */
#include "sys_type.h"

/* NAMING CONSTANT DECLARATIONS
 */

/* MACRO FUNCTION DECLARATIONS
 */
/*------------------------------------------------------------------------------
 * MACRO NAME - L_BITMAP_SET_INDEX
 *------------------------------------------------------------------------------
 * PURPOSE:  For setting corresponding bit(ie. set as 1) of bitmap on according
 *           to specified index.
 * INPUT:    bitmap  -  bitmap to be set
 *           index   -  The index of the bit to be set at bitmap
 * OUTPUT:   None
 * RETURN:   None.
 * NOTE:     1. Cares must be taken on valid range of index, or buffer overflow
 *              might occur. For example, valid range of index for UI32_T bitmap[2]
 *              is 0~63.
 *           2. bitmap must be UI32_T array. or UI32_T*.
 *------------------------------------------------------------------------------
 */
#define L_BITMAP_SET_INDEX(bitmap, index)           \
    (bitmap) [index >> 5] |= BIT_VALUE(index&31)

/*------------------------------------------------------------------------------
 * MACRO NAME - L_BITMAP_RESET_INDEX
 *------------------------------------------------------------------------------
 * PURPOSE:  For resetting corresponding bit(ie. set as 0) of bitmap on according
 *           to specified index.
 * INPUT:    bitmap  -  bitmap to be reset
 *           index   -  The index of the bit to be reset at bitmap
 * OUTPUT:   None
 * RETURN:   None.
 * NOTE:     1. Cares must be taken on valid range of index, or buffer overflow
 *              might occur. For example, valid range of index for UI32_T bitmap[2]
 *              is 0~63.
 *           2. bitmap must be UI32_T array. or UI32_T*.
 *------------------------------------------------------------------------------
 */
#define L_BITMAP_RESET_INDEX(bitmap, index)        \
    (bitmap) [index >> 5] &= ~BIT_VALUE(index&31)


#define L_BITMAP_ISSET_INDEX(bitmap, index)        \
    ((bitmap) [index >> 5] & BIT_VALUE(index & 31))

#define L_BITMAP_PORT_ISSET(bitmap, port) (((bitmap)[(port -1) / 8] & (0x80 >> ((port -1) % 8))) != 0)

#define L_BITMAP_PORT_SET(bitmap, port) do {((bitmap)[(port -1) / 8] |= (0x80 >> ((port -1) % 8)));}while(0)

#define L_BITMAP_PORT_UNSET(bitmap, port) do {((bitmap)[(port -1) / 8] &= ~( 0x80 >> ((port -1) % 8)));}while(0)


#define CHECK_FLAG(V,F)      ((V) & (F))
#define SET_FLAG(V,F)        (V) = (V) | (F)
#define UNSET_FLAG(V,F)      (V) = (V) & ~(F)
#define FLAG_ISSET(V,F)      (((V) & (F)) == (F))

/* DATA TYPE DECLARATIONS
 */

/* EXPORTED SUBPROGRAM SPECIFICATIONS
 */
/*------------------------------------------------------------------------------
 * ROUTINE NAME - L_BITMAP_Get_BitPos_List
 *------------------------------------------------------------------------------
 * PURPOSE:  Get the list of bit position from the given bitmap.
 * INPUT:
 *           bitmap       - bitmap to be used to generate the list
 * OUTPUT:
 *           bit_pos_list - list of bit position will be put in this array
 *
 * RETURN:   Number of bit position in bit_pos_list
 * NOTE:
 *           1. The bit position 0 is at least significant bit of least
 *              significant byte.
 *------------------------------------------------------------------------------
 */
UI32_T 
L_BITMAP_Get_BitPos_List (UI32_T bitmap, UI8_T bit_pos_list[32]);

BOOL_T
L_BITMAP_is_null(UI8_T *pbmp);

BOOL_T
L_BITMAP_port_is_set(UI8_T *pbmp, UI32_T port_num);

void
L_BITMAP_port_set(UI8_T *pbmp, UI32_T port_num);

void
L_BITMAP_port_unset(UI8_T *pbmp, UI32_T port_num);

void
L_BITMAP_bmp_xor(UI8_T *pbmp_dst, UI8_T *pbmp_src);

#endif    /* End of L_BITMAP_H */
