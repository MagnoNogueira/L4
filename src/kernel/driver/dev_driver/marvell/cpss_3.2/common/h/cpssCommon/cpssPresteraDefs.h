/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssPresteraDefs.h
*
* DESCRIPTION:
*       Includes general system definitions.
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*       $Revision: 10 $
*
*******************************************************************************/
#ifndef __cpssPresteraDefsh
#define __cpssPresteraDefsh

#include <cpss/extServices/os/gtOs/gtGenTypes.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define MARVELL_VENDOR_ID 0x11ab

#define GT_SDMA_WA

/* Max number of PPs in system  */
#define PRV_CPSS_MAX_PP_DEVICES_CNS             128

/* Max number pf ports in PP device */
#define PRV_CPSS_MAX_PP_PORTS_NUM_CNS           64

/* Max number of Xbars in system  */
#define PRV_CPSS_MAX_XBAR_DEVICES_CNS           128

/* Max fabric ports in xbar device */
#define PRV_CPSS_MAX_XBAR_FPORTS_NUM_CNS        13

/* Define the different memory sizes    */
#ifndef _1KB
#define _1KB            (0x400)
#define _2KB            (0x800)
#define _4KB            (0x1000)
#define _6KB            (0x1800)
#define _8KB            (0x2000)
#define _10KB           (0x2800)
#define _16KB           (0x4000)
#define _24KB           (0x6000)
#define _32KB           (0x8000)
#define _64KB           (0x10000)
#define _128KB          (0x20000)
#define _256KB          (0x40000)
#define _512KB          (0x80000)
#define _1MB            (0x100000)
#define _2MB            (0x200000)
#define _3MB            (0x300000)
#define _4MB            (0x400000)
#define _6MB            (0x600000)
#define _8MB            (0x800000)
#define _16MB           (0x1000000)
#define _32MB           (0x2000000)
#define _64MB           (0x4000000)
#define _128MB          (0x8000000)
#define _256MB          (0x10000000)
#endif

/* Define the different sizes    */
#ifndef _1K
#define _1K            (0x400)
#define _2K            (0x800)
#define _3K            (0xC00)
#define _4K            (0x1000)
#define _5K            (0x1400)
#define _6K            (0x1800)
#define _8K            (0x2000)
#define _10K           (0x2800)
#define _12K           (0x3000)
#define _16K           (0x4000)
#define _24K           (0x6000)
#define _32K           (0x8000)
#define _64K           (0x10000)
#define _128K          (0x20000)
#define _256K          (0x40000)
#define _512K          (0x80000)
#define _1M            (0x100000)
#define _2M            (0x200000)
#define _3M            (0x300000)
#define _4M            (0x400000)
#define _6M            (0x600000)
#define _8M            (0x800000)
#define _12M           (0xC00000)
#define _16M           (0x1000000)
#define _24M           (0x1800000)
#define _32M           (0x2000000)
#define _64M           (0x4000000)
#define _128M          (0x8000000)
#define _256M          (0x10000000)
#endif

/* Define single bit masks.             */
#ifndef BIT_0
#define BIT_0           (0x1)
#define BIT_1           (0x2)
#define BIT_2           (0x4)
#define BIT_3           (0x8)
#define BIT_4           (0x10)
#define BIT_5           (0x20)
#define BIT_6           (0x40)
#define BIT_7           (0x80)
#define BIT_8           (0x100)
#define BIT_9           (0x200)
#define BIT_10          (0x400)
#define BIT_11          (0x800)
#define BIT_12          (0x1000)
#define BIT_13          (0x2000)
#define BIT_14          (0x4000)
#define BIT_15          (0x8000)
#define BIT_16          (0x10000)
#define BIT_17          (0x20000)
#define BIT_18          (0x40000)
#define BIT_19          (0x80000)
#define BIT_20          (0x100000)
#define BIT_21          (0x200000)
#define BIT_22          (0x400000)
#define BIT_23          (0x800000)
#define BIT_24          (0x1000000)
#define BIT_25          (0x2000000)
#define BIT_26          (0x4000000)
#define BIT_27          (0x8000000)
#define BIT_28          (0x10000000)
#define BIT_29          (0x20000000)
#define BIT_30          (0x40000000)
#define BIT_31          (0x80000000)
#endif

/* used by functionsSupportedBmp */
#define PRV_CPSS_EXMX_FUNCTIONS_SUPPORT_CNS         BIT_0
#define PRV_CPSS_EXMXTG_FUNCTIONS_SUPPORT_CNS       BIT_1
#define PRV_CPSS_DXCH_FUNCTIONS_SUPPORT_CNS         BIT_2
#define PRV_CPSS_DXCH2_FUNCTIONS_SUPPORT_CNS        BIT_3
#define PRV_CPSS_DXSAL_FUNCTIONS_SUPPORT_CNS        BIT_4
#define PRV_CPSS_EXMXPM_FUNCTIONS_SUPPORT_CNS       BIT_5
#define PRV_CPSS_DXCH3_FUNCTIONS_SUPPORT_CNS        BIT_6
#define PRV_CPSS_DXCH_XCAT_FUNCTIONS_SUPPORT_CNS    BIT_7
#define PRV_CPSS_DXCH_LION_FUNCTIONS_SUPPORT_CNS    BIT_8

/* check NULL pointer */
#define CPSS_NULL_PTR_CHECK_MAC(ptr)    if(ptr == NULL) return GT_BAD_PTR

/* Return the mask including "numOfBits" bits. for 0..31 bits   */
#define BIT_MASK_0_31_MAC(numOfBits) (~(0xFFFFFFFF << (numOfBits)))

/* the macro of BIT_MASK_MAC() in VC will act in wrong way for 32 bits , and will
   result 0 instead of 0xffffffff

   so macro BIT_MASK_IMPR - is improvement of BIT_MASK_MAC to support 32 bits
*/
#define BIT_MASK_MAC(numOfBits)    ((numOfBits) == 32 ? 0xFFFFFFFF : BIT_MASK_0_31_MAC(numOfBits))

/* Calculate the field mask for a given offset & length */
/* e.g.: BIT_MASK_MAC(8,2) = 0xFFFFFCFF                     */
#define FIELD_MASK_NOT_MAC(offset,len)                      \
        (~(BIT_MASK_MAC((len)) << (offset)))

/* Calculate the field mask for a given offset & length */
/* e.g.: BIT_MASK_MAC(8,2) = 0x00000300                     */
#define FIELD_MASK_MAC(offset,len)                      \
        ( (BIT_MASK_MAC((len)) << (offset)) )

/* Returns the info located at the specified offset & length in data.   */
#define U32_GET_FIELD_MAC(data,offset,length)           \
        (((data) >> (offset)) & ((1u << (length)) - 1u))

/* Sets the field located at the specified offset & length in data.     */
#define U32_SET_FIELD_MAC(data,offset,length,val)           \
   (data) = (((data) & FIELD_MASK_NOT_MAC((offset),(length))) | ((val) <<(offset)))

/* Sets the field located at the specified offset & length in data.
    the field may be with value with more bits then length */
#define U32_SET_FIELD_MASKED_MAC(data,offset,length,val)           \
   (U32_SET_FIELD_MAC((data),(offset),(length),((val) & BIT_MASK_MAC(length))))


/* Sets the field located at the specified offset & length in data.
   NOTE :
   1. with out clearing the field ---> Assume that that the field was
      already cleared !!!
   2. the field MUST NOT be with value with more bits then length
   3. length is NOT USED !!!
*/
#define U32_SET_FIELD_NO_CLEAR_MAC(data,offset,length,val)           \
   (data) |= ((val) <<(offset))

/* Sets the field located at the specified offset & length in data.
   NOTE :
   1. with out clearing the field ---> Assume that that the field was
      already cleared !!!
   2. the field may be with value with more bits then length
*/
#define U32_SET_FIELD_NO_CLEAR_MASKED_MAC(data,offset,length,val)    \
    U32_SET_FIELD_NO_CLEAR_MAC(data,offset,length,((val) & BIT_MASK_MAC(length) ))


#define CALC_MASK_MAC(fieldLen, fieldOffset, mask)     \
            if(((fieldLen) + (fieldOffset)) >= 32)     \
             (mask) = (GT_U32)(0 - (1<< (fieldOffset)));    \
            else                                   \
          (mask) = (((1<<((fieldLen) + (fieldOffset)))) - (1 << (fieldOffset)))

#define BYTESWAP_MAC(data)                      \
        (((data) << 24))  |                 \
        (((data) & 0xff00)      << 8)   |   \
        (((data) & 0xff0000)    >> 8)   |   \
        (((data) >> 24))


/* A macro for performing left shifting on a 64 bit data            */
/* highData -   A 32 bit word including the MSB part of the data    */
/* lowData  -   A 32 bit word including the LSB part of the data    */
/* shift    -   Number of bits to shift.                            */
/* overFlow -   The (left) part of highData which overflowed.       */
#define U64_SHIFT_LEFT_MAC(overFlow,highData,lowData,shift)         \
        (overFlow) = ((overFlow) << (shift)) | ((highData) >> (32 - (shift))); \
        (highData) = ((highData) << (shift));                      \
        (highData)|= ((lowData) >> (32 - (shift)));              \
        (lowData)  = ((lowData) << (shift))

#define U64_SHIFT_RIGHT_MAC(highData,lowData,underFlow,shift)         \
        (underFlow) = ((underFlow) >> (shift)) | ((lowData) << (32 - (shift)));\
        (lowData)   = ((lowData) >> (shift));                      \
        (lowData)  |= ((highData) << (32 - (shift)));              \
        (highData)  = ((highData) >> (shift))

#define BOOL2BIT_MAC(x) (((x) == GT_FALSE) ? 0 : 1)

#define BIT2BOOL_MAC(x) (((x) == 1) ? GT_TRUE : GT_FALSE)
/* Return the minimum of x & y. */
#ifndef MIN
#define MIN(x,y) (((x) < (y)) ? (x) : (y))
#endif  /* MIN */
#ifndef MAX
#define MAX(x,y) (((x) > (y)) ? (x) : (y))
#endif  /* MAX */


#define U8_SWAP_BITS(x)         \
        ((((x)&0x80)>>7)|(((x)&0x40)>>5)|(((x)&0x20)>>3)|(((x)&0x10)>>1) \
         |(((x)&0x1)<<7) | (((x)&0x2)<<5) | (((x)&0x4)<<3) |(((x)&0x8)<<1) )


/* the MACRO is integer that has bit number 'x' is 1
   and all other bits are 0 */
#define BYTES_ARRAY_BIT_MASK(x)   \
        (1 << (x))

/* Get the byte of the x bit from an array of bytes */
#define BYTES_ARRAY_GET_BYTE(a,x) \
        ( ( (GT_U8 *) a)[(x)/8] )

/* checks wheter bit 'x' in 'a' is set and than returns TRUE,  */
/* otherwise return FALSE.                                     */
#define BYTES_ARRAY_CHKBIT(a,x) \
    ( ( BYTES_ARRAY_GET_BYTE(a,x) & BYTES_ARRAY_BIT_MASK((x)%8) ) >> ((x)%8) )

/* Clear (reset) bit 'x' in an array of bytes */
#define BYTES_ARRAY_CLRBIT(a,x) \
    (BYTES_ARRAY_GET_BYTE(a,x) &=  ~(BYTES_ARRAY_BIT_MASK((x)%8) ) )

/* Clear (reset) bit 'x' in an array of bytes */
#define BYTES_ARRAY_SETBIT(a,x) \
    (BYTES_ARRAY_GET_BYTE(a,x) |=  BYTES_ARRAY_BIT_MASK((x)%8) )

/*   INVERT bit 'x' in in an array of bytes. */
#define BYTES_ARRAY_INVBIT(a,x) \
    ( BYTES_ARRAY_GET_BYTE(a,x) ^= BYTES_ARRAY_BIT_MASK((x)%8) )

/* Get/set a nibble from/to up to U32 field
 * Nibble 0 represent bits 0:3 etc
 */
#define GT_NIBBLE_SIZE_CNS (4)
#define GT_GET_NIBBLE_MAC(field, nibbleIdx) \
            U32_GET_FIELD_MAC(field, (nibbleIdx * GT_NIBBLE_SIZE_CNS), GT_NIBBLE_SIZE_CNS);

#define GT_SET_NIBBLE_MAC(field, nibbleIdx, val) \
            U32_SET_FIELD_MAC(field, (nibbleIdx * GT_NIBBLE_SIZE_CNS), GT_NIBBLE_SIZE_CNS, val);

/* check that a number is not out of range of max_bit_num */
#define CHECK_BITS_DATA_RANGE_MAC(number,max_bit_num) \
        ( number < ( 1 << max_bit_num )) ? GT_TRUE:GT_FALSE

#ifndef __gtCommonDefsh
/*
 * typedef: enum GT_INT_TYPE
 *
 * Description: GT device interrupt connection type
 *
 * Enumerations:
 *    GT_INT_2_PP_GPP0 - interrupt line is connected to packet
 *                            processor GPP pin 0.
 *    GT_INT_2_PP_GPP1 - interrupt line is connected to packet
 *                            processor GPP pin 1.
 *    GT_INT_2_PP_GPP2 - interrupt line is connected to packet
 *                            processor GPP pin 2.
 *    GT_INT_2_UPLINK_GPP - interrupt line is connected to packet
 *                            processor uplink GPP.
 *    GT_INT_2_PCI_A   - interrupt is directly connected to PCI A
 *                            interrupt
 *    GT_INT_2_PCI_B   - interrupt is directly connected to PCI B
 *                            interrupt
 *    GT_INT_2_PCI_C   - interrupt is directly connected to PCI C
 *                            interrupt
 *    GT_INT_2_PCI_D   - interrupt is directly connected to PCI D
 *                            interrupt
 *    GT_INT_OTHER     - interrupt is connected in some other manner.
 *
 */
typedef enum
{
    GT_INT_2_PP_GPP0 = 0,
    GT_INT_2_PP_GPP1,
    GT_INT_2_PP_GPP2,
    GT_INT_2_UPLINK_GPP,
    GT_INT_2_PCI_A,
    GT_INT_2_PCI_B,
    GT_INT_2_PCI_C,
    GT_INT_2_PCI_D,
    GT_INT_OTHER
}GT_INT_TYPE;


/*
 * typedef: struct GT_INT
 *
 * Description:
 *     GT device Interrupt definition
 *
 * Fields:
 *  intType   - interrupt connection type.
 *  intVecNum - The interrupt vector number this device is connected to in
 *              case of PCI or other interrupt, or the packet processor device
 *              number connected to by its GPP pin.
 *    intMask - The interrupt mask to enable GT interrupts (used in Mips)
 */
typedef struct
{
    GT_INT_TYPE          intType;
    GT_U32               intVecNum;
    GT_U32               intMask;
}GT_INT;
#endif  /* __gtCommonDefsh */


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __cpssPresteraDefsh */

