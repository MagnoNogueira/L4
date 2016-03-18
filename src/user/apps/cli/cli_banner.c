/* FUNCTION NAME: cli_banner.h
* PURPOSE: Provide service functions of banner.
* NOTES:
* REASON:
* Copyright(C)      Accton Corporation, 2002
*/

/* INCLUDE FILE DECLARATIONS
 */
#include <stdio.h>
#include <string.h>
#include "sys_cpnt.h"

#if (SYS_CPNT_CLI_BANNER == TRUE)
#include "cli_banner.h"
#endif

/* NAMING CONSTANT DECLARATIONS
 */
/* MACRO FUNCTION DECLARATIONS
 */
/* DATA TYPE DECLARATIONS
 */
/* LOCAL SUBPROGRAM DECLARATIONS
 */
/* STATIC VARIABLE DECLARATIONS
 */

#if (SYS_CPNT_CLI_BANNER == TRUE)
 static char company[BANNER_MAX_DATA_LENGTH+1];
 static char department[BANNER_MAX_DATA_LENGTH+1];
 static BannerManagerInfo_T manager_info[BANNER_MAX_NUMBER_MANAGER_INFO];
 static char equipment_location[BANNER_MAX_DATA_LENGTH+1];
 static BannerEquipmentInfo_T equipment_info;
 static BannerDCPowerInfo_T dc_power_info;
 static char lp_number[BANNER_MAX_DATA_LENGTH+1];
 static char mux[BANNER_MAX_DATA_LENGTH+1];
 static char ip_lan[BANNER_MAX_DATA_LENGTH+1];
 static char note[BANNER_MAX_NOTE_DATA_LENGTH+1];



/* EXPORTED SUBPROGRAM BODIES
 */
/* LOCAL SUBPROGRAM BODIES
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
BOOL_T CLI_BANNER_Initialize()
{
    /* row status 0 implies destroy so it can be initialized by memset() function */
    memset(company, 0, sizeof(company));
    memset(department, 0, sizeof(department));
    memset(manager_info, 0, sizeof(BannerManagerInfo_T));
    memset(equipment_location, 0, sizeof(equipment_location));
    memset(&equipment_info, 0, sizeof(BannerEquipmentInfo_T));
    memset(&dc_power_info, 0, sizeof(BannerDCPowerInfo_T));
    memset(lp_number, 0, sizeof(lp_number));
    memset(mux, 0, sizeof(mux));
    memset(ip_lan, 0, sizeof(ip_lan));
    memset(note, 0, sizeof(note));

    return TRUE;
}

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
BOOL_T CLI_BANNER_SetCompany(char *company_p)
{
    if(company_p == NULL)
        return FALSE;
    strncpy(company, company_p, BANNER_MAX_DATA_LENGTH);
    return TRUE;
}

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
BOOL_T CLI_BANNER_GetCompany(char *company_p)
{
    if(company_p == NULL)
        return FALSE;
    strncpy(company_p, company, BANNER_MAX_DATA_LENGTH);
    return TRUE;
}

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
SYS_TYPE_Get_Running_Cfg_T CLI_BANNER_GetRunningCompany(char *company_p)
{
    if(company_p == NULL)
        return SYS_TYPE_GET_RUNNING_CFG_FAIL;
    if(strlen(company) == 0)
    {
        strcpy(company_p,"");
        return SYS_TYPE_GET_RUNNING_CFG_NO_CHANGE;
    }
    strncpy(company_p, company, BANNER_MAX_DATA_LENGTH);
    return SYS_TYPE_GET_RUNNING_CFG_SUCCESS;
}

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
BOOL_T CLI_BANNER_SetDepartment(char *department_p)
{
    if(department_p == NULL)
        return FALSE;
    strncpy(department, department_p, BANNER_MAX_DATA_LENGTH);
    return TRUE;
}

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
BOOL_T CLI_BANNER_GetDepartment(char *department_p)
{
    if(department_p == NULL)
        return FALSE;
    strncpy(department_p, department, BANNER_MAX_DATA_LENGTH);
    return TRUE;
}

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
SYS_TYPE_Get_Running_Cfg_T CLI_BANNER_GetRunningDepartment(char *department_p)
{
    if(department_p == NULL)
        return SYS_TYPE_GET_RUNNING_CFG_FAIL;
    if(strlen(department) == 0)
    {
        strcpy(department_p,"");
        return SYS_TYPE_GET_RUNNING_CFG_NO_CHANGE;
    }
    strncpy(department_p, department, BANNER_MAX_DATA_LENGTH);
    return SYS_TYPE_GET_RUNNING_CFG_SUCCESS;
}

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
BOOL_T CLI_BANNER_SetManagerInfo(UI32_T index, BannerManagerInfo_T *manager_info_p)
{
    memset(&manager_info[index-1], 0, sizeof(BannerManagerInfo_T));

    if(manager_info_p == NULL)
        return FALSE;
    memcpy(&manager_info[index-1], manager_info_p, sizeof(BannerManagerInfo_T));
    return TRUE;
}

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
BOOL_T CLI_BANNER_GetManagerInfo(UI32_T index, BannerManagerInfo_T *manager_info_p)
{
    if(manager_info_p == NULL)
        return FALSE;
    memcpy(manager_info_p, &manager_info[index-1], sizeof(BannerManagerInfo_T));
    return TRUE;
}


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
SYS_TYPE_Get_Running_Cfg_T CLI_BANNER_GetRunningManagerInfoIsTrue(UI32_T index)
{
    UI32_T ret_val = SYS_TYPE_GET_RUNNING_CFG_NO_CHANGE;

    if(strlen(manager_info[index-1].name_ar) > 0)
    {
        ret_val = SYS_TYPE_GET_RUNNING_CFG_SUCCESS;
    }
    else
    {
        ret_val = SYS_TYPE_GET_RUNNING_CFG_NO_CHANGE;
    }

    if(strlen(manager_info[index-1].phone_number_ar) > 0)
    {
        ret_val = SYS_TYPE_GET_RUNNING_CFG_SUCCESS;
    }
    else
    {
        ret_val = SYS_TYPE_GET_RUNNING_CFG_NO_CHANGE;
    }

    return ret_val;
}

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
SYS_TYPE_Get_Running_Cfg_T CLI_BANNER_GetRunningManagerInfo(UI32_T index, BannerManagerInfo_T *manager_info_p)
{
    UI32_T ret_val = SYS_TYPE_GET_RUNNING_CFG_NO_CHANGE;

    if(manager_info_p == NULL)
        return SYS_TYPE_GET_RUNNING_CFG_FAIL;
    if(strlen(manager_info[index-1].name_ar) > 0)
    {
        strncpy(manager_info_p->name_ar, manager_info[index-1].name_ar, BANNER_MAX_DATA_LENGTH);
        ret_val = SYS_TYPE_GET_RUNNING_CFG_SUCCESS;
    }
    else
    {
        strcpy(manager_info_p->name_ar,"");
        ret_val = SYS_TYPE_GET_RUNNING_CFG_SUCCESS;
    }

    if(strlen(manager_info[index-1].phone_number_ar) > 0)
    {
        strncpy(manager_info_p->phone_number_ar, manager_info[index-1].phone_number_ar, BANNER_MAX_DATA_LENGTH);
        ret_val = SYS_TYPE_GET_RUNNING_CFG_SUCCESS;
    }
    else
    {
        strcpy(manager_info_p->phone_number_ar,"");
        ret_val = SYS_TYPE_GET_RUNNING_CFG_SUCCESS;
    }

    return ret_val;
}

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
BOOL_T CLI_BANNER_SetEquipmentLocation(char *equipment_location_p)
{
    if(equipment_location_p == NULL)
        return FALSE;
    strncpy(equipment_location, equipment_location_p, BANNER_MAX_DATA_LENGTH);
    return TRUE;
}

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
BOOL_T CLI_BANNER_GetEquipmentLocation(char *equipment_location_p)
{
    if(equipment_location_p == NULL)
        return FALSE;
    strncpy(equipment_location_p, equipment_location, BANNER_MAX_DATA_LENGTH);
    return TRUE;
}

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
SYS_TYPE_Get_Running_Cfg_T CLI_BANNER_GetRunningEquipmentLocation(char *equipment_location_p)
{
    if(equipment_location_p == NULL)
        return SYS_TYPE_GET_RUNNING_CFG_FAIL;
    if(strlen(equipment_location) == 0)
    {
        strcpy(equipment_location_p,"");
        return SYS_TYPE_GET_RUNNING_CFG_NO_CHANGE;
    }
    strncpy(equipment_location_p, equipment_location, BANNER_MAX_DATA_LENGTH);
    return SYS_TYPE_GET_RUNNING_CFG_SUCCESS;
}

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
BOOL_T CLI_BANNER_SetEquipmentInfo(BannerEquipmentInfo_T *equipment_info_p)
{
    memset(&equipment_info, 0, sizeof(BannerEquipmentInfo_T));

    if(equipment_info_p == NULL)
        return FALSE;
    memcpy(&equipment_info, equipment_info_p, sizeof(BannerEquipmentInfo_T));
    return TRUE;
}

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
BOOL_T CLI_BANNER_GetEquipmentInfo(BannerEquipmentInfo_T *equipment_info_p)
{
    if(equipment_info_p == NULL)
        return FALSE;
    memcpy(equipment_info_p, &equipment_info, sizeof(BannerEquipmentInfo_T));
    return TRUE;
}

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
SYS_TYPE_Get_Running_Cfg_T CLI_BANNER_GetRunningEquipmentInfo(BannerEquipmentInfo_T *equipment_info_p)
{
    UI32_T ret_val = SYS_TYPE_GET_RUNNING_CFG_NO_CHANGE;

    if(equipment_info_p == NULL)
        return SYS_TYPE_GET_RUNNING_CFG_FAIL;

    if(strlen(equipment_info.manufacturer_ar) > 0)
    {
        strncpy(equipment_info_p->manufacturer_ar, equipment_info.manufacturer_ar, BANNER_MAX_DATA_LENGTH);
        ret_val = SYS_TYPE_GET_RUNNING_CFG_SUCCESS;
    }
    else
    {
        strcpy(equipment_info_p->manufacturer_ar,"");
    }

    if(strlen(equipment_info.manufacturer_id_ar) > 0)
    {
        strncpy(equipment_info_p->manufacturer_id_ar, equipment_info.manufacturer_id_ar, BANNER_MAX_DATA_LENGTH);
        ret_val = SYS_TYPE_GET_RUNNING_CFG_SUCCESS;
    }
    else
    {
        strcpy(equipment_info_p->manufacturer_id_ar,"");
    }

    if(strlen(equipment_info.floor_ar) > 0)
    {
        strncpy(equipment_info_p->floor_ar, equipment_info.floor_ar, BANNER_MAX_DATA_LENGTH);
        ret_val = SYS_TYPE_GET_RUNNING_CFG_SUCCESS;
    }
    else
    {
        strcpy(equipment_info_p->floor_ar,"");
    }

    if(strlen(equipment_info.row_ar) > 0)
    {
        strncpy(equipment_info_p->row_ar, equipment_info.row_ar, BANNER_MAX_DATA_LENGTH);
        ret_val = SYS_TYPE_GET_RUNNING_CFG_SUCCESS;
    }
    else
    {
        strcpy(equipment_info_p->row_ar,"");
    }

    if(strlen(equipment_info.rack_ar) > 0)
    {
        strncpy(equipment_info_p->rack_ar, equipment_info.rack_ar, BANNER_MAX_DATA_LENGTH);
        ret_val = SYS_TYPE_GET_RUNNING_CFG_SUCCESS;
    }
    else
    {
        strcpy(equipment_info_p->rack_ar,"");

    }

    if(strlen(equipment_info.self_rack_ar) > 0)
    {
        strncpy(equipment_info_p->self_rack_ar, equipment_info.self_rack_ar, BANNER_MAX_DATA_LENGTH);
        ret_val = SYS_TYPE_GET_RUNNING_CFG_SUCCESS;
    }
    else
    {
        strcpy(equipment_info_p->self_rack_ar,"");
    }

    return ret_val;
}

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
BOOL_T CLI_BANNER_SetDCPowerInfo(BannerDCPowerInfo_T *dc_power_info_p)
{
    memset(&dc_power_info, 0, sizeof(BannerDCPowerInfo_T));

    if(dc_power_info_p == NULL)
        return FALSE;
    memcpy(&dc_power_info, dc_power_info_p, sizeof(BannerDCPowerInfo_T));
    return TRUE;
}

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
BOOL_T CLI_BANNER_GetDCPowerInfo(BannerDCPowerInfo_T *dc_power_info_p)
{
    if(dc_power_info_p == NULL)
        return FALSE;
    memcpy(dc_power_info_p, &dc_power_info, sizeof(BannerDCPowerInfo_T));
    return TRUE;
}

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
SYS_TYPE_Get_Running_Cfg_T CLI_BANNER_GetRunningDCPowerInfo(BannerDCPowerInfo_T *dc_power_info_p)
{
    UI32_T ret_val = SYS_TYPE_GET_RUNNING_CFG_NO_CHANGE;

    if(dc_power_info_p == NULL)
        return SYS_TYPE_GET_RUNNING_CFG_FAIL;

    if(strlen(dc_power_info.floor_ar) > 0)
    {
        strncpy(dc_power_info_p->floor_ar, dc_power_info.floor_ar, BANNER_MAX_DATA_LENGTH);
        ret_val = SYS_TYPE_GET_RUNNING_CFG_SUCCESS;
    }
    else
    {
        strcpy(dc_power_info_p->floor_ar,"");
    }

    if(strlen(dc_power_info.row_ar) > 0)
    {
        strncpy(dc_power_info_p->row_ar, dc_power_info.row_ar, BANNER_MAX_DATA_LENGTH);
        ret_val = SYS_TYPE_GET_RUNNING_CFG_SUCCESS;
    }
    else
    {
        strcpy(dc_power_info_p->row_ar,"");
    }

    if(strlen(dc_power_info.rack_ar) > 0)
    {
        strncpy(dc_power_info_p->rack_ar, dc_power_info.rack_ar, BANNER_MAX_DATA_LENGTH);
        ret_val = SYS_TYPE_GET_RUNNING_CFG_SUCCESS;
    }
    else
    {
        strcpy(dc_power_info_p->rack_ar,"");
    }

    if(strlen(dc_power_info.electrical_circuit_ar) > 0)
    {
        strncpy(dc_power_info_p->electrical_circuit_ar, dc_power_info.electrical_circuit_ar, BANNER_MAX_DATA_LENGTH);
        ret_val = SYS_TYPE_GET_RUNNING_CFG_SUCCESS;
    }
    else
    {
        strcpy(dc_power_info_p->electrical_circuit_ar,"");
    }

    return ret_val;
}

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
BOOL_T CLI_BANNER_SetLPNumber(char *lp_number_p)
{
    if(lp_number_p == NULL)
        return FALSE;
    strncpy(lp_number, lp_number_p, BANNER_MAX_DATA_LENGTH);
    return TRUE;
}

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
BOOL_T CLI_BANNER_GetLPNumber(char *lp_number_p)
{
    if(lp_number_p == NULL)
        return FALSE;
    strncpy(lp_number_p, lp_number, BANNER_MAX_DATA_LENGTH);
    return TRUE;
}

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
SYS_TYPE_Get_Running_Cfg_T CLI_BANNER_GetRunningLPNumber(char *lp_number_p)
{
    if(lp_number_p == NULL)
        return SYS_TYPE_GET_RUNNING_CFG_FAIL;
    if(strlen(lp_number) == 0)
    {
        strcpy(lp_number_p,"");
        return SYS_TYPE_GET_RUNNING_CFG_NO_CHANGE;
    }
    strncpy(lp_number_p, lp_number, BANNER_MAX_DATA_LENGTH);
    return SYS_TYPE_GET_RUNNING_CFG_SUCCESS;
}

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
BOOL_T CLI_BANNER_SetMUX(char *mux_p)
{
    if(mux_p == NULL)
        return FALSE;
    strncpy(mux, mux_p, BANNER_MAX_DATA_LENGTH);
    return TRUE;
}

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
BOOL_T CLI_BANNER_GetMUX(char *mux_p)
{
    if(mux_p == NULL)
        return FALSE;
    strncpy(mux_p, mux, BANNER_MAX_DATA_LENGTH);
    return TRUE;
}

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
SYS_TYPE_Get_Running_Cfg_T CLI_BANNER_GetRunningMUX(char *mux_p)
{
    if(mux_p == NULL)
        return SYS_TYPE_GET_RUNNING_CFG_FAIL;
    if(strlen(mux) == 0)
    {
        strcpy(mux_p,"");
        return SYS_TYPE_GET_RUNNING_CFG_NO_CHANGE;
    }
    strncpy(mux_p, mux, BANNER_MAX_DATA_LENGTH);
    return SYS_TYPE_GET_RUNNING_CFG_SUCCESS;
}

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
BOOL_T CLI_BANNER_SetIpLan(char *ip_lan_p)
{
    if(ip_lan_p == NULL)
        return FALSE;
    strncpy(ip_lan, ip_lan_p, BANNER_MAX_DATA_LENGTH);
    return TRUE;
}

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
BOOL_T CLI_BANNER_GetIpLan(char *ip_lan_p)
{
    if(ip_lan_p == NULL)
        return FALSE;
    strncpy(ip_lan_p, ip_lan, BANNER_MAX_DATA_LENGTH);
    return TRUE;
}

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
SYS_TYPE_Get_Running_Cfg_T CLI_BANNER_GetRunningIpLan(char *ip_lan_p)
{
    if(ip_lan_p == NULL)
        return SYS_TYPE_GET_RUNNING_CFG_FAIL;
    if(strlen(ip_lan) == 0)
    {
        strcpy(ip_lan_p,"");
        return SYS_TYPE_GET_RUNNING_CFG_NO_CHANGE;
    }
    strncpy(ip_lan_p, ip_lan, BANNER_MAX_DATA_LENGTH);
    return SYS_TYPE_GET_RUNNING_CFG_SUCCESS;
}

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
BOOL_T CLI_BANNER_SetNote(char *note_p)
{
    if(note_p == NULL)
        return FALSE;
    strncpy(note, note_p, BANNER_MAX_NOTE_DATA_LENGTH);
    return TRUE;
}

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
BOOL_T CLI_BANNER_GetNote(char *note_p)
{
    if(note_p == NULL)
        return FALSE;
    strncpy(note_p, note, BANNER_MAX_NOTE_DATA_LENGTH);
    return TRUE;
}

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
SYS_TYPE_Get_Running_Cfg_T CLI_BANNER_GetRunningNote(char *note_p)
{
    if(note_p == NULL)
        return SYS_TYPE_GET_RUNNING_CFG_FAIL;
    if(strlen(note) == 0)
    {
        strcpy(note_p,"");
        return SYS_TYPE_GET_RUNNING_CFG_NO_CHANGE;
    }
    strncpy(note_p, note, BANNER_MAX_NOTE_DATA_LENGTH);
    return SYS_TYPE_GET_RUNNING_CFG_SUCCESS;
}

#endif
