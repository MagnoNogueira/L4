/* FUNCTION NAME: FP_CONFIG.H
* PURPOSE:
*   1. Provide basic uniform interfaces for different slice distribution.
*
* NOTES:
*   {Something must be known or noticed}
*   {1. How to use these functions - Give an example.}
*   {2. Sequence of messages if applicable.}
*   {3. Any design limitation}
*   {4. Any performance limitation}
*   {5. Is it a reusable component}
*
*REASON:
*    DESCRIPTION: separate function information and group information.
*    CREATOR:   Jinfeng Chen
*    Date:      2008-10-08
*
* Copyright(C)      Accton Corporation, 2008
*/

#ifndef _FP_CONFIG_H
#define _FP_CONFIG_H

#include "sys_type.h"
#include "sys_adpt.h"

#include "rule_type.h"

#ifdef MARVELL_CPSS
    #include <marvell/dev_rm.h>
#else

#ifdef REALTEK
    #include "realtek/dev_rm.h"
#else
    #include "esw/dev_rm.h"
    #include "inc.h"
#endif /* REALTEK */

#define FP_CONFIG_MAX_DESC_STR_LEN                  48

#ifdef REALTEK
typedef struct
{
    UI32_T                              template_key;
    char                                template_str[FP_CONFIG_MAX_DESC_STR_LEN+1];
} FP_CONFIG_TemplateKeyDescriptor_T;

typedef struct
{
    UI32_T                              count;
    FP_CONFIG_TemplateKeyDescriptor_T   *tk;
} FP_CONFIG_TemplateVector_T;

#else
typedef struct
{
    DEVRM_FIELD_QUALIFY_T               fq;
    char                                fq_str[FP_CONFIG_MAX_DESC_STR_LEN+1];
} FP_CONFIG_FieldQualifyDescriptor_T;

typedef struct
{
    UI32_T                              count;
    FP_CONFIG_FieldQualifyDescriptor_T  *fq;
} FP_CONFIG_FieldQualifyVector_T;

#endif /* REALTEK */

typedef struct
{
    UI32_T                              chunk;
    UI32_T                              flags;
} FP_CONFIG_FieldUDFDescriptor_T;

typedef struct
{
    UI32_T                              count;
    FP_CONFIG_FieldUDFDescriptor_T      *udf;
} FP_CONFIG_FieldUDFVector_T;

typedef struct
{
    RULE_TYPE_FunctionType_T            func_type;
    int                                 rule_pri;
    int                                 rule_quota;
    char                                func_type_str[FP_CONFIG_MAX_DESC_STR_LEN+1];
    char                                rule_pri_str[FP_CONFIG_MAX_DESC_STR_LEN+1];
} FP_CONFIG_FunctionDescriptor_T;

typedef struct
{
    UI32_T                              count;
    FP_CONFIG_FunctionDescriptor_T      *fd;
} FP_CONFIG_FunctionVector_T;

typedef struct
{
    UI32_T                              group_id;
    UI32_T                              group_mode;
    UI32_T                              selector_stage;
    UI32_T                              first_selector_num;
    
#ifdef REALTEK
    FP_CONFIG_TemplateVector_T          tk_vector;
#else
    FP_CONFIG_FieldQualifyVector_T      fq_vector;
#endif

    FP_CONFIG_FieldUDFVector_T          udf_vector;
    FP_CONFIG_FunctionVector_T          func_vector;

    char                                group_mode_str[FP_CONFIG_MAX_DESC_STR_LEN+1];
    char                                selector_stage_str[FP_CONFIG_MAX_DESC_STR_LEN+1];
    char                                first_selector_num_str[FP_CONFIG_MAX_DESC_STR_LEN+1];
} FP_CONFIG_STRUCT_T;

typedef struct FP_CONFIG_FunctionInfo_S
{
    RULE_TYPE_FunctionType_T            function_type;
    UI32_T                              group_id;           /* array index */
    int                                 rule_pri;           /* fun priority:low, middle, high */
    UI32_T                              rule_quota;
} FP_CONFIG_FunctionInfo_T;

typedef struct FP_CONFIG_GroupInfo_S
{
    UI32_T                              group_id;           /* group id, use as group priority on create too */
    UI32_T                              group_mode;         /* single wide, double wide */
    UI32_T                              selector_stage;     /* VFP, IFP, EFP */
    UI32_T                              selector_bitmap;    /* a group may include more than one slice. */
    UI32_T                              selector_count;     /* number of selectors within the group. */

#ifdef REALTEK
    UI32_T                              template_num;
    UI32_T                              template_key[4];
#else 
    UI32_T                              w[DEVRM_SHR_BITDCLSIZE(DEVRM_FIELD_QUALIFY_Count)];/* field qualify bitmap */
#endif
    DEVRM_UDF_T                         udf[DEVRM_FIELD_USER_QUALIFY_Count];
    BOOL_T                              is_created;
} FP_CONFIG_GroupInfo_T;

/*------------------------------------------------------------------------------
 * FUNCTION NAME: FP_CONFIG_GetShMemDataSize
 *------------------------------------------------------------------------------
 * PURPOSE: Get shared memory size of fp_config
 * INPUT:   None
 * OUTPUT:  None
 * RETUEN:  None
 * NOTES:
 *------------------------------------------------------------------------------
 */
UI32_T FP_CONFIG_GetShMemDataSize();


/*------------------------------------------------------------------------------
 * FUNCTION NAME: FP_CONFIG_AttachSystemResources
 *------------------------------------------------------------------------------
 * PURPOSE: Attach system resource for FP_CONFIG in the context of the calling
 *          process.
 * INPUT:   None
 * OUTPUT:  None
 * RETUEN:  None
 * NOTES:
 *------------------------------------------------------------------------------
 */
void FP_CONFIG_AttachSystemResources(void *shmem_start_p);


/*------------------------------------------------------------------------------
 * FUNCTION NAME: FP_CONFIG_Init
 *------------------------------------------------------------------------------
 * PURPOSE: Set OM of FP_CONFIG to default value.
 * INPUT:   None
 * OUTPUT:  None
 * RETUEN:  None
 * NOTES:
 *------------------------------------------------------------------------------
 */
void FP_CONFIG_Init();


/*------------------------------------------------------------------------------
 * FUNCTION NAME: FP_CONFIG_Dump
 *------------------------------------------------------------------------------
 * PURPOSE: Dump fp config information
 * INPUT:   None
 * OUTPUT:  None
 * RETUEN:  None
 * NOTES:
 *------------------------------------------------------------------------------
 */
void FP_CONFIG_Dump();


UI32_T FP_CONFIG_NumberOfFunctionType();


/*------------------------------------------------------------------------------
 * FUNCTION NAME: FP_CONFIG_GetFunctionInfo
 *------------------------------------------------------------------------------
 * PURPOSE: Get function information by array index
 * INPUT:   index   - array index
 * OUTPUT:  None
 * RETUEN:  function infomation pointer
 * NOTES:
 *------------------------------------------------------------------------------
 */
FP_CONFIG_FunctionInfo_T * FP_CONFIG_GetFunctionInfo(UI32_T index);


/*------------------------------------------------------------------------------
 * FUNCTION NAME: FP_CONFIG_get_function_info_by_type
 *------------------------------------------------------------------------------
 * PURPOSE: Get function information by type
 * INPUT:   fun_type   - function type
 * OUTPUT:  None
 * RETUEN:  function infomation pointer
 * NOTES:
 *------------------------------------------------------------------------------
 */
FP_CONFIG_FunctionInfo_T * FP_CONFIG_get_function_info_by_type(RULE_TYPE_FunctionType_T fun_type);


UI32_T FP_CONFIG_NumberOfGroup();
FP_CONFIG_GroupInfo_T * FP_CONFIG_get_group_info_by_id(UI32_T group_id);

UI32_T FP_CONFIG_GetConfigSize();

FP_CONFIG_STRUCT_T * FP_CONFIG_GetConfig(UI32_T idx);

#endif /* MARVELL_CPSS */
#endif  /* _FP_CONFIG_H */

