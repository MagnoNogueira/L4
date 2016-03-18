/*
 * Module Name: sys_dflt_xor.h
 *
 * PURPOSE: Define the exclusion rules.
 *
 * NOTES:
 *
 * History:
 *          Date        Modifier        Reason
 *          2005/08/04  Justin Jan      Create this file
 *
 * Copyright(C)      Accton Corporation, 2005
 */

#ifndef SYS_DFLT_XOR_H
#define SYS_DFLT_XOR_H



/* INCLUDE FILE DECLARATIONS
 */
#include "sys_type.h"



/*
 * CONSTANT SYMBOL DEFINITIONS
 */
#define SYS_DFLT_XOR_LACP_PORT_EXCLUSION        BIT_0
#define SYS_DFLT_XOR_TRUNK_MEMBER_EXCLUSION     BIT_1
#define SYS_DFLT_XOR_PRIVATE_PORT_EXCLUSION     BIT_2
#define SYS_DFLT_XOR_MIRROR_TO_PORT_EXCLUSION   BIT_3
#define SYS_DFLT_XOR_MIRRORED_PORT_EXCLUSION    BIT_4
#define SYS_DFLT_XOR_SECURITY_PORT_EXCLUSION    BIT_5

/*
 * Note: YES means the rule contain the exclusion item.
 *       NO  means the rule doesn't need to check the exclusion item.
 * Then:
 *       ENABLE_PORT_LACP_RULE      is private port exclusion,
 *                                     mirror to port exclusion,
 *                                     mirrored port exclusion and
 *                                     security port exclusion.
 *
 *       JOIN_TO_TRUNK_PORT_RULE    is private port exclusion,
 *                                     mirror to port exclusion,
 *                                     mirrored port exclusion and
 *                                     security port exclusion.
 *
 * ---------------------------------------------------------------------------------------------------------------------------
 * |     EXCLUSION ITEM ->      |  PORT LACP  | TRUNK MEMBER | PRIVATE PORT | MIRROR TO PORT | MIRRORED PORT | SECURITY PORT |
 * ---------------------------------------------------------------------------------------------------------------------------
 * | ENABLE_PORT_LACP_RULE      |     NO      |     NO       |     NO       |     NO         |     NO        |     YES       |
 * ---------------------------------------------------------------------------------------------------------------------------
 * | JOIN_TO_TRUNK_PORT_RULE    |     NO      |     NO       |     NO       |     NO         |     NO        |     YES       |
 * ---------------------------------------------------------------------------------------------------------------------------
 * | SET_TO_PRIVATE_PORT_RULE   |     NO      |     NO       |     NO       |     NO         |     NO        |     NO        |
 * ---------------------------------------------------------------------------------------------------------------------------
 * | SET_TO_MIRROR_TO_PORT_RULE |     NO      |     NO       |     NO       |     NO         |     NO        |     NO        |
 * ---------------------------------------------------------------------------------------------------------------------------
 * | SET_TO_MIRRORED_PORT_RULE  |     NO      |     NO       |     NO       |     NO         |     NO        |     NO        |
 * ---------------------------------------------------------------------------------------------------------------------------
 * | SET_TO_SECURITY_PORT_RULE  |     YES     |     YES      |     NO       |     NO         |     NO        |     NO        |
 * ---------------------------------------------------------------------------------------------------------------------------
 *
 */
#define SYS_DFLT_XOR_ENABLE_LACP_RULE           SYS_DFLT_XOR_SECURITY_PORT_EXCLUSION

#define SYS_DFLT_XOR_JOIN_TO_TRUNK_RULE         SYS_DFLT_XOR_SECURITY_PORT_EXCLUSION

#define SYS_DFLT_XOR_SET_TO_PRIVATE_PORT_RULE   0

#define SYS_DFLT_XOR_SET_TO_MIRROR_TO_PORT_RULE 0

#define SYS_DFLT_XOR_SET_TO_MIRRORED_PORT_RULE  0

#define SYS_DFLT_XOR_SET_TO_SECURITY_PORT_RULE  (SYS_DFLT_XOR_LACP_PORT_EXCLUSION |      \
                                                 SYS_DFLT_XOR_TRUNK_MEMBER_EXCLUSION)


#endif /* End of SYS_DFLT_XOR_H */
