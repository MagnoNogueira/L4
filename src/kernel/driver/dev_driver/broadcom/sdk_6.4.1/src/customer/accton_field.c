/*
 * File:	accton_field.c
 * Purpose:     Accton-specific APIs
 *
 */

#include <assert.h>

#include <soc/mem.h>
#include <soc/drv.h>

//#include <bcm_int/esw/field.h>
//#include <bcm_int/esw/firebolt.h>
#include <bcm_int/control.h>

#include <customer/accton_field.h>

#ifdef BCM_FIELD_SUPPORT

/***************************************************************************
 * 	Accton-specific APIs for MAU MIBs Configuration and Status Retrieving  *  	        * 
 ***************************************************************************/

/*
 * Function: accton_bcm_field_group_create_mode_slice_id
 *
 * Purpose:
 *     Create a group with both a mode (single, double, etc.) and a Group ID.
 *
 * Parameters:
 *     unit - BCM device number.
 *     qset - Field qualifier set
 *     pri  - Priority within allowable range (see bcm_field_status_get),
 *            or BCM_FIELD_GROUP_PRIO_ANY to automatically assign a
 *            priority; each priority value may be used only once
 *    mode  - Group mode (single, double, triple or Auto-wide)
 *    group - Requested field group ID
 *    sset   - group slice index bitmap.
 * Returns:
 *     BCM_E_INIT      - BCM unit not initialized
 *     BCM_E_RESOURCE  - no select codes will satisfy qualifier set
 *     BCM_E_NONE      - Success
 */
int
accton_bcm_field_group_create_mode_slice_id(
	int	unit,
	bcm_field_qset_t	qset,
	int	pri,
	bcm_field_group_mode_t	mode,
	bcm_field_group_t	group,
	bcm_field_group_sbmp_t sset)
{
	int	r_rv;

	if (!BCM_UNIT_VALID(unit)) {
		r_rv = BCM_E_UNIT;
	} else {
#if 1
        r_rv = bcm_field_group_create_id(unit,qset,pri,group);
#elif 2
        r_rv = bcm_field_group_create_mode_id(unit,qset,pri,mode,group);
#else
        r_rv = BCM_E_UNAVAIL;

        if (SOC_IS_ESW(unit))
        {
            r_rv = accton_bcm_esw_field_group_create_mode_slice_id(unit,qset,pri,mode,group,sset);
        }
#endif
	}

	return r_rv;
}

int
accton_bcm_field_entry_copy_id(
	int	unit,
	bcm_field_entry_t	src_entry,
	bcm_field_entry_t	dst_entry)
{
	int	r_rv;

	if (!BCM_UNIT_VALID(unit)) {
		r_rv = BCM_E_UNIT;
	} else {
#if 1
        r_rv = bcm_field_entry_copy_id(unit,src_entry,dst_entry);
#else
        r_rv = BCM_E_UNAVAIL;

        if (SOC_IS_ESW(unit))
        {
            r_rv = accton_bcm_esw_field_entry_copy_id(unit,src_entry,dst_entry);
        }
#endif
	}
	return r_rv;
}

int accton_bcm_field_meter_control_set(int unit, uint32 value)
{
    int rv = BCM_E_UNAVAIL;

#if 0
#if defined(BCM_FIREBOLT_SUPPORT) || defined(BCM_FIREBOLT2_SUPPORT) || defined(BCM_RAVEN_SUPPORT)
    if (SOC_IS_FB(unit) || SOC_IS_RAVEN(unit))
    {
        uint32 regval;

        rv = READ_FP_METER_CONTROLr(unit, &regval);
        if(BCM_FAILURE(rv))
            return (rv);

        soc_reg_field_set(unit, FP_METER_CONTROLr, &regval, PACKET_IFG_BYTESf, value);
        BCM_IF_ERROR_RETURN(WRITE_FP_METER_CONTROLr(unit, regval));
    }
#endif
#endif

    return rv;
}

#endif  /* BCM_FIELD_SUPPORT */

