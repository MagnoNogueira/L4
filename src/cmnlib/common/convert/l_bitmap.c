/* MODULE NAME:  l_bitmap.c
 * PURPOSE:
 * This module provides operations related to bit map manipulation.
 *
 * NOTES:
 *
 * REASON:
 * CREATOR:      Ryan
 * HISTORY
 *    11/3/2005 - Ryan, Created
 *    11/3/2005 - Charlie Chen, Modified
 *
 * Copyright(C)      Accton Corporation, 2005
 */

/* INCLUDE FILE DECLARATIONS
 */
#include <string.h> /* for memcpy() */
#include <assert.h>
#include "sys_type.h"
#include "sys_adpt.h"
#include "sys_hwcfg.h"
#include "l_bitmap.h"

/* NAMING CONSTANT DECLARATIONS
 */
#ifdef SYS_HWCFG_LITTLE_ENDIAN_CPU
    #define  BYTE0      0
    #define  BYTE1      1
    #define  BYTE2      2
    #define  BYTE3      3
#else
    #define  BYTE0      3
    #define  BYTE1      2
    #define  BYTE2      1
    #define  BYTE3      0
#endif

/* MACRO FUNCTION DECLARATIONS
 */

/* DATA TYPE DECLARATIONS
 */
typedef struct
{
    UI32_T  count;
    UI8_T   blist[8];
} BitPosList_T;

/* LOCAL SUBPROGRAM DECLARATIONS
 */

/* STATIC VARIABLE DECLARATIONS
 */
const static BitPosList_T bit_pos_tab[256] =
{
    {0,/* 0 */ {0} },
    {1,/* 1 */ {0} },
    {1,/* 2 */ {1} },
    {2,/* 3 */ {0,1} },
    {1,/* 4 */ {2} },
    {2,/* 5 */ {0,2} },
    {2,/* 6 */ {1,2} },
    {3,/* 7 */ {0,1,2} },
    {1,/* 8 */ {3} },
    {2,/* 9 */ {0,3} },
    {2,/* A */ {1,3} },
    {3,/* B */ {0,1,3} },
    {2,/* C */ {2,3} },
    {3,/* D */ {0,2,3} },
    {3,/* E */ {1,2,3} },
    {4,/* F */ {0,1,2,3} },
    {1,/* 10 */ {4} },
    {2,/* 11 */ {0,4} },
    {2,/* 12 */ {1,4} },
    {3,/* 13 */ {0,1,4} },
    {2,/* 14 */ {2,4} },
    {3,/* 15 */ {0,2,4} },
    {3,/* 16 */ {1,2,4} },
    {4,/* 17 */ {0,1,2,4} },
    {2,/* 18 */ {3,4} },
    {3,/* 19 */ {0,3,4} },
    {3,/* 1A */ {1,3,4} },
    {4,/* 1B */ {0,1,3,4} },
    {3,/* 1C */ {2,3,4} },
    {4,/* 1D */ {0,2,3,4} },
    {4,/* 1E */ {1,2,3,4} },
    {5,/* 1F */ {0,1,2,3,4} },
    {1,/* 20 */ {5} },
    {2,/* 21 */ {0,5} },
    {2,/* 22 */ {1,5} },
    {3,/* 23 */ {0,1,5} },
    {2,/* 24 */ {2,5} },
    {3,/* 25 */ {0,2,5} },
    {3,/* 26 */ {1,2,5} },
    {4,/* 27 */ {0,1,2,5} },
    {2,/* 28 */ {3,5} },
    {3,/* 29 */ {0,3,5} },
    {3,/* 2A */ {1,3,5} },
    {4,/* 2B */ {0,1,3,5} },
    {3,/* 2C */ {2,3,5} },
    {4,/* 2D */ {0,2,3,5} },
    {4,/* 2E */ {1,2,3,5} },
    {5,/* 2F */ {0,1,2,3,5} },
    {2,/* 30 */ {4,5} },
    {3,/* 31 */ {0,4,5} },
    {3,/* 32 */ {1,4,5} },
    {4,/* 33 */ {0,1,4,5} },
    {3,/* 34 */ {2,4,5} },
    {4,/* 35 */ {0,2,4,5} },
    {4,/* 36 */ {1,2,4,5} },
    {5,/* 37 */ {0,1,2,4,5} },
    {3,/* 38 */ {3,4,5} },
    {4,/* 39 */ {0,3,4,5} },
    {4,/* 3A */ {1,3,4,5} },
    {5,/* 3B */ {0,1,3,4,5} },
    {4,/* 3C */ {2,3,4,5} },
    {5,/* 3D */ {0,2,3,4,5} },
    {5,/* 3E */ {1,2,3,4,5} },
    {6,/* 3F */ {0,1,2,3,4,5} },
    {1,/* 40 */ {6} },
    {2,/* 41 */ {0,6} },
    {2,/* 42 */ {1,6} },
    {3,/* 43 */ {0,1,6} },
    {2,/* 44 */ {2,6} },
    {3,/* 45 */ {0,2,6} },
    {3,/* 46 */ {1,2,6} },
    {4,/* 47 */ {0,1,2,6} },
    {2,/* 48 */ {3,6} },
    {3,/* 49 */ {0,3,6} },
    {3,/* 4A */ {1,3,6} },
    {4,/* 4B */ {0,1,3,6} },
    {3,/* 4C */ {2,3,6} },
    {4,/* 4D */ {0,2,3,6} },
    {4,/* 4E */ {1,2,3,6} },
    {5,/* 4F */ {0,1,2,3,6} },
    {2,/* 50 */ {4,6} },
    {3,/* 51 */ {0,4,6} },
    {3,/* 52 */ {1,4,6} },
    {4,/* 53 */ {0,1,4,6} },
    {3,/* 54 */ {2,4,6} },
    {4,/* 55 */ {0,2,4,6} },
    {4,/* 56 */ {1,2,4,6} },
    {5,/* 57 */ {0,1,2,4,6} },
    {3,/* 58 */ {3,4,6} },
    {4,/* 59 */ {0,3,4,6} },
    {4,/* 5A */ {1,3,4,6} },
    {5,/* 5B */ {0,1,3,4,6} },
    {4,/* 5C */ {2,3,4,6} },
    {5,/* 5D */ {0,2,3,4,6} },
    {5,/* 5E */ {1,2,3,4,6} },
    {6,/* 5F */ {0,1,2,3,4,6} },
    {2,/* 60 */ {5,6} },
    {3,/* 61 */ {0,5,6} },
    {3,/* 62 */ {1,5,6} },
    {4,/* 63 */ {0,1,5,6} },
    {3,/* 64 */ {2,5,6} },
    {4,/* 65 */ {0,2,5,6} },
    {4,/* 66 */ {1,2,5,6} },
    {5,/* 67 */ {0,1,2,5,6} },
    {3,/* 68 */ {3,5,6} },
    {4,/* 69 */ {0,3,5,6} },
    {4,/* 6A */ {1,3,5,6} },
    {5,/* 6B */ {0,1,3,5,6} },
    {4,/* 6C */ {2,3,5,6} },
    {5,/* 6D */ {0,2,3,5,6} },
    {5,/* 6E */ {1,2,3,5,6} },
    {6,/* 6F */ {0,1,2,3,5,6} },
    {3,/* 70 */ {4,5,6} },
    {4,/* 71 */ {0,4,5,6} },
    {4,/* 72 */ {1,4,5,6} },
    {5,/* 73 */ {0,1,4,5,6} },
    {4,/* 74 */ {2,4,5,6} },
    {5,/* 75 */ {0,2,4,5,6} },
    {5,/* 76 */ {1,2,4,5,6} },
    {6,/* 77 */ {0,1,2,4,5,6} },
    {4,/* 78 */ {3,4,5,6} },
    {5,/* 79 */ {0,3,4,5,6} },
    {5,/* 7A */ {1,3,4,5,6} },
    {6,/* 7B */ {0,1,3,4,5,6} },
    {5,/* 7C */ {2,3,4,5,6} },
    {6,/* 7D */ {0,2,3,4,5,6} },
    {6,/* 7E */ {1,2,3,4,5,6} },
    {7,/* 7F */ {0,1,2,3,4,5,6} },
    {1,/* 80 */ {7} },
    {2,/* 81 */ {0,7} },
    {2,/* 82 */ {1,7} },
    {3,/* 83 */ {0,1,7} },
    {2,/* 84 */ {2,7} },
    {3,/* 85 */ {0,2,7} },
    {3,/* 86 */ {1,2,7} },
    {4,/* 87 */ {0,1,2,7} },
    {2,/* 88 */ {3,7} },
    {3,/* 89 */ {0,3,7} },
    {3,/* 8A */ {1,3,7} },
    {4,/* 8B */ {0,1,3,7} },
    {3,/* 8C */ {2,3,7} },
    {4,/* 8D */ {0,2,3,7} },
    {4,/* 8E */ {1,2,3,7} },
    {5,/* 8F */ {0,1,2,3,7} },
    {2,/* 90 */ {4,7} },
    {3,/* 91 */ {0,4,7} },
    {3,/* 92 */ {1,4,7} },
    {4,/* 93 */ {0,1,4,7} },
    {3,/* 94 */ {2,4,7} },
    {4,/* 95 */ {0,2,4,7} },
    {4,/* 96 */ {1,2,4,7} },
    {5,/* 97 */ {0,1,2,4,7} },
    {3,/* 98 */ {3,4,7} },
    {4,/* 99 */ {0,3,4,7} },
    {4,/* 9A */ {1,3,4,7} },
    {5,/* 9B */ {0,1,3,4,7} },
    {4,/* 9C */ {2,3,4,7} },
    {5,/* 9D */ {0,2,3,4,7} },
    {5,/* 9E */ {1,2,3,4,7} },
    {6,/* 9F */ {0,1,2,3,4,7} },
    {2,/* A0 */ {5,7} },
    {3,/* A1 */ {0,5,7} },
    {3,/* A2 */ {1,5,7} },
    {4,/* A3 */ {0,1,5,7} },
    {3,/* A4 */ {2,5,7} },
    {4,/* A5 */ {0,2,5,7} },
    {4,/* A6 */ {1,2,5,7} },
    {5,/* A7 */ {0,1,2,5,7} },
    {3,/* A8 */ {3,5,7} },
    {4,/* A9 */ {0,3,5,7} },
    {4,/* AA */ {1,3,5,7} },
    {5,/* AB */ {0,1,3,5,7} },
    {4,/* AC */ {2,3,5,7} },
    {5,/* AD */ {0,2,3,5,7} },
    {5,/* AE */ {1,2,3,5,7} },
    {6,/* AF */ {0,1,2,3,5,7} },
    {3,/* B0 */ {4,5,7} },
    {4,/* B1 */ {0,4,5,7} },
    {4,/* B2 */ {1,4,5,7} },
    {5,/* B3 */ {0,1,4,5,7} },
    {4,/* B4 */ {2,4,5,7} },
    {5,/* B5 */ {0,2,4,5,7} },
    {5,/* B6 */ {1,2,4,5,7} },
    {6,/* B7 */ {0,1,2,4,5,7} },
    {4,/* B8 */ {3,4,5,7} },
    {5,/* B9 */ {0,3,4,5,7} },
    {5,/* BA */ {1,3,4,5,7} },
    {6,/* BB */ {0,1,3,4,5,7} },
    {5,/* BC */ {2,3,4,5,7} },
    {6,/* BD */ {0,2,3,4,5,7} },
    {6,/* BE */ {1,2,3,4,5,7} },
    {7,/* BF */ {0,1,2,3,4,5,7} },
    {2,/* C0 */ {6,7} },
    {3,/* C1 */ {0,6,7} },
    {3,/* C2 */ {1,6,7} },
    {4,/* C3 */ {0,1,6,7} },
    {3,/* C4 */ {2,6,7} },
    {4,/* C5 */ {0,2,6,7} },
    {4,/* C6 */ {1,2,6,7} },
    {5,/* C7 */ {0,1,2,6,7} },
    {3,/* C8 */ {3,6,7} },
    {4,/* C9 */ {0,3,6,7} },
    {4,/* CA */ {1,3,6,7} },
    {5,/* CB */ {0,1,3,6,7} },
    {4,/* CC */ {2,3,6,7} },
    {5,/* CD */ {0,2,3,6,7} },
    {5,/* CE */ {1,2,3,6,7} },
    {6,/* CF */ {0,1,2,3,6,7} },
    {3,/* D0 */ {4,6,7} },
    {4,/* D1 */ {0,4,6,7} },
    {4,/* D2 */ {1,4,6,7} },
    {5,/* D3 */ {0,1,4,6,7} },
    {4,/* D4 */ {2,4,6,7} },
    {5,/* D5 */ {0,2,4,6,7} },
    {5,/* D6 */ {1,2,4,6,7} },
    {6,/* D7 */ {0,1,2,4,6,7} },
    {4,/* D8 */ {3,4,6,7} },
    {5,/* D9 */ {0,3,4,6,7} },
    {5,/* DA */ {1,3,4,6,7} },
    {6,/* DB */ {0,1,3,4,6,7} },
    {5,/* DC */ {2,3,4,6,7} },
    {6,/* DD */ {0,2,3,4,6,7} },
    {6,/* DE */ {1,2,3,4,6,7} },
    {7,/* DF */ {0,1,2,3,4,6,7} },
    {3,/* E0 */ {5,6,7} },
    {4,/* E1 */ {0,5,6,7} },
    {4,/* E2 */ {1,5,6,7} },
    {5,/* E3 */ {0,1,5,6,7} },
    {4,/* E4 */ {2,5,6,7} },
    {5,/* E5 */ {0,2,5,6,7} },
    {5,/* E6 */ {1,2,5,6,7} },
    {6,/* E7 */ {0,1,2,5,6,7} },
    {4,/* E8 */ {3,5,6,7} },
    {5,/* E9 */ {0,3,5,6,7} },
    {5,/* EA */ {1,3,5,6,7} },
    {6,/* EB */ {0,1,3,5,6,7} },
    {5,/* EC */ {2,3,5,6,7} },
    {6,/* ED */ {0,2,3,5,6,7} },
    {6,/* EE */ {1,2,3,5,6,7} },
    {7,/* EF */ {0,1,2,3,5,6,7} },
    {4,/* F0 */ {4,5,6,7} },
    {5,/* F1 */ {0,4,5,6,7} },
    {5,/* F2 */ {1,4,5,6,7} },
    {6,/* F3 */ {0,1,4,5,6,7} },
    {5,/* F4 */ {2,4,5,6,7} },
    {6,/* F5 */ {0,2,4,5,6,7} },
    {6,/* F6 */ {1,2,4,5,6,7} },
    {7,/* F7 */ {0,1,2,4,5,6,7} },
    {5,/* F8 */ {3,4,5,6,7} },
    {6,/* F9 */ {0,3,4,5,6,7} },
    {6,/* FA */ {1,3,4,5,6,7} },
    {7,/* FB */ {0,1,3,4,5,6,7} },
    {6,/* FC */ {2,3,4,5,6,7} },
    {7,/* FD */ {0,2,3,4,5,6,7} },
    {7,/* FE */ {1,2,3,4,5,6,7} },
    {8,/* FF */ {0,1,2,3,4,5,6,7} }
};

/* EXPORTED SUBPROGRAM BODIES */
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
UI32_T L_BITMAP_Get_BitPos_List (UI32_T bitmap, UI8_T bit_pos_list[32])
{
    const BitPosList_T *pos_entry_p;
    UI32_T  return_bit_count;
    UI32_T  i;

    /* BODY
     */
    if ( bit_pos_list == 0 )
        return 0;

    /* For best performance, we don't use for loop here
     */
    /* Get 1st least sig significant byte, bit 0 ~ bit 7
     */
    pos_entry_p = bit_pos_tab +  (((UI8_T*)&bitmap) [BYTE0] );
    memcpy (bit_pos_list, pos_entry_p->blist, pos_entry_p->count);
    bit_pos_list += (return_bit_count = pos_entry_p->count);


    /* Get bit 8 ~ bit 15
     */
    pos_entry_p = bit_pos_tab +  (((UI8_T*)&bitmap) [BYTE1] );
    for ( i=0; i<pos_entry_p->count; i++)
    {
        *bit_pos_list++ = pos_entry_p->blist[i] + 8;
    }
    return_bit_count +=  pos_entry_p->count;

    /* Get bit 16 ~ bit 23
     */
    pos_entry_p = bit_pos_tab +  (((UI8_T*)&bitmap) [BYTE2] );
    for ( i=0; i<pos_entry_p->count; i++)
    {
        *bit_pos_list++ = pos_entry_p->blist[i] + 16;
    }
    return_bit_count +=  pos_entry_p->count;

    /* Get bit 24 ~ bit 31
     */
    pos_entry_p = bit_pos_tab +  (((UI8_T*)&bitmap) [BYTE3] );
    for ( i=0; i<pos_entry_p->count; i++)
    {
        *bit_pos_list++ = pos_entry_p->blist[i] + 24;
    }
    return_bit_count +=  pos_entry_p->count;

    return return_bit_count;
}


/*check the port bitmap whether is NULL or not*/
BOOL_T
L_BITMAP_is_null(UI8_T *pbmp)
{
  BOOL_T ret = FALSE;
  UI32_T i;
  
  for (i = 0; i < SYS_ADPT_TOTAL_NBR_OF_BYTE_FOR_1BIT_PORT_LIST; i++)
  {
      if (pbmp[i] != 0)
      {
          ret=TRUE;		
          break;
      }
  }
  
  return ret;
}

/*check whether one port is set or not*/
BOOL_T
L_BITMAP_port_is_set(UI8_T *pbmp, UI32_T port_num)
{
  BOOL_T ret = FALSE;
  UI32_T tmp;
  
  if (port_num > SYS_ADPT_TOTAL_NBR_OF_LPORT || port_num < 1)
      return ret;
  
  tmp = pbmp[(port_num -1)/8];
  
  if ((tmp &= 0x80 >> ((port_num -1) % 8)) != 0)
      ret = TRUE;

  return ret;
}


void
L_BITMAP_port_set(UI8_T *pbmp, UI32_T port_num)
{
  assert(pbmp);
  
  if ((port_num > SYS_ADPT_TOTAL_NBR_OF_LPORT) || (port_num < 1))
      return;
  
  pbmp[(port_num -1)/8] |= ( 0x80 >> (port_num -1) % 8 );
  
  return;
}


void
L_BITMAP_port_unset(UI8_T *pbmp, UI32_T port_num)
{
  assert(pbmp);

  if (port_num > SYS_ADPT_TOTAL_NBR_OF_LPORT || port_num < 1)
      return;
  
  pbmp[(port_num -1) / 8] &= ~( 0x80 >> (port_num -1) % 8 );

  return;
}

/*clear dst corresponding bit when bit in src has been set*/
void
L_BITMAP_bmp_xor(UI8_T *pbmp_dst, UI8_T *pbmp_src)
{
  UI32_T i;

  assert(pbmp_dst);
  assert(pbmp_src);
  
  for (i = 0; i < SYS_ADPT_TOTAL_NBR_OF_LPORT; i++)
  {
      if (!L_BITMAP_port_is_set(pbmp_src, i))
          continue;

      L_BITMAP_port_unset(pbmp_dst, i);
  }
  
  return;
}



/* LOCAL SUBPROGRAM BODIES */

/* Functions for unit test */
#ifdef _TEST
#include <stdio.h>

void L_BITMAP_dump(UI32_T bitmap, int cnt, UI8_T bit_pos_list[])
{
	int idx;

	printf("bitmap=0x%lx count=%d pos_list={", bitmap, cnt);
	for(idx=0; idx<cnt; idx++)
		printf("%hd ",bit_pos_list[idx]);

    printf("}\n");
}

#define L_BITMAP_TEST_AND_DUMP(bitmap) \
    L_BITMAP_dump(bitmap, L_BITMAP_Get_BitPos_List (bitmap, bit_pos_list), bit_pos_list)
void L_BITMAP_main (void)
{
    UI8_T  bit_pos_list[32];
    UI32_T bitmap=0;

    L_BITMAP_TEST_AND_DUMP(0);
    L_BITMAP_TEST_AND_DUMP(1);
    L_BITMAP_TEST_AND_DUMP(4);
    L_BITMAP_TEST_AND_DUMP(65535);
    L_BITMAP_TEST_AND_DUMP(0x001FF);
    L_BITMAP_TEST_AND_DUMP(0x3311FF);
    L_BITMAP_TEST_AND_DUMP(0x553311FF);

    L_BITMAP_SET_INDEX(&bitmap, 0);
    L_BITMAP_SET_INDEX(&bitmap, 3);
    L_BITMAP_SET_INDEX(&bitmap, 31);
    L_BITMAP_TEST_AND_DUMP(bitmap);

    L_BITMAP_RESET_INDEX(&bitmap, 3);
    L_BITMAP_TEST_AND_DUMP(bitmap);
    /* expected output:
       bitmap=0x0 count=0 pos_list={}
       bitmap=0x1 count=1 pos_list={0 }
       bitmap=0x4 count=1 pos_list={2 }
       bitmap=0xffff count=16 pos_list={0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 }
       bitmap=0x1ff count=9 pos_list={0 1 2 3 4 5 6 7 8 }
       bitmap=0x3311ff count=14 pos_list={0 1 2 3 4 5 6 7 8 12 16 17 20 21 }
       bitmap=0x553311ff count=18 pos_list={0 1 2 3 4 5 6 7 8 12 16 17 20 21 24 26 28 30 }
       bitmap=0x80000009 count=3 pos_list={0 3 31 }
       bitmap=0x80000001 count=2 pos_list={0 31 }
    */

    return;
}
#endif
