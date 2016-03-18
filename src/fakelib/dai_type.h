#ifndef __DAI_TYPE_H__
#define __DAI_TYPE_H__

#include "sys_type.h"

#define DAI_TYPE_NEED_NO_ADDITIONAL_VALIDATE                0x00000000
#define DAI_TYPE_ADDITIONAL_VALIDATE_DSTMAC                 0x00000001
#define DAI_TYPE_ADDITIONAL_VALIDATE_IP                     0x00000002
#define DAI_TYPE_ADDITIONAL_VALIDATE_IP_ALLOW_ZEROS         0x00000010
#define DAI_TYPE_ADDITIONAL_VALIDATE_SRCMAC                 0x00000004
#define DAI_TYPE_LOG_FUN_NO                                 1
#define DAI_TYPE_LOG_ERR_NO                                 1
#define DAI_TYPE_SN_LAN                                     1
#define DAI_TYPE_ARP_FORMAT                                 0x0806
#define DAI_TYPE_ARP_OP_REQUEST                             1
#define DAI_TYPE_ARP_OP_REPLY                               2

typedef struct DAI_TYPE_ArpPktFormat_S
{
    UI16_T  hardwareType;
    UI16_T  protocolType;
    UI8_T   hardwareLen;
    UI8_T   protocolLen;
    UI16_T  opcode;
    UI8_T   srcMacAddr[6];
    UI32_T  srcIp;
    UI8_T   dstMacAddr[6];
    UI32_T  dstIp;
}__attribute__((packed, aligned(1))) DAI_TYPE_ArpPktFormat_T;

#endif /* __DAI_TYPE_H__ */
