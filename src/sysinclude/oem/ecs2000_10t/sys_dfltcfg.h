#ifndef  SYS_DFLTCFG_H
#define  SYS_DFLTCFG_H


/*----------------------------------------------------------------------------
 * Package: SYS_DFLTCFG.H                                                        
 * Purpose: This package defines the system factory default configuration
 *          setting.   When there is no any system default configuration file
 *          in NVRAM read by File System.  CLI will read this text file to
 *          save in NVRAM as default cfg file.
 * Note: The naming constant defined in this package shall be reused	  
 *        by all the BNBU L2/L3 switch projects .
 *  History
 *
 *   Jason Hsue     01/17/2001      new created
 *          Suggested by Charles Cheng
 * ------------------------------------------------------------------------
 * Copyright(C)							  	ACCTON Technology Corp. , 2002      
 * ------------------------------------------------------------------------
 */

/* INCLUDE FILE DECLARATIONS
 */
#include "sys_type.h"



char    *sys_dfltcfg =
{
"!\n"
"!\n"
"username admin access-level 15\n"
"username admin password 7 21232f297a57a5a743894a0e4a801fc3\n"
"!\n"
"username guest access-level 0\n"
"username guest password 7 084e0343a0486ff05530df6c705c8bb4\n"
"!\n"
"enable password level 15 7 1b3231655cebb7a1f783eddf27d254ca\n"
"!\n"
"snmp-server community public ro\n"
"snmp-server community private rw\n"
"!\n"
"vlan database\n"
" vlan 1 name DefaultVlan media ethernet state active\n"
"!\n"
"interface vlan 1\n"
" ip address dhcp\n"
"!\n"
"line console\n"
"!\n"
"line vty\n"
"!\n"
"!\n"
"end\n"
"\n"
"\n"
};

#endif
