#ifndef _STKTPLG_SHOM_H
#define _STKTPLG_SHOM_H

#include "sys_type.h"
#include "l_mm.h"
#include "sysrsc_mgr.h"
#include "stktplg_om.h"
struct STKTPLG_BOARD_BoardInfo_S;/*add by fen.wang,it just for pass compiler*/

void STKTPLG_SHOM_GetShMemInfo(SYSRSC_MGR_SEGID_T *segid_p, UI32_T *seglen_p);

void STKTPLG_SHOM_InitiateSystemResources(void);

void STKTPLG_SHOM_AttachSystemResources(void);

BOOL_T STKTPLG_SHOM_GetMyUnitID(UI32_T *my_unit_id);

BOOL_T STKTPLG_SHOM_SetHgDownPortLinkState(UI32_T state);

BOOL_T STKTPLG_SHOM_SetHgUpPortLinkState(UI32_T state);

BOOL_T STKTPLG_SHOM_GetHgDownPortLinkState(UI32_T *state);

BOOL_T STKTPLG_SHOM_GetHgPortLinkState(UI32_T *up_state,UI32_T *down_state);

BOOL_T STKTPLG_SHOM_GetHgUpPortLinkState(UI32_T *state);

BOOL_T STKTPLG_SHOM_GetMaxPortNumberOnBoard(UI8_T unit, UI32_T *max_port_number);

BOOL_T STKTPLG_SHOM_IsOptionModule();
BOOL_T STKTPLG_SHOM_SetUnitBoardInfo(UI8_T unit, struct STKTPLG_BOARD_BoardInfo_S  *board_info);

void STKTPLG_SHOM_SetMyUnitID(UI32_T my_unit_id);

//BOOL_T STKTPLG_SHOM_SetStktplgInfo(STKTPLG_OM_Ctrl_Info_T* ctrl_info,STK_UNIT_CFG_T *stk_unit_cfg,struct STKTPLG_BOARD_BoardInfo_S *units_board_info_p);

//BOOL_T STKTPLG_SHOM_GetCtrlInfo(STKTPLG_OM_Ctrl_Info_T* ctrl_info);

BOOL_T STKTPLG_SHOM_GetStackingPortInfo(UI32_T *state,UI32_T *uplinkport,UI32_T *downlinkport);

BOOL_T STKTPLG_SHOM_UnitExist(UI32_T unit_id);

BOOL_T STKTPLG_SHOM_SetStackingPortInfo(UI32_T state,UI32_T uplinkport,UI32_T downlinkport);

/* FUNCTION NAME : STKTPLG_SHOM_GetConfigTopologyInfoDoneStatus
 * PURPOSE: To get the status of configuring topology info.
 * INPUT:   None.
 * OUTPUT:  None.
 * RETUEN:  TRUE   -- STKTPLG had configured topology
 *                    info to lower layer.
 *          FALSE  -- STKTPLG had not configured
 *                    topology info to lower layer.
 * NOTES:
 *          Tables in ASIC need to be initialized after
 *          the module id had been configured through
 *          the operation "Configure Topology Info" in
 *          STKTPLG. Thus in SWDRV_EnterTransitionMode(),
 *          it needs to poll this status and call
 *          DEV_PMGR_ResetOnDemand() to re-initialize ASIC
 *          after "Configure Topology Info" is finished.
 */
BOOL_T STKTPLG_SHOM_GetConfigTopologyInfoDoneStatus(void);
#endif
