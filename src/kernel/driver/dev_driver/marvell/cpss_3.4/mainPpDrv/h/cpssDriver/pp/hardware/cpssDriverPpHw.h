/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDriverPpHw.h
*
* DESCRIPTION:
*       CPSS PP Driver Hardware Access API
*
* FILE REVISION NUMBER:
*       $Revision: 4 $
*
*******************************************************************************/
#ifndef __cpssDriverPpHwh
#define __cpssDriverPpHwh

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <cpss/generic/cpssTypes.h>


/*******************************************************************************
*   Note: about the 'portGroupId' parameter , in the next APIs.
*   portGroupId - The port group Id. relevant only to 'multi-port-groups' devices.
*       when value is CPSS_PORT_GROUP_UNAWARE_MODE_CNS :
*       1. For write APIs CPSS_PORT_GROUP_UNAWARE_MODE_CNS, meaning function should write
*          all active port groups , to the specified table entry
*          (same as there was no portGroupId parameter)
*       2. For read APIs CPSS_PORT_GROUP_UNAWARE_MODE_CNS, meaning function should read
*          register only from 1st active port group , from the specified table entry
*          (same as there was no portGroupId parameter).
*******************************************************************************/


/*
 * typedef: enum  CPSS_DRV_HW_TRACE_TYPE_ENT
 *
 * Description: HW Trace type
 *
 * Fields:
 *      CPSS_DRV_HW_TRACE_TYPE_READ_E   - HW access is read.
 *      CPSS_DRV_HW_TRACE_TYPE_WRITE_E  - HW access is write.
 *      CPSS_DRV_HW_TRACE_TYPE_BOTH_E   - Both HW access read and write.
 */
typedef enum
{
    CPSS_DRV_HW_TRACE_TYPE_READ_E,
    CPSS_DRV_HW_TRACE_TYPE_WRITE_E,
    CPSS_DRV_HW_TRACE_TYPE_BOTH_E
}CPSS_DRV_HW_TRACE_TYPE_ENT;


/*******************************************************************************
* cpssDrvPpHwRegisterRead
*
* DESCRIPTION:
*       Read a register value from the given PP.
*
* INPUTS:
*       devNum  - The PP to read from.
*       portGroupId  - The port group Id. relevant only to 'multi-port-groups' devices.
*                 supports value CPSS_PORT_GROUP_UNAWARE_MODE_CNS
*       regAddr - The register's address to read from.
*
* OUTPUTS:
*       data - Includes the register value.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on hardware error
*       GT_NOT_INITIALIZED - if the driver was not initialized
*       GT_HW_ERROR - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDrvPpHwRegisterRead
(
    IN GT_U8    devNum,
    IN GT_U32   portGroupId,
    IN GT_U32   regAddr,
    OUT GT_U32  *data
);

/*******************************************************************************
* cpssDrvPpHwRegisterWrite
*
* DESCRIPTION:
*       Write to a PP's given register.
*
* INPUTS:
*       devNum  - The PP to write to.
*       portGroupId  - The port group Id. relevant only to 'multi-port-groups' devices.
*                 supports value CPSS_PORT_GROUP_UNAWARE_MODE_CNS
*       regAddr - The register's address to write to.
*       data    - The value to be written.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on hardware error
*       GT_NOT_INITIALIZED - if the driver was not initialized
*       GT_HW_ERROR - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDrvPpHwRegisterWrite
(
    IN GT_U8    devNum,
    IN GT_U32   portGroupId,
    IN GT_U32   regAddr,
    IN GT_U32   value
);

/*******************************************************************************
* cpssDrvPpHwRegFieldGet
*
* DESCRIPTION:
*       Read a selected register field.
*
* INPUTS:
*       devNum  - The PP device number to read from.
*       portGroupId  - The port group Id. relevant only to 'multi-port-groups' devices.
*                 supports value CPSS_PORT_GROUP_UNAWARE_MODE_CNS
*       regAddr - The register's address to read from.
*       fieldOffset - The start bit number in the register.
*       fieldLength - The number of bits to be read.
*
* OUTPUTS:
*       fieldData   - Data to read from the register.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on hardware error
*       GT_NOT_INITIALIZED - if the driver was not initialized
*       GT_HW_ERROR - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDrvPpHwRegFieldGet
(
    IN GT_U8    devNum,
    IN GT_U32   portGroupId,
    IN GT_U32   regAddr,
    IN GT_U32   fieldOffset,
    IN GT_U32   fieldLength,
    OUT GT_U32  *fieldData
);

/*******************************************************************************
* cpssDrvPpHwRegFieldSet
*
* DESCRIPTION:
*       Write value to selected register field.
*
* INPUTS:
*       devNum  - The PP device number to write to.
*       portGroupId  - The port group Id. relevant only to 'multi-port-groups' devices.
*                 supports value CPSS_PORT_GROUP_UNAWARE_MODE_CNS
*       regAddr - The register's address to write to.
*       fieldOffset - The start bit number in the register.
*       fieldLength - The number of bits to be written to register.
*       fieldData   - Data to be written into the register.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on hardware error
*       GT_NOT_INITIALIZED - if the driver was not initialized
*       GT_HW_ERROR - on hardware error
*
* COMMENTS:
*       this function actually read the register modifies the requested field
*       and writes the new value back to the HW.
*
*******************************************************************************/
GT_STATUS cpssDrvPpHwRegFieldSet
(
    IN GT_U8    devNum,
    IN GT_U32   portGroupId,
    IN GT_U32   regAddr,
    IN GT_U32   fieldOffset,
    IN GT_U32   fieldLength,
    IN GT_U32   fieldData

);

/*******************************************************************************
* cpssDrvPpHwRegBitMaskRead
*
* DESCRIPTION:
*       Reads the unmasked bits of a register.
*
* INPUTS:
*       devNum  - PP device number to read from.
*       portGroupId  - The port group Id. relevant only to 'multi-port-groups' devices.
*                 supports value CPSS_PORT_GROUP_UNAWARE_MODE_CNS
*       regAddr - Register address to read from.
*       mask    - Mask for selecting the read bits.
*
* OUTPUTS:
*       dataPtr    - Data read from register.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on hardware error
*       GT_NOT_INITIALIZED - if the driver was not initialized
*       GT_HW_ERROR - on hardware error
*
* COMMENTS:
*       The bits in value to be read are the masked bit of 'mask'.
*
*******************************************************************************/
GT_STATUS cpssDrvPpHwRegBitMaskRead
(
    IN GT_U8    devNum,
    IN GT_U32   portGroupId,
    IN GT_U32   regAddr,
    IN GT_U32   mask,
    OUT GT_U32  *dataPtr
);

/*******************************************************************************
* cpssDrvPpHwRegBitMaskWrite
*
* DESCRIPTION:
*       Writes the unmasked bits of a register.
*
* INPUTS:
*       devNum  - PP device number to write to.
*       portGroupId  - The port group Id. relevant only to 'multi-port-groups' devices.
*                 supports value CPSS_PORT_GROUP_UNAWARE_MODE_CNS
*       regAddr - Register address to write to.
*       mask    - Mask for selecting the written bits.
*       value   - Data to be written to register.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on hardware error
*       GT_NOT_INITIALIZED - if the driver was not initialized
*       GT_HW_ERROR - on hardware error
*
* COMMENTS:
*       The bits in value to be written are the masked bit of 'mask'.
*
*******************************************************************************/
GT_STATUS cpssDrvPpHwRegBitMaskWrite
(
    IN GT_U8    devNum,
    IN GT_U32   portGroupId,
    IN GT_U32   regAddr,
    IN GT_U32   mask,
    IN GT_U32   value
);

/*******************************************************************************
* cpssDrvPpHwRamRead
*
* DESCRIPTION:
*       Read from PP's RAM.
*
* INPUTS:
*       devNum  - The PP device number to read from.
*       portGroupId  - The port group Id. relevant only to 'multi-port-groups' devices.
*                 supports value CPSS_PORT_GROUP_UNAWARE_MODE_CNS
*       addr    - Address offset to read from.
*       length  - Number of Words (4 byte) to read.
*
* OUTPUTS:
*       data    - An array containing the read data.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on hardware error
*       GT_NOT_INITIALIZED - if the driver was not initialized
*       GT_HW_ERROR - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDrvPpHwRamRead
(
    IN GT_U8    devNum,
    IN GT_U32   portGroupId,
    IN GT_U32   addr,
    IN GT_U32   length,
    OUT GT_U32  *data
);

/*******************************************************************************
* cpssDrvPpHwRamWrite
*
* DESCRIPTION:
*       Writes to PP's RAM.
*
* INPUTS:
*       devNum  - The PP device number to write to.
*       portGroupId  - The port group Id. relevant only to 'multi-port-groups' devices.
*                 supports value CPSS_PORT_GROUP_UNAWARE_MODE_CNS
*       addr    - Address offset to write to.
*       length  - Number of Words (4 byte) to write.
*       data    - An array containing the data to be written.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on hardware error
*       GT_NOT_INITIALIZED - if the driver was not initialized
*       GT_HW_ERROR - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDrvPpHwRamWrite
(
    IN GT_U8    devNum,
    IN GT_U32   portGroupId,
    IN GT_U32   addr,
    IN GT_U32   length,
    IN GT_U32   *data
);

/*******************************************************************************
* cpssDrvPpHwVectorRead
*
* DESCRIPTION:
*       Read from PP's RAM a vector of addresses.
*
* INPUTS:
*       devNum  - The PP device number to read from.
*       portGroupId  - The port group Id. relevant only to 'multi-port-groups' devices.
*                 supports value CPSS_PORT_GROUP_UNAWARE_MODE_CNS
*       addrArr - Address array to read from.
*       arrLen  - The size of addrArr/dataArr.
*
* OUTPUTS:
*       dataArr - An array containing the read data.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on hardware error
*       GT_NOT_INITIALIZED - if the driver was not initialized
*       GT_HW_ERROR - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDrvPpHwVectorRead
(
    IN GT_U8    devNum,
    IN GT_U32   portGroupId,
    IN GT_U32   addrArr[],
    OUT GT_U32  dataArr[],
    IN GT_U32   arrLen
);

/*******************************************************************************
* cpssDrvPpHwVectorWrite
*
* DESCRIPTION:
*       Writes to PP's RAM a vector of addresses.
*
* INPUTS:
*       devNum  - The PP device number to write to.
*       portGroupId  - The port group Id. relevant only to 'multi-port-groups' devices.
*                 supports value CPSS_PORT_GROUP_UNAWARE_MODE_CNS
*       addrArr - Address offset to write to.
*       dataArr - An array containing the data to be written.
*       arrLen  - The size of addrArr/dataArr.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on hardware error
*       GT_NOT_INITIALIZED - if the driver was not initialized
*       GT_HW_ERROR - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDrvPpHwVectorWrite
(
    IN GT_U8    devNum,
    IN GT_U32   portGroupId,
    IN GT_U32   addrArr[],
    IN GT_U32   dataArr[],
    IN GT_U32   arrLen
);

/*******************************************************************************
* cpssDrvPpHwRamInReverseWrite
*
* DESCRIPTION:
*       Writes to PP's RAM in reverse.
*
* INPUTS:
*       devNum  - The PP device number to write to.
*       portGroupId  - The port group Id. relevant only to 'multi-port-groups' devices.
*                 supports value CPSS_PORT_GROUP_UNAWARE_MODE_CNS
*       addr    - Address offset to write to.
*       length  - Number of Words (4 byte) to write.
*       data    - An array containing the data to be written.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on hardware error
*       GT_NOT_INITIALIZED - if the driver was not initialized
*       GT_HW_ERROR - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDrvPpHwRamInReverseWrite
(
    IN GT_U8    devNum,
    IN GT_U32   portGroupId,
    IN GT_U32   addr,
    IN GT_U32   length,
    IN GT_U32   *data
);

/*******************************************************************************
* cpssDrvPpHwIsrRead
*
* DESCRIPTION:
*       Read a register value using special interrupt address completion region.
*
* INPUTS:
*       devNum  - The PP to read from.
*       portGroupId  - The port group Id. relevant only to 'multi-port-groups' devices.
*                 supports value CPSS_PORT_GROUP_UNAWARE_MODE_CNS
*       regAddr - The register's address to read from.
*                 Note: regAddr should be < 0x1000000
*
* OUTPUTS:
*       dataPtr - Includes the register value.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on hardware error
*       GT_NOT_INITIALIZED - if the driver was not initialized
*       GT_HW_ERROR - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_INLINE GT_STATUS cpssDrvPpHwIsrRead
(
    IN GT_U8    devNum,
    IN GT_U32   portGroupId,
    IN GT_U32   regAddr,
    IN GT_U32   *dataPtr
);

/*******************************************************************************
* cpssDrvPpHwIsrWrite
*
* DESCRIPTION:
*       Write a register value using special interrupt address completion region
*
* INPUTS:
*       devNum  - The PP to write to.
*       portGroupId  - The port group Id. relevant only to 'multi-port-groups' devices.
*                 supports value CPSS_PORT_GROUP_UNAWARE_MODE_CNS
*       regAddr - The register's address to write to.
*       data    - The value to be written.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on hardware error
*       GT_NOT_INITIALIZED - if the driver was not initialized
*       GT_HW_ERROR - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDrvPpHwIsrWrite
(
    IN GT_U8    devNum,
    IN GT_U32   portGroupId,
    IN GT_U32   regAddr,
    IN GT_U32   value
);

/*******************************************************************************
* cpssDrvPpHwInternalPciRegRead
*
*
* DESCRIPTION:
*       This function reads from an internal pci register, it's used by the
*       initialization process and the interrupt service routine.
*
* INPUTS:
*       devNum  - The Pp's device numbers.
*       portGroupId  - The port group Id. relevant only to 'multi-port-groups' devices.
*                 supports value CPSS_PORT_GROUP_UNAWARE_MODE_CNS
*       regAddr - The register's address to read from.
*
* OUTPUTS:
*       data    - The read data.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*       GT_NOT_INITIALIZED - if the driver was not initialized
*       GT_HW_ERROR - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDrvPpHwInternalPciRegRead
(
    IN  GT_U8   devNum,
    IN GT_U32   portGroupId,
    IN  GT_U32  regAddr,
    OUT GT_U32  *data
);

/*******************************************************************************
* cpssDrvPpHwInternalPciRegWrite
*
* DESCRIPTION:
*       This function reads from an internal pci register, it's used by the
*       initialization process and the interrupt service routine.
*
* INPUTS:
*       devNum  - The Pp's device numbers.
*       portGroupId  - The port group Id. relevant only to 'multi-port-groups' devices.
*                 supports value CPSS_PORT_GROUP_UNAWARE_MODE_CNS
*       regAddr - The register's address to read from.
*       data    - Data to be written.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*       GT_NOT_INITIALIZED - if the driver was not initialized
*       GT_HW_ERROR - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDrvPpHwInternalPciRegWrite
(
    IN  GT_U8   devNum,
    IN GT_U32   portGroupId,
    IN  GT_U32  regAddr,
    IN  GT_U32  data
);

/*******************************************************************************
* cpssDrvPpHwByteSwap
*
* DESCRIPTION:
*       This function performs a byte swap on a given word IF the PCI bus swap
*       the info from the PP - otherwise the function do nothing
*
* INPUTS:
*       devNum  - device on which the byte swap is performed.
*       portGroupId  - The port group Id. relevant only to 'multi-port-groups' devices.
*                 supports value CPSS_PORT_GROUP_UNAWARE_MODE_CNS
*       data    - The data to be byte swapped.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       The data after performing byte-swapping (if needed).
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_INLINE GT_U32 cpssDrvPpHwByteSwap
(
    IN  GT_U8   devNum,
    IN GT_U32   portGroupId,
    IN  GT_U32  data
);

/*******************************************************************************
* cpssDrvPpHwTraceEnable
*
* DESCRIPTION:
*       This function enables/disables to trace hw access. 
*
* INPUTS:
*       devNum      - The Pp's device number.
*       traceType   - trace access type.
*       enable      - GT_TRUE: enable hw access info tracing
*                     GT_FALSE: disable hw access info tracing
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDrvPpHwTraceEnable
(
    IN GT_U8                        devNum,
    IN CPSS_DRV_HW_TRACE_TYPE_ENT   traceType,
    IN GT_BOOL                      enable
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __cpssDriverPpHwh */

