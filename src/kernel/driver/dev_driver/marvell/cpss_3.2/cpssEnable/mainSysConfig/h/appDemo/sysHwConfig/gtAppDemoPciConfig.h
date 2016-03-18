/*******************************************************************************
*              Copyright 2001, GALILEO TECHNOLOGY, LTD.
*
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL. NO RIGHTS ARE GRANTED
* HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT OF MARVELL OR ANY THIRD
* PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE DISCRETION TO REQUEST THAT THIS
* CODE BE IMMEDIATELY RETURNED TO MARVELL. THIS CODE IS PROVIDED "AS IS".
* MARVELL MAKES NO WARRANTIES, EXPRESS, IMPLIED OR OTHERWISE, REGARDING ITS
* ACCURACY, COMPLETENESS OR PERFORMANCE. MARVELL COMPRISES MARVELL TECHNOLOGY
* GROUP LTD. (MTGL) AND ITS SUBSIDIARIES, MARVELL INTERNATIONAL LTD. (MIL),
* MARVELL TECHNOLOGY, INC. (MTI), MARVELL SEMICONDUCTOR, INC. (MSI), MARVELL
* ASIA PTE LTD. (MAPL), MARVELL JAPAN K.K. (MJKK), GALILEO TECHNOLOGY LTD. (GTL)
* AND GALILEO TECHNOLOGY, INC. (GTI).
********************************************************************************
* gtAppDemoPciConfig.h
*
* DESCRIPTION:
*       Prestera Devices pci initialization & detection module.
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.2.1 $
*
*******************************************************************************/
#ifndef __gtAppDemoPciConfigh
#define __gtAppDemoPciConfigh

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <gtExtDrv/drivers/gtPciDrv.h>
#include <appDemo/os/appOs.h>


#define GT_PCI_VENDOR_ID    0x11AB


/*
 * typedef: struct GT_PCI_DEV_VENDOR_ID
 *
 * Description: PCI device and vendor ID struct
 *
 * Fields:
 *   vendorId - The Prestera PCI vendor Id.
 *   devId    - The different Prestera PCI device Id.
 */
typedef struct
{
    GT_U16  vendorId;
    GT_U16  devId;
} GT_PCI_DEV_VENDOR_ID;


/*
 * typedef: struct GT_PCI_INFO
 *
 * Description: PCI device information
 *
 * Fields:
 *      pciDevVendorId - The Prestera PCI vendor Id.
 *      pciBaseAddr    - The different Prestera PCI device Id.
 *      pciIdSel       - The device PCI ID_SEL.
 *      pciBusNum      - The PCI bus number.
 *      internalPciBase- Base address to which the internal pci registers
 *                       are mapped to.
 *
 */
typedef struct
{
    GT_PCI_DEV_VENDOR_ID    pciDevVendorId;
    GT_U32                  pciBaseAddr;
    GT_U32                  internalPciBase;
    GT_U32                  pciIdSel;
    GT_U32                  pciBusNum;
    GT_U32                  funcNo;
    GT_U32                  pciHeaderInfo[16];
} GT_PCI_INFO;



/*******************************************************************************
* gtPresteraGetPciDev
*
* DESCRIPTION:
*       This routine search for Prestera Devices Over the PCI.
*
* INPUTS:
*       first - whether to bring the first device, if GT_FALSE return the next
*               device.
*
* OUTPUTS:
*       pciInfo - the next device PCI info.
*
* RETURNS:
*       GT_OK      - on success.
*       GT_FAIL    - otherwise.
*       GT_NO_MORE - no more prestera devices.
*
* COMMENTS:
*       None.
*
* Galtis:
*       None.
*
* Toolkit:
*
*******************************************************************************/
GT_STATUS gtPresteraGetPciDev
(
    IN  GT_BOOL     first,
    OUT GT_PCI_INFO *pciInfo
);



/*******************************************************************************
* gtPresteraGetPciIntVec
*
* DESCRIPTION:
*       This routine return the PCI interrupt vector.
*
* INPUTS:
*       pciInt - PCI interrupt number.
*
* OUTPUTS:
*       intVec - PCI interrupt vector.
*
* RETURNS:
*       GT_OK      - on success.
*       GT_FAIL    - otherwise.
*
* COMMENTS:
*       None.
*
* Galtis:
*       None.
*
* Toolkit:
*******************************************************************************/
GT_STATUS gtPresteraGetPciIntVec
(
    IN  GT_PCI_INT  pciInt,
    OUT void        **intVec
);

/*******************************************************************************
* gtPresteraGetPciIntMask
*
* DESCRIPTION:
*       This routine return the PCI interrupt mask to enable/disable interrupts.
*
* INPUTS:
*       pciInt - PCI interrupt number.
*
* OUTPUTS:
*       intMask - PCI interrupt mask.
*
* RETURNS:
*       GT_OK      - on success.
*       GT_FAIL    - otherwise.
*
* COMMENTS:
*       None.
*
* Galtis:
*       None.
*
* Toolkit:
*******************************************************************************/
GT_STATUS gtPresteraGetPciIntMask
(
    IN  GT_PCI_INT  pciInt,
    OUT GT_U32      *intMask
);

/*******************************************************************************
* gtPresteraGetDefIntrLine
*
* DESCRIPTION:
*       This routine return the default PCI interrupt line on board.
*
* INPUTS:
*
* OUTPUTS:
*
* RETURNS:
*       GT_PCI_INT - PCI interrupt line.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_PCI_INT  gtPresteraGetDefIntrLine(void);
#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif  /* __gtAppDemoPciConfigh */
