/* FUNCTION NAME:  cli_banner.c
* PURPOSE: Provide service functions of banner.
* NOTES:
* Copyright(C)      Accton Corporation, 2002
*/


#ifndef CLI_BANNER_H
#define CLI_BANNER_H

/* INCLUDE FILE DECLARATIONS
 */
#include "sys_type.h"

/* NAMING CONSTANT DECLARATIONS
 */
#define BANNER_MAX_DATA_LENGTH  32
#define BANNER_MAX_NOTE_DATA_LENGTH  150
#define BANNER_MAX_NUMBER_MANAGER_INFO 3

/* MACRO FUNCTION DECLARATIONS
 */

/* DATA TYPE DECLARATIONS
 */
typedef struct BannerManagerInfo_S
{
    char    name_ar[BANNER_MAX_DATA_LENGTH+1];
    char    phone_number_ar[BANNER_MAX_DATA_LENGTH+1];
}BannerManagerInfo_T;

typedef struct BannerEquipmentInfo_S
{
    char    manufacturer_id_ar[BANNER_MAX_DATA_LENGTH+1];
    char    floor_ar[BANNER_MAX_DATA_LENGTH+1];
    char    row_ar[BANNER_MAX_DATA_LENGTH+1];
    char    rack_ar[BANNER_MAX_DATA_LENGTH+1];
    char    self_rack_ar[BANNER_MAX_DATA_LENGTH+1];
    char    manufacturer_ar[BANNER_MAX_DATA_LENGTH+1];
}BannerEquipmentInfo_T;

typedef struct BannerDCPowerInfo_S
{
    char    floor_ar[BANNER_MAX_DATA_LENGTH+1];
    char    row_ar[BANNER_MAX_DATA_LENGTH+1];
    char    rack_ar[BANNER_MAX_DATA_LENGTH+1];
    char    electrical_circuit_ar[BANNER_MAX_DATA_LENGTH+1];
}BannerDCPowerInfo_T;

/* EXPORTED SUBPROGRAM SPECIFICATIONS
 */


/*--------------------------------------------------------------------------
 * ROUTINE NAME - CLI_BANNER_Initialize
 *---------------------------------------------------------------------------
 * PURPOSE:  initialize banner
 * INPUT:    none
 * OUTPUT:   NONE.
 * RETURN:   TRUE/FALSE.
 * NOTE:     NONE.
 *---------------------------------------------------------------------------
 */
BOOL_T CLI_BANNER_Initialize();

/* ---------------------------------------------------------------------
 * ROUTINE NAME  - CLI_BANNER_SetCompany
 * ---------------------------------------------------------------------
 * PURPOSE  : Set the company name in banner
 * INPUT    : company_p
 * OUTPUT   : none
 * RETURN   : TRUE/FALSE
 * NOTES    : none
 * ---------------------------------------------------------------------
 */
BOOL_T CLI_BANNER_SetCompany(char *company_p);

/* ---------------------------------------------------------------------
 * ROUTINE NAME  - CLI_BANNER_GetCompany
 * ---------------------------------------------------------------------
 * PURPOSE  : Get the company name in banner
 * INPUT    : none
 * OUTPUT   : company_p
 * RETURN   : TRUE/FALSE
 * NOTES    : none
 * ---------------------------------------------------------------------
 */
BOOL_T CLI_BANNER_GetCompany(char *company_p);

/* ---------------------------------------------------------------------
 * ROUTINE NAME  - CLI_BANNER_GetRunningCompany
 * ---------------------------------------------------------------------
 * PURPOSE: This function returns SYS_TYPE_GET_RUNNING_CFG_SUCCESS if
 *          the non-default company name is successfully retrieved.
 *          Otherwise, SYS_TYPE_GET_RUNNING_CFG_NO_CHANGE is returned.
 * INPUT: None.
 * OUTPUT: company_p
 * RETURN: SYS_TYPE_GET_RUNNING_CFG_SUCCESS/SYS_TYPE_GET_RUNNING_CFG_NO_CHANGE/SYS_TYPE_GET_RUNNING_CFG_FAIL
 * NOTES: 1. This function shall only be invoked by CLI to save the
 *           "running configuration" to local or remote files.
 *        2. Since only non-default configuration will be saved, this
 *           function shall return non-default system name.
 *        3. Caller has to prepare buffer for storing system name
 * ---------------------------------------------------------------------
 */
SYS_TYPE_Get_Running_Cfg_T CLI_BANNER_GetRunningCompany(char *company_p);

/* ---------------------------------------------------------------------
 * ROUTINE NAME  - CLI_BANNER_SetDepartment
 * ---------------------------------------------------------------------
 * PURPOSE  : Set the name of the responsible department in banner
 * INPUT    : department_p
 * OUTPUT   : none
 * RETURN   : TRUE/FALSE
 * NOTES    : none
 * ---------------------------------------------------------------------
 */
BOOL_T CLI_BANNER_SetDepartment(char *department_p);

/* ---------------------------------------------------------------------
 * ROUTINE NAME  - CLI_BANNER_GetDepartment
 * ---------------------------------------------------------------------
 * PURPOSE  : Get the name of the responsible department in banner
 * INPUT    : none
 * OUTPUT   : company_p
 * RETURN   : TRUE/FALSE
 * NOTES    : none
 * ---------------------------------------------------------------------
 */
BOOL_T CLI_BANNER_GetDepartment(char *department_p);

/* ---------------------------------------------------------------------
 * ROUTINE NAME  - CLI_BANNER_GetRunningDepartment
 * ---------------------------------------------------------------------
 * PURPOSE: This function returns SYS_TYPE_GET_RUNNING_CFG_SUCCESS if
 *          the non-default name of the responsible department is
 *          successfully retrieved.
 *          Otherwise, SYS_TYPE_GET_RUNNING_CFG_NO_CHANGE is returned.
 * INPUT: None.
 * OUTPUT: department_p
 * RETURN: SYS_TYPE_GET_RUNNING_CFG_SUCCESS/SYS_TYPE_GET_RUNNING_CFG_NO_CHANGE/SYS_TYPE_GET_RUNNING_CFG_FAIL
 * NOTES: 1. This function shall only be invoked by CLI to save the
 *           "running configuration" to local or remote files.
 *        2. Since only non-default configuration will be saved, this
 *           function shall return non-default system name.
 *        3. Caller has to prepare buffer for storing system name
 * ---------------------------------------------------------------------
 */
SYS_TYPE_Get_Running_Cfg_T CLI_BANNER_GetRunningDepartment(char *department_p);

/* ---------------------------------------------------------------------
 * ROUTINE NAME  - CLI_BANNER_SetManagerInfo
 * ---------------------------------------------------------------------
 * PURPOSE  : Set the name and telephone number to contact the manager in banner
 * INPUT    : index, manager_info_p
 * OUTPUT   : none
 * RETURN   : TRUE/FALSE
 * NOTES    : none
 * ---------------------------------------------------------------------
 */
BOOL_T CLI_BANNER_SetManagerInfo(UI32_T index, BannerManagerInfo_T *manager_info_p);

/* ---------------------------------------------------------------------
 * ROUTINE NAME  - CLI_BANNER_GetManagerInfo
 * ---------------------------------------------------------------------
 * PURPOSE  : Get the name and telephone number to contact the manager in banner
 * INPUT    : index
 * OUTPUT   : manager_info_p
 * RETURN   : TRUE/FALSE
 * NOTES    : none
 * ---------------------------------------------------------------------
 */
BOOL_T CLI_BANNER_GetManagerInfo(UI32_T index, BannerManagerInfo_T *manager_info_p);


/* ---------------------------------------------------------------------
 * ROUTINE NAME  - CLI_BANNER_GetRunningManagerInfoIsTrue
 * ---------------------------------------------------------------------
 * PURPOSE  : determine ManagerInfo with index
 * INPUT    : index
 * OUTPUT   : none
 * RETURN   : SYS_TYPE_GET_RUNNING_CFG_SUCCESS/SYS_TYPE_GET_RUNNING_CFG_NO_CHANGE/SYS_TYPE_GET_RUNNING_CFG_FAIL
 * NOTES    : none
 * ---------------------------------------------------------------------
 */
SYS_TYPE_Get_Running_Cfg_T CLI_BANNER_GetRunningManagerInfoIsTrue(UI32_T index);


/* ---------------------------------------------------------------------
 * ROUTINE NAME  - CLI_BANNER_GetRunningManagerInfo
 * ---------------------------------------------------------------------
 * PURPOSE: This function returns SYS_TYPE_GET_RUNNING_CFG_SUCCESS if
 *          the non-default name and telephone number to contact the
 *          manager is successfully retrieved.
 *          Otherwise, SYS_TYPE_GET_RUNNING_CFG_NO_CHANGE is returned.
 * INPUT: index.
 * OUTPUT: manager_info_p
 * RETURN: SYS_TYPE_GET_RUNNING_CFG_SUCCESS/SYS_TYPE_GET_RUNNING_CFG_NO_CHANGE/SYS_TYPE_GET_RUNNING_CFG_FAIL
 * NOTES: 1. This function shall only be invoked by CLI to save the
 *           "running configuration" to local or remote files.
 *        2. Since only non-default configuration will be saved, this
 *           function shall return non-default system name.
 *        3. Caller has to prepare buffer for storing system name
 * ---------------------------------------------------------------------
 */
SYS_TYPE_Get_Running_Cfg_T CLI_BANNER_GetRunningManagerInfo(UI32_T index, BannerManagerInfo_T *manager_info_p);

/* ---------------------------------------------------------------------
 * ROUTINE NAME  - CLI_BANNER_SetEquipmentLocation
 * ---------------------------------------------------------------------
 * PURPOSE  : Set the physical location of the equipment city and sreet
 *            address in banner.
 * INPUT    : equipment_location_p
 * OUTPUT   : none
 * RETURN   : TRUE/FALSE
 * NOTES    : none
 * ---------------------------------------------------------------------
 */
BOOL_T CLI_BANNER_SetEquipmentLocation(char *equipment_location_p);

/* ---------------------------------------------------------------------
 * ROUTINE NAME  - CLI_BANNER_GetEquipmentLocation
 * ---------------------------------------------------------------------
 * PURPOSE  : Get the physical location of the equipment city and sreet
 *            address in banner.
 * INPUT    : none
 * OUTPUT   : equipment_location_p
 * RETURN   : TRUE/FALSE
 * NOTES    : none
 * ---------------------------------------------------------------------
 */
BOOL_T CLI_BANNER_GetEquipmentLocation(char *equipment_location_p);

/* ---------------------------------------------------------------------
 * ROUTINE NAME  - CLI_BANNER_GetRunningEquipmentLocation
 * ---------------------------------------------------------------------
 * PURPOSE: This function returns SYS_TYPE_GET_RUNNING_CFG_SUCCESS if
 *          the non-default the physical location of the equipment city
 *          and sreet address in banner is successfully retrieved.
 *          Otherwise, SYS_TYPE_GET_RUNNING_CFG_NO_CHANGE is returned.
 * INPUT: None.
 * OUTPUT: equipment_location_p
 * RETURN: SYS_TYPE_GET_RUNNING_CFG_SUCCESS/SYS_TYPE_GET_RUNNING_CFG_NO_CHANGE/SYS_TYPE_GET_RUNNING_CFG_FAIL
 * NOTES: 1. This function shall only be invoked by CLI to save the
 *           "running configuration" to local or remote files.
 *        2. Since only non-default configuration will be saved, this
 *           function shall return non-default system name.
 *        3. Caller has to prepare buffer for storing system name
 * ---------------------------------------------------------------------
 */
SYS_TYPE_Get_Running_Cfg_T CLI_BANNER_GetRunningEquipmentLocation(char *equipment_location_p);

/* ---------------------------------------------------------------------
 * ROUTINE NAME  - CLI_BANNER_SetEquipmentInfo
 * ---------------------------------------------------------------------
 * PURPOSE  : Set the information about the equipment in banner.
 * INPUT    : equipment_info_p
 * OUTPUT   : none
 * RETURN   : TRUE/FALSE
 * NOTES    : none
 * ---------------------------------------------------------------------
 */
BOOL_T CLI_BANNER_SetEquipmentInfo(BannerEquipmentInfo_T *equipment_info_p);

/* ---------------------------------------------------------------------
 * ROUTINE NAME  - CLI_BANNER_GetEquipmentInfo
 * ---------------------------------------------------------------------
 * PURPOSE  : Get the information about the equipment in banner.
 * INPUT    : none
 * OUTPUT   : equipment_info_p
 * RETURN   : TRUE/FALSE
 * NOTES    : none
 * ---------------------------------------------------------------------
 */
BOOL_T CLI_BANNER_GetEquipmentInfo(BannerEquipmentInfo_T *equipment_info_p);

/* ---------------------------------------------------------------------
 * ROUTINE NAME  - CLI_BANNER_GetRunningEquipmentInfo
 * ---------------------------------------------------------------------
 * PURPOSE: This function returns SYS_TYPE_GET_RUNNING_CFG_SUCCESS if
 *          the non-default the information about the equipment in banner
 *          is successfully retrieved.
 *          Otherwise, SYS_TYPE_GET_RUNNING_CFG_NO_CHANGE is returned.
 * INPUT: None.
 * OUTPUT: equipment_info_p
 * RETURN: SYS_TYPE_GET_RUNNING_CFG_SUCCESS/SYS_TYPE_GET_RUNNING_CFG_NO_CHANGE/SYS_TYPE_GET_RUNNING_CFG_FAIL
 * NOTES: 1. This function shall only be invoked by CLI to save the
 *           "running configuration" to local or remote files.
 *        2. Since only non-default configuration will be saved, this
 *           function shall return non-default system name.
 *        3. Caller has to prepare buffer for storing system name
 * ---------------------------------------------------------------------
 */
SYS_TYPE_Get_Running_Cfg_T CLI_BANNER_GetRunningEquipmentInfo(BannerEquipmentInfo_T *equipment_info_p);

/* ---------------------------------------------------------------------
 * ROUTINE NAME  - CLI_BANNER_SetDCPowerInfo
 * ---------------------------------------------------------------------
 * PURPOSE  : Set the information about the DC power supply in banner.
 * INPUT    : dc_power_info_p
 * OUTPUT   : none
 * RETURN   : TRUE/FALSE
 * NOTES    : none
 * ---------------------------------------------------------------------
 */
BOOL_T CLI_BANNER_SetDCPowerInfo(BannerDCPowerInfo_T *dc_power_info_p);

/* ---------------------------------------------------------------------
 * ROUTINE NAME  - CLI_BANNER_GetDCPowerInfo
 * ---------------------------------------------------------------------
 * PURPOSE  : Get the information about the DC power supply in banner.
 * INPUT    : none
 * OUTPUT   : dc_power_info_p
 * RETURN   : TRUE/FALSE
 * NOTES    : none
 * ---------------------------------------------------------------------
 */
BOOL_T CLI_BANNER_GetDCPowerInfo(BannerDCPowerInfo_T *dc_power_info_p);

/* ---------------------------------------------------------------------
 * ROUTINE NAME  - CLI_BANNER_GetRunningDCPowerInfo
 * ---------------------------------------------------------------------
 * PURPOSE: This function returns SYS_TYPE_GET_RUNNING_CFG_SUCCESS if
 *          the non-default the information about the DC power supply
 *          in banner is successfully retrieved.
 *          Otherwise, SYS_TYPE_GET_RUNNING_CFG_NO_CHANGE is returned.
 * INPUT: None.
 * OUTPUT: dc_power_info_p
 * RETURN: SYS_TYPE_GET_RUNNING_CFG_SUCCESS/SYS_TYPE_GET_RUNNING_CFG_NO_CHANGE/SYS_TYPE_GET_RUNNING_CFG_FAIL
 * NOTES: 1. This function shall only be invoked by CLI to save the
 *           "running configuration" to local or remote files.
 *        2. Since only non-default configuration will be saved, this
 *           function shall return non-default system name.
 *        3. Caller has to prepare buffer for storing system name
 * ---------------------------------------------------------------------
 */
SYS_TYPE_Get_Running_Cfg_T CLI_BANNER_GetRunningDCPowerInfo(BannerDCPowerInfo_T *dc_power_info_p);

/* ---------------------------------------------------------------------
 * ROUTINE NAME  - CLI_BANNER_SetLPNumber
 * ---------------------------------------------------------------------
 * PURPOSE  : Set the LP number in banner.
 * INPUT    : lp_number_p
 * OUTPUT   : none
 * RETURN   : TRUE/FALSE
 * NOTES    : none
 * ---------------------------------------------------------------------
 */
BOOL_T CLI_BANNER_SetLPNumber(char *lp_number_p);

/* ---------------------------------------------------------------------
 * ROUTINE NAME  - CLI_BANNER_GetLPNumber
 * ---------------------------------------------------------------------
 * PURPOSE  : Get the LP number in banner.
 * INPUT    : none
 * OUTPUT   : lp_number_p
 * RETURN   : TRUE/FALSE
 * NOTES    : none
 * ---------------------------------------------------------------------
 */
BOOL_T CLI_BANNER_GetLPNumber(char *lp_number_p);

/* ---------------------------------------------------------------------
 * ROUTINE NAME  - CLI_BANNER_GetRunningLPNumber
 * ---------------------------------------------------------------------
 * PURPOSE: This function returns SYS_TYPE_GET_RUNNING_CFG_SUCCESS if
 *          the non-default the LP number in banner is successfully retrieved.
 *          Otherwise, SYS_TYPE_GET_RUNNING_CFG_NO_CHANGE is returned.
 * INPUT: None.
 * OUTPUT: lp_number_p
 * RETURN: SYS_TYPE_GET_RUNNING_CFG_SUCCESS/SYS_TYPE_GET_RUNNING_CFG_NO_CHANGE/SYS_TYPE_GET_RUNNING_CFG_FAIL
 * NOTES: 1. This function shall only be invoked by CLI to save the
 *           "running configuration" to local or remote files.
 *        2. Since only non-default configuration will be saved, this
 *           function shall return non-default system name.
 *        3. Caller has to prepare buffer for storing system name
 * ---------------------------------------------------------------------
 */
SYS_TYPE_Get_Running_Cfg_T CLI_BANNER_GetRunningLPNumber(char *lp_number_p);

/* ---------------------------------------------------------------------
 * ROUTINE NAME  - CLI_BANNER_SetMUX
 * ---------------------------------------------------------------------
 * PURPOSE  : Set the position of the equipment in the MUX in banner.
 * INPUT    : mux_p
 * OUTPUT   : none
 * RETURN   : TRUE/FALSE
 * NOTES    : none
 * ---------------------------------------------------------------------
 */
BOOL_T CLI_BANNER_SetMUX(char *mux_p);

/* ---------------------------------------------------------------------
 * ROUTINE NAME  - CLI_BANNER_GetMUX
 * ---------------------------------------------------------------------
 * PURPOSE  : Get the position of the equipment in the MUX in banner.
 * INPUT    : none
 * OUTPUT   : mux_p
 * RETURN   : TRUE/FALSE
 * NOTES    : none
 * ---------------------------------------------------------------------
 */
BOOL_T CLI_BANNER_GetMUX(char *mux_p);

/* ---------------------------------------------------------------------
 * ROUTINE NAME  - CLI_BANNER_GetRunningMUX
 * ---------------------------------------------------------------------
 * PURPOSE: This function returns SYS_TYPE_GET_RUNNING_CFG_SUCCESS if
 *          the non-default the position of the equipment in the MUX in banner
 *          is successfully retrieved.
 *          Otherwise, SYS_TYPE_GET_RUNNING_CFG_NO_CHANGE is returned.
 * INPUT: None.
 * OUTPUT: mux_p
 * RETURN: SYS_TYPE_GET_RUNNING_CFG_SUCCESS/SYS_TYPE_GET_RUNNING_CFG_NO_CHANGE/SYS_TYPE_GET_RUNNING_CFG_FAIL
 * NOTES: 1. This function shall only be invoked by CLI to save the
 *           "running configuration" to local or remote files.
 *        2. Since only non-default configuration will be saved, this
 *           function shall return non-default system name.
 *        3. Caller has to prepare buffer for storing system name
 * ---------------------------------------------------------------------
 */
SYS_TYPE_Get_Running_Cfg_T CLI_BANNER_GetRunningMUX(char *mux_p);

/* ---------------------------------------------------------------------
 * ROUTINE NAME  - CLI_BANNER_SetIpLan
 * ---------------------------------------------------------------------
 * PURPOSE  : Set the IP address and mask for the equipment in banner.
 * INPUT    : ip_lan_p
 * OUTPUT   : none
 * RETURN   : TRUE/FALSE
 * NOTES    : none
 * ---------------------------------------------------------------------
 */
BOOL_T CLI_BANNER_SetIpLan(char *ip_lan_p);

/* ---------------------------------------------------------------------
 * ROUTINE NAME  - CLI_BANNER_GetIpLan
 * ---------------------------------------------------------------------
 * PURPOSE  : Get the IP address and mask for the equipment in banner.
 * INPUT    : none
 * OUTPUT   : ip_lan_p
 * RETURN   : TRUE/FALSE
 * NOTES    : none
 * ---------------------------------------------------------------------
 */
BOOL_T CLI_BANNER_GetIpLan(char *ip_lan_p);

/* ---------------------------------------------------------------------
 * ROUTINE NAME  - CLI_BANNER_GetRunningIpLan
 * ---------------------------------------------------------------------
 * PURPOSE: This function returns SYS_TYPE_GET_RUNNING_CFG_SUCCESS if
 *          the non-default IP address and mask for the equipment in banner
 *          is successfully retrieved.
 *          Otherwise, SYS_TYPE_GET_RUNNING_CFG_NO_CHANGE is returned.
 * INPUT: None.
 * OUTPUT: ip_lan_p
 * RETURN: SYS_TYPE_GET_RUNNING_CFG_SUCCESS/SYS_TYPE_GET_RUNNING_CFG_NO_CHANGE/SYS_TYPE_GET_RUNNING_CFG_FAIL
 * NOTES: 1. This function shall only be invoked by CLI to save the
 *           "running configuration" to local or remote files.
 *        2. Since only non-default configuration will be saved, this
 *           function shall return non-default system name.
 *        3. Caller has to prepare buffer for storing system name
 * ---------------------------------------------------------------------
 */
SYS_TYPE_Get_Running_Cfg_T CLI_BANNER_GetRunningIpLan(char *ip_lan_p);

/* ---------------------------------------------------------------------
 * ROUTINE NAME  - CLI_BANNER_SetNote
 * ---------------------------------------------------------------------
 * PURPOSE  : Set the additional information in banner.
 * INPUT    : note_p
 * OUTPUT   : none
 * RETURN   : TRUE/FALSE
 * NOTES    : none
 * ---------------------------------------------------------------------
 */
BOOL_T CLI_BANNER_SetNote(char *note_p);

/* ---------------------------------------------------------------------
 * ROUTINE NAME  - CLI_BANNER_GetNote
 * ---------------------------------------------------------------------
 * PURPOSE  : Get the additional information in banner.
 * INPUT    : none
 * OUTPUT   : note_p
 * RETURN   : TRUE/FALSE
 * NOTES    : none
 * ---------------------------------------------------------------------
 */
BOOL_T CLI_BANNER_GetNote(char *note_p);

/* ---------------------------------------------------------------------
 * ROUTINE NAME  - CLI_BANNER_GetRunningNote
 * ---------------------------------------------------------------------
 * PURPOSE: This function returns SYS_TYPE_GET_RUNNING_CFG_SUCCESS if
 *          the non-default the additional information in banner is successfully retrieved.
 *          Otherwise, SYS_TYPE_GET_RUNNING_CFG_NO_CHANGE is returned.
 * INPUT: None.
 * OUTPUT: note_p
 * RETURN: SYS_TYPE_GET_RUNNING_CFG_SUCCESS/SYS_TYPE_GET_RUNNING_CFG_NO_CHANGE/SYS_TYPE_GET_RUNNING_CFG_FAIL
 * NOTES: 1. This function shall only be invoked by CLI to save the
 *           "running configuration" to local or remote files.
 *        2. Since only non-default configuration will be saved, this
 *           function shall return non-default system name.
 *        3. Caller has to prepare buffer for storing system name
 * ---------------------------------------------------------------------
 */
SYS_TYPE_Get_Running_Cfg_T CLI_BANNER_GetRunningNote(char *note_p);

#endif /*CLI_BANNER_H*/
