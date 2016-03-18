/*******************************************************************************
*                Copyright 2009, Marvell International Ltd.
* This code contains confidential information of Marvell semiconductor, inc.
* no rights are granted herein under any patent, mask work right or copyright
* of Marvell or any third party.
* Marvell reserves the right at its sole discretion to request that this code
* be immediately returned to Marvell. This code is provided "as is".
* Marvell makes no warranties, express, implied or otherwise, regarding its
* accuracy, completeness or performance.
********************************************************************************
* gtDbDxLion48Config.h
*
* DESCRIPTION:
*       This file includes  serdes configuration and port mapping for Lion 48 board
*
* DEPENDENCIES:
*
*
* FILE REVISION NUMBER:
*       $Revision: 2 $
*
*******************************************************************************/

typedef struct
{
    GT_U32 firstRegAddr;
    GT_U32 lastRegAddr;
    GT_U32 data;
    GT_U32 waitTime;
}GT_SERDES_INIT;


typedef struct
{
    GT_U32 portGroup;
    GT_U32 portNum;     /* port number as defined in CPSS */
    GT_32  fpPortNum;   /* front panel port */
    GT_U32 phyAddr;     /* phy XSMI address */
    GT_U32 smiAddr;     /* smi address for  LionCub */
    GT_U32 lcXpcsPort;  /* XPCS port of Lioncub - Up0 or Up1 */
}GT_LION48_PORT_DATA;

GT_LION48_PORT_DATA lion48PortDataArray[] =
{
/* port group 0 */
    {0,  0,  0,  0, 5, 0},
    {0,  1,  1,  1, 5, 1},
    {0,  2,  2,  2, 4, 1},
    {0,  3,  3,  3, 4, 0},
    {0,  4,  4,  4, 7, 0},
    {0,  5,  5,  5, 7, 1},
    {0,  6,  6,  6, 6, 1},
    {0,  7,  7,  7, 6, 0},
    {0,  8,  8,  8, 9, 0},
    {0,  9,  9,  9, 9, 1},
    {0, 10, 10, 10, 8, 1},
    {0, 11, 11, 11, 8, 0},
    {0, 12, -1, 12, 0, 0},
    {0, 13, -1, 13, 0, 1},
    {0, 14, -1, 14, 0, 1},
    {0, 15, -1, 15, 0, 0},
/* port group 1 */
    {1,  0, 12,  0, 5, 0},
    {1,  1, 13,  1, 5, 1},
    {1,  2, 14,  2, 4, 1},
    {1,  3, 15,  3, 4, 0},
    {1,  4, 16,  4, 7, 0},
    {1,  5, 17,  5, 7, 1},
    {1,  6, 18,  6, 6, 1},
    {1,  7, 19,  7, 6, 0},
    {1,  8, 20,  8, 9, 0},
    {1,  9, 21,  9, 9, 1},
    {1, 10, 22, 10, 8, 1},
    {1, 11, 23, 11, 8, 0},
    {1, 12, -1, 12, 0, 0},
    {1, 13, -1, 13, 0, 1},
    {1, 14, -1, 14, 0, 1},
    {1, 15, -1, 15, 0, 0},
/* port group 2 */
    {2,  0, 24,  0, 5, 0},
    {2,  1, 25,  1, 5, 1},
    {2,  2, 26,  2, 4, 1},
    {2,  3, 27,  3, 4, 0},
    {2,  4, 28,  4, 7, 0},
    {2,  5, 29,  5, 7, 1},
    {2,  6, 30,  6, 6, 1},
    {2,  7, 31,  7, 6, 0},
    {2,  8, 32,  8, 9, 0},
    {2,  9, 33,  9, 9, 1},
    {2, 10, 34, 10, 8, 1},
    {2, 11, 35, 11, 8, 0},
    {2, 12, -1, 12, 0, 0},
    {2, 13, -1, 13, 0, 1},
    {2, 14, -1, 14, 0, 1},
    {2, 15, -1, 15, 0, 0},
/* port group 3 */
    {3,  0, 36,  0, 5, 0},
    {3,  1, 37,  1, 5, 1},
    {3,  2, 38,  2, 4, 1},
    {3,  3, 39,  3, 4, 0},
    {3,  4, 40,  4, 7, 0},
    {3,  5, 41,  5, 7, 1},
    {3,  6, 42,  6, 6, 1},
    {3,  7, 43,  7, 6, 0},
    {3,  8, 44,  8, 9, 0},
    {3,  9, 45,  9, 9, 1},
    {3, 10, 46, 10, 8, 1},
    {3, 11, 47, 11, 8, 0},
    {3, 12, -1, 12, 0, 0},
    {3, 13, -1, 13, 0, 1},
    {3, 14, -1, 14, 0, 1},
    {3, 15, -1, 15, 0, 0},
};

GT_SERDES_INIT lionSerdesInitArray[] =
{
    {0x8800000, 0x8802c00, 0x2700, 0xA}, /* Disable port and assert port reset */
    {0x9800000, 0x9805c00, 0x4e00, 0xa}, /* Configuring reference clock to 156.25MHz */
    {0x9800004, 0x9805c04, 0x0008, 0xa}, /* Deactivate SerDes Reset */
    /*--- Configuring SerDes to 6.25Gbps --- */
    {0x980020c, 0x9805e0c, 0x800a, 0x0}, /* Configuring LP SerDes Offset 0x03 */
    {0x9800210, 0x9805e10, 0x4415, 0x0}, /* Configuring LP SerDes Offset 0x04 */
    {0x9800214, 0x9805e14, 0x8150, 0x1}, /* Configuring LP SerDes Offset 0x05 */
    {0x9800214, 0x9805e14, 0xa150, 0x0}, /* Configuring LP SerDes Offset 0x05 */
    {0x9800218, 0x9805e18, 0xfbaa, 0x0}, /* Configuring LP SerDes Offset 0x06 */
    {0x980021c, 0x9805e1c, 0x8720, 0x0}, /* Configuring LP SerDes Offset 0x07 */
    {0x98003cc, 0x9805fcc, 0x2000, 0x0}, /* Configuring LP SerDes Offset 0x73 */
    {0x980022c, 0x9805e2c, 0x0000, 0x0}, /* Configuring LP SerDes Offset 0x0b */
    {0x9800230, 0x9805e30, 0x0000, 0x0}, /* Configuring LP SerDes Offset 0x0c */
    {0x9800234, 0x9805e34, 0x4000, 0x0}, /* Configuring LP SerDes Offset 0x0d */
    {0x980023c, 0x9805e3c, 0x0008, 0x0}, /* Configuring LP SerDes Offset 0x0f */
    {0x9800250, 0x9805e50, 0xa060, 0x0}, /* Configuring LP SerDes Offset 0x14 */
    {0x9800254, 0x9805e54, 0x7f2d, 0x0}, /* Configuring LP SerDes Offset 0x15 */
    {0x9800258, 0x9805e58, 0x0900, 0x0}, /* Configuring LP SerDes Offset 0x16 */
    {0x980027c, 0x9805e7c, 0x90aa, 0x0}, /* Configuring LP SerDes Offset 0x1F */
    {0x9800280, 0x9805e80, 0x4800, 0x0}, /* Configuring LP SerDes Offset 0x20 */
    {0x980028c, 0x9805e8c, 0x036f, 0x0}, /* Configuring LP SerDes Offset 0x23 */
    {0x980035c, 0x9805f5c, 0x423f, 0x0}, /* Configuring LP SerDes Offset 0x57 */
    {0x980036c, 0x9805f6c, 0x0000, 0x0}, /* Configuring LP SerDes Offset 0x5b*/
    {0x9800234, 0x9805e34, 0x0400, 0x0}, /* Configuring LP SerDes Offset 0x0D */
    {0x9800228, 0x9805e28, 0xe0c0, 0x0}, /* Configuring LP SerDes Offset 0x0a */
    {0x980036c, 0x9805f6c, 0x8040, 0x1}, /* Configuring LP SerDes Offset 0x5b*/
    {0x980036c, 0x9805f6c, 0x8000, 0x5}, /* Configuring LP SerDes Offset 0x5b*/
    {0x9800224, 0x9805e24, 0x0000, 0x1}, /* Configuring LP SerDes Offset 0x09 */
    {0x9800224, 0x9805e24, 0x8000, 0x1}, /* Configuring LP SerDes Offset 0x09 */
    {0x9800224, 0x9805e24, 0x0000, 0xA}, /* Configuring LP SerDes Offset 0x09 */
    /*--- End of SerDes Configuration --- */
    {0x9800004, 0x9805e04, 0x0018, 0xA}, /* Set RxInit to 0x1 */
    {0x9800004, 0x9805e04, 0x0008, 0xA}, /* Set RxInit to 0x0 */
    {0x9800004, 0x9805e04, 0x0048, 0xA}, /* Set RxInit to 0x1 */
    {0x8800200, 0x8802e00, 0x0046, 0x0}, /* XPCS Configurations */
    {0x8800200, 0x8802e00, 0x0047, 0xA}, /* De assert PCS Reset */
    {0x8800000, 0x8802c00, 0x2703, 0xA}, /* Enable port and De-assert port reset */
};
