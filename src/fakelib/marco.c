

#define _GEN_MIN(b)     (b * DEVRM_SHR_BITWIDE)
#define _GEN_MAX(b)     ( ((b+1) * DEVRM_SHR_BITWIDE)-1 )
#define _RANGE(x,m,M)   ( ((m)<=(x)&&(x)<=(M)) ? (x) : (0) )
#define _GETVAL(v,off)  ( _RANGE(v, _GEN_MIN(off), _GEN_MAX(off)) )

/*
    GETVAL(v, off)
    INPUT
    v           : value
    off         : byte offset, start from 0

    RETURN
    0           : input value is not in this byte
    1~bitwide   : input value
 */
#define GETVAL(v,off)   ( _GETVAL(v,off) ? _GETVAL(v,off)-(off*DEVRM_SHR_BITWIDE)+1   : 0 )

/*
    VAL2BMP(v,off)
    INPUT
    v           : value
    off         : byte offset, start from 0

    RETURN
    
    bitmap = 1 << (value % BITWIDE) if v in off byte
    
    value       bitmap
    0           1
    1           2
    2           3
    ..
    31          32
    32->0       1
    33->1       2
*/
#define VAL2BMP(v,off)  ( GETVAL(v,off)  ? (1 << (GETVAL(v,off) % DEVRM_SHR_BITWIDE) >> 1) : 0 )

#if 0
example for VAL2BMP
static FP_CONFIG_GroupInfo_T      group_info[] = {
        /* index 0 */
        {
            0,
            DEVRM_GROUP_MODE_SINGLE,
            DEVRM_STAGE_IFP,
            DEVRM_IFP_SELECTOR_0,
            1,
            {
                VAL2BMP(DEVRM_FIELD_QUALIFY_InPorts,0) | VAL2BMP(DEVRM_FIELD_QUALIFY_SrcIp,0) | VAL2BMP(DEVRM_FIELD_QUALIFY_PacketFormat,0),
                VAL2BMP(DEVRM_FIELD_QUALIFY_InPorts,1) | VAL2BMP(DEVRM_FIELD_QUALIFY_SrcIp,1) | VAL2BMP(DEVRM_FIELD_QUALIFY_PacketFormat,1),
                VAL2BMP(DEVRM_FIELD_QUALIFY_InPorts,2) | VAL2BMP(DEVRM_FIELD_QUALIFY_SrcIp,2) | VAL2BMP(DEVRM_FIELD_QUALIFY_PacketFormat,2),
                VAL2BMP(DEVRM_FIELD_QUALIFY_InPorts,3) | VAL2BMP(DEVRM_FIELD_QUALIFY_SrcIp,3) | VAL2BMP(DEVRM_FIELD_QUALIFY_PacketFormat,3),
            },
            {
                {0, 0, 0, FALSE},
                {0, 0, 0, FALSE},
            },
            FALSE
        },
#endif
