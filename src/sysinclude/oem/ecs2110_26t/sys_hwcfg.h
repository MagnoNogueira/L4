#ifndef SYS_HWCFG_H
#define SYS_HWCFG_H

/*----------------------------------------------------------------------------
 * Package: SYS_HWCFG.H                                                        
 * Purpose: This package defines the hardware adaptation value for the project. 
 * Note: The naming constant defined in this package shall be reused by   
 *        all the BNBU L2/L3 switch projects .
 *  History
 *
 *   ryan     07/16/2001      new created
 *   jjyoung  10/21/2002      modified for Alps(XGS) project
 * 
 * ------------------------------------------------------------------------
 * Copyright(C)							  	ACCTON Technology Corp. , 2001      
 * ------------------------------------------------------------------------
 */


/* INCLUDE FILE DECLARATIONS
 */
#include "sys_type.h"

#define SYS_HWCFG_CPU_82XX

#define  SYS_HWCFG_RTK_CHIP_RTL8380   0x01
#define  SYS_HWCFG_RTK_CHIP_RTL8390   0x02
#define  SYS_HWCFG_RTK_CHIP_FAMILY    SYS_HWCFG_RTK_CHIP_RTL8390



#define LED_PROGRAM_RAM_SIZE  0x100

#define SYS_HWCFG_MAX_METER_ENTRY_NUM                 256
#define SYS_HWCFG_MAX_PKT_BASED_STATISTIC_ENTRY_NUM   256
#define SYS_HWCFG_MAX_BYTE_BASED_STATISTIC_ENTRY_NUM  512

/* Flash device type :
 *
 * SYS_HWCFG_FLASH_MEM_INTEL_28F160B3   : for Intel 28F160B3
 * SYS_HWCFG_FLASH_MEM_INTEL_28F320C3BA : for Intel 28F320C3BA
 * SYS_HWCFG_FLASH_MEM_INTEL_28F320J3A  : for Intel 28F320J3A
 * SYS_HWCFG_FLASH_MEM_INTEL_28F640J3A  : for Intel 28F640J3
 */
//#define SYS_HWCFG_FLASH_MEM_INTEL_28F128J3A 
/* Two of 28F640J3A (64Mbit, 8MByte) Intel Strata Flash memory chips */

/* Data width for a single flash chip */
//#define SYS_HWCFG_FLASH_MEM_DATA_ACCESS_8BIT

/* Data width for a flash block */
//#define SYS_HWCFG_FLASH_MEM_DATA_BANDWIDTH_8BIT

/* How many RCS we use for flash, and the extention start address of flash */
//#define SYS_HWCFG_FLASH_RCS_NUMBER                  3
//#define SYS_HWCFG_FLASH_MEM_EXT_ADDR                0xFF000000  /* 2nd flash chip (NOR) */
//#define SYS_HWCFG_FLASH_MEM_EXT_ADDR_1              0xB0000000  /* 3rd flash chip (NAND) */

//#define SYS_HWCFG_STK_STATUS              0xEA0a0007

/* NAMING CONSTANT DECLARATIONS 
 */

/* The constants ROM_TEXT_ADRS, ROM_SIZE, RAM_HIGH_ADRS, and RAM_LOW_ADRS
 * are used in config.h and Makefile.
 * All definitions for these constants must be identical.
 */
 
/* If you update these constants, you must update the WPJ of BSP */ 
//#define SYS_HWCFG_ROM_BASE_ADRS           0xfff00000      /* base address of ROM */
//#define SYS_HWCFG_ROM_TEXT_ADRS           (SYS_HWCFG_ROM_BASE_ADRS+0x0100) /* with PC & SP */
//#define SYS_HWCFG_ROM_WARM_ADRS           (SYS_HWCFG_ROM_TEXT_ADRS+0x0004) /* warm reboot entry */
//#define SYS_HWCFG_ROM_SIZE                0x00008000      /* 32KB ROM space */
//#define SYS_HWCFG_RAM_LOW_ADRS            0x00010000      /* RAM address for vxWorks */
//#define SYS_HWCFG_RAM_HIGH_ADRS           0x07B00000      /* RAM address for bootrom */


/* FLASH Memory Mapping
 * Note: The following information will be stored in the 8M bytes flash memory:
 *       1. Loader image                    - 32K bytes
 *       2. Agent board information         - 256 bytes
 *       3. main board information          - 256 bytes
 *       4. flash disk system               - (8M - 32K - 256 - 256) bytes
 *          - system confguration data file    
 *          - PROM image file in compressed format                    
 *          - Run Time (Agent) image file in compressed format
 *          - System log file
 *          - Error log file        
 */

//#define SYS_HWCFG_FLASH_MEM_BASE_ADDR               0xFE000000  /* 1st flash chip (NOR) */
//#define SYS_HWCFG_FLASH_MEM_SIZE                    (96 * SYS_TYPE_1M_BYTES)
//#define SYS_HWCFG_FLASH_MAX_RCS0_CONFIG_SIZE        (16 * SYS_TYPE_1M_BYTES)

/* LOADER Image Memory Mapping
 * Note: Reserve 32K bytes flash for Loader image. 
 */
//#define SYS_HWCFG_LOADER_START_ADDR	                0xFFF00100
//#define SYS_HWCFG_LOADER_CODE_SIZE                  (64 * SYS_TYPE_1K_BYTES)

/* Loader Parameters, needed by loader */
//#define SYS_HWCFG_LOADER_DOWNLOAD_BUFFER_ADDRESS    0x07800000
//#define SYS_HWCFG_LOADER_DOWNLOAD_BUFFER_SIZE       0x00600000
//#define	SYS_HWCFG_LOADER_APPLICATION_START_ADDRESS  0x00010000

//#define BSP_DownloadBuffer                          SYS_HWCFG_LOADER_DOWNLOAD_BUFFER_ADDRESS
//#define BSP_DownloadBufferSize                      SYS_HWCFG_LOADER_DOWNLOAD_BUFFER_SIZE
//#define BSP_ApplicationStartAddress                 SYS_HWCFG_LOADER_APPLICATION_START_ADDRESS


/* Agent/Main Board Info Memory Mapping
 * Note: 1. Reserve 256 bytes for agent board information.
 *       2. Reserve 256 bytes for main board information.
 */
//#define SYS_HWCFG_AGENT_BOARD_INFO_START_ADDR       0xFE020000  /* Block 1: save H/W info */
//#define SYS_HWCFG_AGENT_BOARD_INFO_BUF_SIZE         SYS_TYPE_256_BYTES  

//#define SYS_HWCFG_MAIN_BOARD_INFO_START_ADDR        (SYS_HWCFG_AGENT_BOARD_INFO_START_ADDR + SYS_HWCFG_AGENT_BOARD_INFO_BUF_SIZE)                                                   
//#define SYS_HWCFG_MAIN_BOARD_INFO_BUF_SIZE          SYS_TYPE_256_BYTES  

/* Start Address of Flash Reserve Blocks
 * Note: 1. Support max 8 reserve blocks.
 *         2. Start address 0x00000000 means that reserve block is not used.
 *********************************************************
 ******** jerry.du modify 20080605, Flash Map Patition Layout *********
 *********************************************************
 */
/* Need to follow loader's design, thomas */
#define SYS_HWCFG_FLASH_PARTITION_NUM               9
#define SYS_HWCFG_FLASH_TYPE_HRCW                   1
#define SYS_HWCFG_FLASH_TYPE_KERNEL                 2
#define SYS_HWCFG_FLASH_TYPE_ROOTFS                 3
#define SYS_HWCFG_FLASH_TYPE_USERDATA               4
#define SYS_HWCFG_FLASH_TYPE_LOADER                 5
#define SYS_HWCFG_FLASH_TYPE_LOADERDATA             6
#define SYS_HWCFG_FLASH_TYPE_HWINFO                 7
#define SYS_HWCFG_FLASH_TYPE_SYSINFO                8
#define SYS_HWCFG_FLASH_TYPE_SYSCONFIG              9
#define SYS_HWCFG_FLASH_RESERVE_BLOCK0_START_ADDR   0xFE000000  /* Block 0 (0xFE00000~0xFE01FFFF): save CPU config word */
#define SYS_HWCFG_FLASH_RESERVE_BLOCK1_START_ADDR   0xFE020000
#define SYS_HWCFG_FLASH_RESERVE_BLOCK2_START_ADDR   0xFE300000
#define SYS_HWCFG_FLASH_RESERVE_BLOCK3_START_ADDR   0xFF100000
#define SYS_HWCFG_FLASH_RESERVE_BLOCK4_START_ADDR   0xFFF00000
#define SYS_HWCFG_FLASH_RESERVE_BLOCK5_START_ADDR   0xFFF80000
#define SYS_HWCFG_FLASH_RESERVE_BLOCK6_START_ADDR   0xFFFA0000
#define SYS_HWCFG_FLASH_RESERVE_BLOCK7_START_ADDR   0xFFFC0000
#define SYS_HWCFG_FLASH_RESERVE_BLOCK8_START_ADDR   0xFFFE0000
#define SYS_HWCFG_FLASH_RESERVE_BLOCK0_SIZE   0x00020000
#define SYS_HWCFG_FLASH_RESERVE_BLOCK1_SIZE   0x002E0000
#define SYS_HWCFG_FLASH_RESERVE_BLOCK2_SIZE   0x00E00000
#define SYS_HWCFG_FLASH_RESERVE_BLOCK3_SIZE   0x00E00000
#define SYS_HWCFG_FLASH_RESERVE_BLOCK4_SIZE   0x00080000
#define SYS_HWCFG_FLASH_RESERVE_BLOCK5_SIZE   0x00020000
#define SYS_HWCFG_FLASH_RESERVE_BLOCK6_SIZE   0x00020000
#define SYS_HWCFG_FLASH_RESERVE_BLOCK7_SIZE   0x00020000
#define SYS_HWCFG_FLASH_RESERVE_BLOCK8_SIZE   0x00020000

/* PCI device
 */
#define SYS_HWCFG_SUPPORT_PCI                       TRUE

/* Flash Disk Memory Mapping
 */
//#define SYS_HWCFG_FLASH_DISK_START_ADDR	            (SYS_HWCFG_MAIN_BOARD_INFO_START_ADDR + SYS_HWCFG_MAIN_BOARD_INFO_BUF_SIZE)
//#define SYS_HWCFG_FLASH_DISK_SIZE                   (SYS_HWCFG_FLASH_MEM_BASE_ADDR + SYS_HWCFG_FLASH_MEM_SIZE - SYS_HWCFG_FLASH_DISK_START_ADDR)

/* DRAM Memory Mapping and Clock Setting
 */
#define SYS_HWCFG_DRAM_BASE_ADDR                    0x00000000
#define SYS_HWCFG_DRAM_SIZE                         (256 * SYS_TYPE_1M_BYTES) /* Need to change to 256 later, thomas */
#define SYS_HWCFG_USER_RESERVED_MEM_SIZE            (4 * SYS_TYPE_1M_BYTES) /* For UC only */

/* This setting value will impact the UART baud rate as well  */

/* Define the Decremental Clock Rate.
   Note:  If No RTC is supported, this value must be filled in.
 */
/* This setting value will impact the UART baud rate as well  */
//#define SYS_HWCFG_INPUT_CLOCK_FREQ                  100000000
//#define SYS_HWCFG_DRAM_CLOCK_FREQ                   100000000
//#define SYS_HWCFG_MAIN_CLOCK_FREQ                   (SYS_HWCFG_INPUT_CLOCK_FREQ*4)
//#define SYS_HWCFG_BRG_CLOCK_FREQ                    (SYS_HWCFG_MAIN_CLOCK_FREQ/4)

/* Define the Decremental Clock Rate.
 * Note:  If No RTC is supported, this value must be filled in.
 */
//#define SYS_HWCFG_DEC_CLOCK_FREQ                    (SYS_HWCFG_INPUT_CLOCK_FREQ/4)
#if 0
#define SYS_HWCFG_DRAM_CLOCK_FREQ                   133000000
#define SYS_HWCFG_DEC_CLOCK_FREQ                    33000000
#endif

//#define SYS_CLK_RATE 100

#define CONSOLE_BAUD_RATE 115200
/* System Interrupt Vector Table
 */
//#define SYS_HWCFG_INTR_VECTOR_BASE_ADDR             SYS_HWCFG_DRAM_BASE_ADDR
//#define SYS_HWCFG_INTR_VECTOR_TABLE_SIZE            (12 * SYS_TYPE_1K_BYTES)

/* Unclear Memory Mapping
 * 1. Warm Boot: Stack Topology Change, Software Reset/Restart Request
 * 2. System Log
 * 3. Please make sure you know everything will use UC and allocate enough memory 
 */
#define SYS_HWCFG_UC_MEM_SIZE                       SYS_HWCFG_USER_RESERVED_MEM_SIZE  

#define	SYS_HWCFG_UC_MEM_END_ADDR                   (SYS_HWCFG_DRAM_BASE_ADDR + SYS_HWCFG_DRAM_SIZE - 1)
#define	SYS_HWCFG_UC_MEM_START_ADDR		            (SYS_HWCFG_UC_MEM_END_ADDR + 1 - SYS_HWCFG_UC_MEM_SIZE)

/* LAN Buffer Static Memory Mapping
 * Why static?  Because we want to allocate these packet buffer and be used by NIC driver.
 * But for NIC driver, it could use PCI DMA to write to these buffers directly and these
 * memory space can not be cached.  So we need to separate the buffer from the normal memory
 * area.
 */
/* Moved to SYS_ADPT.H. 5/6/2004, Charles, requested by Wuli
 */
/* 
#define SYS_HWCFG_MAX_LAN_RX_BUF_SIZE_PER_PACKET    1600
#define SYS_HWCFG_MAX_NBR_OF_LAN_PACKET_RX_BUFFER   512
#define SYS_HWCFG_LAN_BUF_SIZE                      (SYS_HWCFG_MAX_LAN_RX_BUF_SIZE_PER_PACKET * SYS_HWCFG_MAX_NBR_OF_LAN_PACKET_RX_BUFFER)
*/
#define SYS_HWCFG_LAN_BUF_START_ADDR                (SYS_HWCFG_UC_MEM_END_ADDR + 1 - SYS_HWCFG_USER_RESERVED_MEM_SIZE)

/* PROM/Run Time Image Memory Mapping/Working Area
 * NOTE: 1. The following naming constants are used to define the image header of PROM/Run Time code.
 *       2. The TFTP/XMODEM download utility shall verify the image header of downloaded PROM/Run Time code
 *       3. The BOOT LOADER shall load the PROM/Run Time code to the proper working area based on 
 *          the SYS_HWCFG_PROM_CODE_START_ADDR/SYS_HWCFG_RUN_CODE_START_ADDR defined in image header.   
 */
//#define SYS_HWCFG_PROM_CODE_START_ADDR              (SYS_HWCFG_DRAM_BASE_ADDR + SYS_HWCFG_INTR_VECTOR_TABLE_SIZE)
//#define SYS_HWCFG_RUN_CODE_START_ADDR               SYS_HWCFG_PROM_CODE_START_ADDR


/* Define LED display address
 */

/*eric */
/*modify by dohan 01/12/04 for 8-bits data width*/ 
/*RCS1 bank range 0xEA000000 ~ 0xFF7FFFFF*/
#define SYS_HWCFG_RCS1_BASE							0xEA000000
#define SYS_HWCFG_RCS1_OFFSET	                    0x01
 /*NVRAM*/
 /*NVRAM range 0xFF000000 ~ 0xFF0000FF*/
#define SYS_HWCFG_NVRAM_BASE						(SYS_HWCFG_RCS1_BASE + 0x0 * SYS_HWCFG_RCS1_OFFSET)
#define SYS_HWCFG_NVRAM_OFFSET						SYS_HWCFG_RCS1_OFFSET
 /*CPLD*/

/*CPLD, description based on Hardware Spec V0.7_0729*/
/*eric */
#define SYS_HWCFG_CPLD_BASE							(SYS_HWCFG_RCS1_BASE + 0xA0000 * SYS_HWCFG_RCS1_OFFSET)
#define SYS_HWCFG_CPLD_OFFSET						SYS_HWCFG_RCS1_OFFSET

/*CPLD range 0xFF500000 ~ 0xFF7FFFFF*/
#define SYS_HWCFG_CPLD1_BASE                        (SYS_HWCFG_RCS1_BASE + 0xD0000 * SYS_HWCFG_RCS1_OFFSET)
#define SYS_HWCFG_CPLD1_OFFSET						SYS_HWCFG_RCS1_OFFSET
 
/* Push Button (Read Only, 0xE0100000)
 * -----------------------------------------------------------------------------------------
 * |            7        |              6              |        5        |        4        |        3        |          2        |         1       |          0      |
 * |-----------|---------------|------------------------------------------------------------|
 * |Master/Slave|Stacking/Up-Link| Reserved | Reserved | Reserved |  Reserved  | Reserved | Reserved |
 * -----------------------------------------------------------------------------------------
 Master/Slave:
        1: Master
        0: Slave
 Stacking/Up-Link:
        1: Stacking
        0: Up-Link
 */
#define SYS_HWCFG_STACK_BUTTON_ADDR                 (SYS_HWCFG_CPLD_BASE + 0x00) /* 0xE0100000 */
    
/* Stacking status (Read Only, 0xE0100000)
  */
#define SYS_HWCFG_STACK_STATUS_ADDR                 (SYS_HWCFG_RCS1_BASE + 0xA0007 * SYS_HWCFG_RCS1_OFFSET) /*0xFF0A0007*/
#define SYS_HWCFG_STACK_BUTTON_MASK                  0x80    /* 10000000 */
#define SYS_HWCFG_STACK_BUTTON_PRESSED               0x80

#define SYS_HWCFG_BUTTON_STACKING_ADDR              SYS_HWCFG_STACK_BUTTON_ADDR
#define SYS_HWCFG_BUTTON_STACKING_MASK              0x40
#define SYS_HWCFG_BUTTON_STACKING_PRESSED           0x40
 
/* System Reset (Read/Write, 0xE0100001)
 * ------------------------------------------------------------------------------------------------
 * |         7       |        6        |       5         |            4            |         3       |             2               |             1              |         0       |
 * |-----------------------------|-------------|---------|---------------|-------------------------|
 * | Reserved | Reserved | Reserved |Reset Cold Start| Reserved |Reset BCM5248U|Reset BCM5482S| Reserved |
 * ------------------------------------------------------------------------------------------------
 Reset Cold Start:
        1: A low to high transition will cause a system level hardware reset when N_WDT_DIS is set to low. 
            Set this bit high to enable watchdog timer to trigger a hardware-reset when watchdog time out.
        0: Disable watchdog timer reset and hardware reset
 Reset BCM5248:
        1: BCM5248 are placed in normal operation state
        0: BCM5248 are placed in reset state
 Reset BCM5482S:
        1: BCM5482S are placed in normal operation state
        0: BCM5482S are placed in reset state
 */
//#define SYS_HWCFG_SYSTEM_RESET_ADDR                 (SYS_HWCFG_CPLD_BASE + 0x01) /* 0xE0100001 */

//#define SYS_HWCFG_RESET_COLDSTART_ADDR              SYS_HWCFG_SYSTEM_RESET_ADDR
//#define SYS_HWCFG_RESET_COLDSTART_MASK              0x10
//#define SYS_HWCFG_RESET_COLDSTART_RESET             0x10
//#define SYS_HWCFG_RESET_COLDSTART_NORMAL            0x00

//#define SYS_HWCFG_RESET_PHY_ADDR                    SYS_HWCFG_SYSTEM_RESET_ADDR
//#define SYS_HWCFG_RESET_PHY_MASK                    0x06
//#define SYS_HWCFG_RESET_PHY_RESET                   0x06
//#define SYS_HWCFG_RESET_PHY_NORMAL                  0x00

/* For watch dog use
 */
/* Watch Dog Control(Read/Write, 0xE0100002)
 * -----------------------------------------------------------------------------------
 * |        7           |          6        |          5      |         4        |         3       |        2        |         1       |         0       |
 * |-----------|----------|---------|-------------------------------------------------|
 * | /WDT_CLK | /DIS_WDT | Reserved | Reserved | Reserved | Reserved | Reserved | Reserved |
 * -----------------------------------------------------------------------------------
 /WDT_CLK:
        Watchdog timer clock. A high to low transition will kick off watchdog timer. 
        The watchdog timer expired if there is no more kicking on it more than 6 seconds. 
        This is should be keep in 0 during normal operation state. A low to high then high to low transition
        is required to kick off or extend the watchdog timer.
 /DIS_WDT:
        Watchdog timer disable, it prevents watchdog timer expired event to the NMI input of CPU. 
        It is a logic-AND to the output of the watchdog timer to gate off the timer expired event to the CPU.
        It should be note that this bit should enable after the watchdog timer has been kicked off.
        Otherwise a wrong timer expired event will be generated.
        1: Enable the watchdog timer expired output signal to the NMI input of CPU
        0: Disable the watchdog timer expired output signal to the NMI input of CPU
 */
#define SYS_HWCFG_WATCH_DOG_CONTROL_ADDRESS         0xEA0A000F
#define SYS_HWCFG_WATCH_DOG_CONTROL_DIS_WDT         0x40
#define SYS_HWCFG_WATCH_DOG_CONTROL_WDT_CLK         0x80

#define SYS_HWCFG_WATCH_DOG_PRE_ENABEL_KICK_L   0x0
#define SYS_HWCFG_WATCH_DOG_PRE_ENABEL_KICK_H  0x80
/* Clear NMI(Read Only, 0xE0100004)
 * ---------------------------------------------------------------------------------------
 * |               7               |        6       |         5       |        4        |        3        |       2        |         1        |         0       |
 * |----------------|---------------------------------------------------------------------|
 * |Watchdog Time out| Reserved | Reserved | Reserved | Reserved | Reserved | Reserved | Reserved |
 * ---------------------------------------------------------------------------------------
 Watchdog Time out:
        1: Watchdog timer expired
        0: Watchdog timer is not enabled or is progressing.
 */
#define SYS_HWCFG_NMI_CONTROL_ADDRESS             (SYS_HWCFG_CPLD_BASE + 0x04) /* 0xE0100004 */

/* I2C Interface Select(Read/Write, 0xE0100003)
 * --------------------------------------------------------------------------
 * |         7       |        6        |         5       |        4        |          3      |      2      |      1     |      0      |
 * |-------------------------------------------------------------------------|
 * | Reserved | Reserved | Reserved | Reserved | Reserved |    ID2    |    ID1    |    ID0    |
 * --------------------------------------------------------------------------
 ID Select:
        000: GBIC Port25
        001: GBIC Port26
        010: GBIC Port27
        011: GBIC Port28
 */
#define SYS_HWCFG_I2C_SELECT_ADDR                   (SYS_HWCFG_CPLD_BASE + 0x03) /* 0xE0100003 */
#define SYS_HWCFG_I2C_SELECT_MASK                   0x07
 
/* System LED 0 (Read/Write, 0xE0100007)
 * ----------------------------------------------------------------------------------------
 * |         7      |          6      |       5         |        4        |        3        |       2        |               1           |               0         |
 * |---------|--------|---------|---------|-------------------|--------------|-------------|
 * |   RPU_G  |   RPU_A  |   PWR_G  |   PWR_A  | Reserved | Reserved |STACK_LINK_G|STACK_LINK_A|
 * ----------------------------------------------------------------------------------------
 RPU_G, RPU_A:
        00: Off
        10: Green
        01: Amber
        11: Off
 PWR_G, PWR_A:
        00: Off
        10: Green
        01: Amber
        11: Off
 STACK_LINK_G, STACK_LINK_A:
        00: Off
        10: Green
        01: Amber
        11: Off
 */
#define SYS_HWCFG_SYSTEM_LED_0_ADDR                 (SYS_HWCFG_RCS1_BASE + 0xA0002 * SYS_HWCFG_RCS1_OFFSET) /*0xEA0A0002*/

/* PowerLED */
#define SYS_HWCFG_SYSTEM_PWR_LED_ADDR               SYS_HWCFG_SYSTEM_LED_0_ADDR
#define SYS_HWCFG_SYSTEM_PWR_LED_MASK               0x30
#define SYS_HWCFG_SYSTEM_PWR_LED_SHIFT              4
#define SYS_HWCFG_SYSTEM_PWR_LED_GREEN              0x10
#define SYS_HWCFG_SYSTEM_PWR_LED_AMBER              0x20
#define SYS_HWCFG_SYSTEM_PWR_LED_OFF                0x30

/* RPU LED */
#define SYS_HWCFG_SYSTEM_RPU_LED_ADDR               SYS_HWCFG_SYSTEM_LED_0_ADDR
#define SYS_HWCFG_SYSTEM_RPU_LED_MASK               0xc0
#define SYS_HWCFG_SYSTEM_RPU_LED_SHIFT              6
#define SYS_HWCFG_SYSTEM_RPU_LED_GREEN              0x40
#define SYS_HWCFG_SYSTEM_RPU_LED_AMBER              0x80
#define SYS_HWCFG_SYSTEM_RPU_LED_OFF                0xc0

/* Stacking link LED */
#define SYS_HWCFG_SYSTEM_STACK_LINK_LED_ADDR        SYS_HWCFG_SYSTEM_LED_0_ADDR
#define SYS_HWCFG_SYSTEM_STACK_LINK_LED_MASK        0x03
#define SYS_HWCFG_SYSTEM_STACK_LINK_LED_SHIFT       0
#define SYS_HWCFG_SYSTEM_STACK_LINK_OK              0x01
#define SYS_HWCFG_SYSTEM_STACK_LINK_DOWN_OR_NOSTACK 0x02
#define SYS_HWCFG_SYSTEM_STACK_LINK_OFF             0x03

/* System LED 1 (Read/Write, 0xEA0A0003)
 * -----------------------------------------------------------------------------------
 * |         7       |        6       |        5            |         4         |        3        |        2        |       1         |       0         |
 * |---------|--------|-----------|-----------|--------------------------------------|
 * |  DIAG_G  |  DIAG_A  | STCK_MST | STCK_SLV | Reserved | Reserved | Reserved | Reserved |
 * ----------------------------------------------------------------------------------------
 DIAG_G, DIAG_A:
        00: Off
        01: Green light, System Diagnostic test OK
        10: Amber light, System Diagnostic test failed
        11: Off
 STCK_MST, STCK_SLV:
        00: Off
        01: Green light, Stack operation State(Master UNIT)
        10: Amber light, Stack operation State(Slave UNIT)
        11: Off, Other units in the stacking chain during stack ready
 */
#define SYS_HWCFG_SYSTEM_LED_1_ADDR                  (SYS_HWCFG_RCS1_BASE + 0xA0003 * SYS_HWCFG_RCS1_OFFSET) /*0xEA0A0003*/

/* Diag LED */
#define SYS_HWCFG_SYSTEM_DIAG_LED_ADDR              SYS_HWCFG_SYSTEM_LED_1_ADDR
#define SYS_HWCFG_SYSTEM_DIAG_LED_MASK              0xC0
#define SYS_HWCFG_SYSTEM_DIAG_SHIFT                 6
#define SYS_HWCFG_SYSTEM_DIAG_OK                    0x40
#define SYS_HWCFG_SYSTEM_DIAG_FAIL                  0x80
#define SYS_HWCFG_SYSTEM_DIAG_OFF                   0xC0

/* Master/Slave LED */
#define SYS_HWCFG_SYSTEM_BASE_LED_ADDR              SYS_HWCFG_SYSTEM_LED_1_ADDR
#define SYS_HWCFG_SYSTEM_BASE_LED_MASK              0x30
#define SYS_HWCFG_SYSTEM_BASE_LED_SHIFT             4
#define SYS_HWCFG_SYSTEM_BASE_MASTER_UNIT           0x10
#define SYS_HWCFG_SYSTEM_BASE_SECOND_UNIT           0x20
#define SYS_HWCFG_SYSTEM_BASE_OTHER_UNIT            0x30

/*7 segment LED  (Read/Write, 0xE0100009)
*/
#define SYS_HWCFG_7_SEGMENT_LED_OFFSET              0x4
#define SYS_HWCFG_7_SEGMENT_LED_ADDR                SYS_HWCFG_CPLD_BASE + SYS_HWCFG_7_SEGMENT_LED_OFFSET  /*0xEA0A0004*/

#define SYS_HWCFG_7_SEGMENT_LED_UP                  0xfe
#define SYS_HWCFG_7_SEGMENT_LED_RIGHT_UP            0xfd
#define SYS_HWCFG_7_SEGMENT_LED_RIGHT_DOWN          0xfb
#define SYS_HWCFG_7_SEGMENT_LED_DOWN                0xf7
#define SYS_HWCFG_7_SEGMENT_LED_LEFT_DOWN           0xef
#define SYS_HWCFG_7_SEGMENT_LED_LEFT_UP             0xdf
#define SYS_HWCFG_7_SEGMENT_LED_DIGIT_0             0xc0
#define SYS_HWCFG_7_SEGMENT_LED_DIGIT_1             0xf9
#define SYS_HWCFG_7_SEGMENT_LED_DIGIT_2             0xa4
#define SYS_HWCFG_7_SEGMENT_LED_DIGIT_3             0xb0
#define SYS_HWCFG_7_SEGMENT_LED_DIGIT_4             0x99
#define SYS_HWCFG_7_SEGMENT_LED_DIGIT_5             0x92
#define SYS_HWCFG_7_SEGMENT_LED_DIGIT_6             0x82
#define SYS_HWCFG_7_SEGMENT_LED_DIGIT_7             0xf8
#define SYS_HWCFG_7_SEGMENT_LED_DIGIT_8             0x80
#define SYS_HWCFG_7_SEGMENT_LED_DIGIT_9             0x90
#define SYS_HWCFG_7_SEGMENT_LED_OFF                 0xff
#define SYS_HWCFG_7_SEGMENT_LED_E                   0x86

/*epld define*/
#define SYS_HWCFG_EPLD_PAGE_SIZE        0x10
#define SYS_HWCFG_EPLD_PAGE1_ADDR       0xEA010000
#define SYS_HWCFG_EPLD_MIN_PAGE         1
#define SYS_HWCFG_EPLD_MAX_PAGE         1

/* Power Status(Read Only, 0xE0100005)
 */
#define SYS_HWCFG_POWER_STATUS_ADDR                 (SYS_HWCFG_RCS1_BASE + 0xA0002 * SYS_HWCFG_RCS1_OFFSET) 

/* Fan & PWR status (Read , 0xEA0A0003)
 */
#define SYS_HWCFG_FAN_FAULT_STATUS_ADDR             (SYS_HWCFG_RCS1_BASE + 0xA0003 * SYS_HWCFG_RCS1_OFFSET) /*0xFF0A0003*/

#define SYS_HWCF_RPS_ADVANCED_CONTROL
#ifdef  SYS_HWCF_RPS_ADVANCED_CONTROL     
/* Suppose H/W EPLD register generic design based on Helix as:
 * ------------------------------------------------------------------------------------------
 * |        7          |          6        |        5       |           4      |         3       |                2            |        1        |        0           |
 * |----------|----------|-----------------------------|---------------|- --------|----------|
 * |  STATUS1  | STATUS2  | Reserved | Reserved | Reserved |POWERRPS_PRE| Reserved | System_PG |
 * ------------------------------------------------------------------------------------------
 * STATUS2: STATUS1: System_PG
     0       0         0       -- Reserved, no definition.
     0       0         1       -- RPS normal, RPS connected, RPS NOT delivering power
     0       1         0       -- RPS normal, RPS connected, RPS delivering power
     0       1         1       -- RPS fail
     1       0         X       -- Fan failed but thermal normal, impossible in ES3628BT
     1       1         X       -- Thermal failed, impossible in ES3628BT
     
 POWERRPS_PRE : RPS PRESENT SIGNAL FROM RPS
        0: RPS is NOT present
        1: RPS is present
 System_PG : INTERNAL POWER GOOD SIGNAL TO RPS, SYSTEM
        0: INTERNAL P/S FAIL 
        1: INTERNAL P/S GOOD
 */
/* these masks specify the bit to use
 */
#define SYS_HWCFG_RPS600W_NEW_DEFINITION              TRUE

#define SYS_HWCFG_PWR_STATUS_MASK                     0x01    /* 00000001 */
#define SYS_HWCFG_RPS_PRES_MASK                       0x04    /* 00000100 */
#define SYS_HWCFG_POWER_DETECT_STATUS1_MASK           0x80    /* 10000000 */
#define SYS_HWCFG_POWER_DETECT_STATUS2_MASK           0x40    /* 01000000 */  


/* these value specify the value of the status
 */
#define SYS_HWCFG_RPS_FAIL                            0x81    /* 10000001 */ /* BIT7:BIT0 =1:1 RPS fail */
#define SYS_HWCFG_RPS_FAN_OR_THERMAL_FAIL             0x40    /* 01000000 */ /* BIT6 = 1 RPS Thermal or fan fail */
#define SYS_HWCFG_RPS_PRES_OK                         0x04    /* 00000100 */ /* BIT 2 means RPS presnet */
#define SYS_HWCFG_PWR_STATUS_OK                       0x01    /* 00000001 */ /* BIT 0  means AC on   */
#else
/* Suppose H/W EPLD register generic design as:
 ------------------------------------------------------------------------------------------
 |     7     |     6    |    5     |     4    |    3     |     2    |    1     |     0    |
 |--------------------------------------------|----------|----------|----------|----------|
 |                    Reseved                 | RPS_PRES | PWR_PRES |RPS_STATUS|PWR_STATUS|
 ------------------------------------------------------------------------------------------
 */
/* these masks specify the bit to use
 */
#define SYS_HWCFG_PWR_STATUS_MASK                     0x01    /* 00000001 */
#define SYS_HWCFG_RPS_STATUS_MASK                     0x02    /* 00000010 */
#define SYS_HWCFG_PWR_PRES_MASK                       0x04    /* 00000100 */
#define SYS_HWCFG_RPS_PRES_MASK                       0x08    /* 00001000 */

/* these value specify the value of the status
 */
#define SYS_HWCFG_PWR_STATUS_OK                       0x01    /* 00000001 */ /* "1" means AC on   */
#define SYS_HWCFG_RPS_STATUS_OK                       0x02    /* 00000010 */ /* "1" means AC on   */
#define SYS_HWCFG_PWR_PRES_OK                         0x04    /* 00000100 */ /* "1" means present */
#define SYS_HWCFG_RPS_PRES_OK                         0x08    /* 00001000 */ /* "1" means presnet */

#endif /* End of #ifdef   SYS_HWCF_RPS_ADVANCED_CONTROL */

/* Fan type: Define which fan we used at this product 
 * SYS_HWCFG_FAN_MAX6651 ==> BLANC, BLANC_08, Hagrid, Hagrid L3
 * SYS_HWCFG_FAN_ADM1030 ==> EIF8X10G
 */
#define SYS_HWCFG_FAN_NONE       1         /* not fan control chip */
#define SYS_HWCFG_FAN_ADM1030    2         /* ADM1030 chip */
#define SYS_HWCFG_FAN_MAX6651    3         /* MAX6651 chip */
#define SYS_HWCFG_FAN_WIN83782   4         /* WINBOND W83782D chip */
#define SYS_HWCFG_FAN_ADT7475    5
#define SYS_HWCFG_FAN_CONTROLLER_TYPE      SYS_HWCFG_FAN_WIN83782

/* Define Module Present and Link Detected Register Address 
 */
#define SYS_HWCFG_MODULE_STATUS_ADDR                (SYS_HWCFG_RCS1_BASE + 0xA0004 * SYS_HWCFG_RCS1_OFFSET)  /*0xff500020*/

#define SYS_HWCFG_MODULE_A_PRZ_MASK                 0x80
#define SYS_HWCFG_MODULE_B_PRZ_MASK                 0x40
#define SYS_HWCFG_MODULE_PRZ_MASK                   (SYS_HWCFG_MODULE_A_PRZ_MASK | SYS_HWCFG_MODULE_B_PRZ_MASK)
#define SYS_HWCFG_MODULE_ABSENT                     0x00

/* Define Module ID A Register Address */
#define SYS_HWCFG_MODULE_ID_A_ADDR                  (SYS_HWCFG_RCS1_BASE + 0xA0005 * SYS_HWCFG_RCS1_OFFSET) /*0xEA0A0005*/
#define SYS_HWCFG_MODULE_ID_A_MASK                  0xe0

/* Define Module ID B Register Address */
#define SYS_HWCFG_MODULE_ID_B_ADDR                  (SYS_HWCFG_RCS1_BASE + 0xA0006 * SYS_HWCFG_RCS1_OFFSET) /*0xEA0A0006*/
#define SYS_HWCFG_MODULE_ID_B_MASK                  0xe0

#define SYS_HWCFG_MODULE_ID_XFP                     0x20

/* Define New Module IP for New 10G Copper module 
 * by Eric Hsu 
 */
#define SYS_HWCFG_MODULE_ID_10G_COPPER              0xa0
/* GBIC/MINIGBIC Tx Disable and SCL Select Setting at module A and B (Read/Write) 
 */
#define SYS_HWCFG_GBIC_AB_MINIGBIC_CONTROL_ADDR         (SYS_HWCFG_RCS1_BASE + 0xA0008 * SYS_HWCFG_RCS1_OFFSET) /*0xFF500040*/
#define SYS_HWCFG_GBIC_A_TX_DISABLE                     0x80
#define SYS_HWCFG_GBIC_B_TX_DISABLE                     0x40

/* GBIC/MINIGBIC Tx Disable and SCL Select Setting at module C and D (Read/Write) 
 */
#define SYS_HWCFG_GBIC_CD_MINIGBIC_CONTROL_ADDR         (SYS_HWCFG_RCS1_BASE + 0xD0003 * SYS_HWCFG_RCS1_OFFSET) /*0xFF500040*/
#define SYS_HWCFG_GBIC_C_TX_DISABLE                     0x80
#define SYS_HWCFG_GBIC_D_TX_DISABLE                     0x40

/* the constants here are for generic combo SFP detection 
 */
//#define SYS_HWCFG_GBIC_STATUS_0_ADDR                    (SYS_HWCFG_CPLD_BASE + 0x0B) /*0xE010000B*/                  
//#define SYS_HWCFG_GBIC_STATUS_1_ADDR                    (SYS_HWCFG_CPLD_BASE + 0x0D) /*0xE010000D*/                  
/* This bit will tell the bit mask result and telling whether GBIC module is present
 */
#define SYS_HWCFG_GBIC_PRESENT_BIT                      0

/* Define the EPLD address for GBIC A
 */
#define SYS_HWCFG_COMBO_PORT1_SFP_STATUS_ADDR           (SYS_HWCFG_RCS1_BASE + 0xA0009 * SYS_HWCFG_RCS1_OFFSET)
#define SYS_HWCFG_COMBO_PORT1_SFP_MODULE_PRESENT_MASK   0x08

/* Define the EPLD address for GBIC B
 */
#define SYS_HWCFG_COMBO_PORT2_SFP_STATUS_ADDR           (SYS_HWCFG_RCS1_BASE + 0xA0009 * SYS_HWCFG_RCS1_OFFSET)
#define SYS_HWCFG_COMBO_PORT2_SFP_MODULE_PRESENT_MASK   0x04

/* Define the EPLD address for GBIC C
 */
#define SYS_HWCFG_COMBO_PORT3_SFP_STATUS_ADDR           (SYS_HWCFG_RCS1_BASE + 0xD0003 * SYS_HWCFG_RCS1_OFFSET)
#define SYS_HWCFG_COMBO_PORT3_SFP_MODULE_PRESENT_MASK   0x08

/* Define the EPLD address for GBIC D
 */
#define SYS_HWCFG_COMBO_PORT4_SFP_STATUS_ADDR           (SYS_HWCFG_RCS1_BASE + 0xD0003 * SYS_HWCFG_RCS1_OFFSET)
#define SYS_HWCFG_COMBO_PORT4_SFP_MODULE_PRESENT_MASK   0x04

#define SYS_HWCFG_MAX_NBR_OF_I2C_CHANNEL       8 

#define SYS_HWCFG_PCA9548_CHANNEL_0                  1 /* by H/W spec. channel_0 is for M41T11 */
#define SYS_HWCFG_PCA9548_CHANNEL_1                  2 /* by H/W spec. channel_1 is for W83782D */

#define SYS_HWCFG_PCA9548_DATAADDR                   0
#define SYS_HWCFG_PCA9548_DEFAULT_CHANNEL     0

#define SYS_HWCFG_I2C_PHYSICAL_ADDR_PCA9548          0x70
#define SYS_HWCFG_I2C_PHYSICAL_ADDR_M41T11           0x68
#define SYS_HWCFG_I2C_PHYSICAL_ADDR_W83782D          0x29
#define SYS_HWCFG_I2C_PHYSICAL_ADDR_10G_MODULE_1     0x50
#define SYS_HWCFG_I2C_PHYSICAL_ADDR_10G_MODULE_2     0x50
#define SYS_HWCFG_I2C_PHYSICAL_ADDR_SFP_1            0x50
#define SYS_HWCFG_I2C_PHYSICAL_ADDR_SFP_2            0x50
#define SYS_HWCFG_I2C_PHYSICAL_ADDR_SFP_3            0x50
#define SYS_HWCFG_I2C_PHYSICAL_ADDR_SFP_4            0x50

/* PIC device numbers, needed by diagnostic
 */
#define SYS_HWCFG_PCI_DEVICE_NUM_1                  21 /* In diag, thomas */
#define SYS_HWCFG_PCI_DEVICE_NUM_2                  22
#define SYS_HWCFG_PCI_DEVICE_NUM_3                  23
#define SYS_HWCFG_PCI_DEVICE_NUM_4                  24
#define SYS_HWCFG_PCI_DEVICE_NUM_5                  25

#define SYS_HWCFG_MPC824x_PCI_CONFIG_ADDR_REG	    0xF0010900

/*  If this project is multi-chip and need to addr sync for different chips
 */
#define SYS_HWCFG_ADDR_MULTI_CHIP_SYNC              TRUE /* Useless, for future 50 port */

/* Real Time Clock definition
 */
#define SYS_HWCFG_SUPPORT_RTC                       FALSE

/* Misc. Control (0xEA0A000A, Write)
 */
#define SYS_HWCFG_MISC_CONTROL_ADDR                 (SYS_HWCFG_RCS1_BASE + 0xA000A * SYS_HWCFG_RCS1_OFFSET)
#define SYS_HWCFG_XG1_PWR_EN_MASK                   0x40
#define SYS_HWCFG_XG2_PWR_EN_MASK                   0x04
#define SYS_HWCFG_XG1_TX_MASK                       0x80
#define SYS_HWCFG_XG2_TX_MASK                       0x08
#define SYS_HWCFG_MISC_CONTROL_OFF                  0x33

/* SDRAM Configuraton (EA0D-0004, Write-Only) 
 */
#define SYS_HWCFG_SDRAM_CONFIG_ADDR                 (SYS_HWCFG_RCS1_BASE + 0xD0004 * SYS_HWCFG_RCS1_OFFSET) /*0xFF0D0004*/
#define SYS_HWCFG_SDRAM_TYPE_MASK                   0xf8    /* 11111000 */
#define SYS_HWCFG_SDRAM_SIZE_MASK                   0x07    /* 00000111 */
#define SYS_HWCFG_SDRAM_TYPE_0                      0x00    /* 00000xxx */
#define SYS_HWCFG_SDRAM_TYPE_1                      0x00    /* 00001xxx */
#define SYS_HWCFG_SDRAM_TYPE_2                      0x00    /* 00010xxx */
#define SYS_HWCFG_SDRAM_TYPE_3                      0x00    /* 00011xxx */
#define SYS_HWCFG_SDRAM_TYPE_DIMM                   0xf8    /* 11111xxx */

#define SYS_HWCFG_SDRAM_SIZE_4M                     0x00    /* xxxxx000 */
#define SYS_HWCFG_SDRAM_SIZE_8M                     0x01    /* xxxxx001 */
#define SYS_HWCFG_SDRAM_SIZE_16M                    0x02    /* xxxxx010 */
#define SYS_HWCFG_SDRAM_SIZE_32M                    0x03    /* xxxxx011 */
#define SYS_HWCFG_SDRAM_SIZE_64M                    0x04    /* xxxxx100 */
#define SYS_HWCFG_SDRAM_SIZE_128M                   0x05    /* xxxxx101 */
#define SYS_HWCFG_SDRAM_SIZE_256M                   0x06    /* xxxxx110 */
#define SYS_HWCFG_SDRAM_SIZE_512M                   0x07    /* xxxxx111 */

/* EPLD Version (Read Only, 0xE010000F) 
  * ----------------------------------------------------------------------------
  * |      7        |        6      |        5       |        4      |        3       |        2        |        1        |        0        |
  * |--------|--------|-------- |--------|-------- |---------|---------|---------|
  * | HW_ID4 | HW_ID3 | HW_ID2   | HW_ID1 |  HW_ID0  | VER_ID2  | VER_ID1  | VER_ID0  |
  * ----------------------------------------------------------------------------
  HW_ID4
        1: External
        0: Internal
  HW_ID3
        1: POE
        0: NonPoE
  HW_ID2
        1: 52 Ports
        0: 28 Ports
  HW_ID1
        1: AC
        0: DC
  HW_ID0
        Reserved
  VER ID:Version Description
        000 R0A
        001 R0B
         ?
        100 R01
  */
#define SYS_HWCFG_EPLD_VERSION_ADDR                 (SYS_HWCFG_RCS1_BASE + 0xD0006 * SYS_HWCFG_RCS1_OFFSET)

#define SYS_HWCFG_I2C_SLAVE_EEPROM                  0xa0

/* Winbond W83782D Control address via I2C bus */
#define SYS_HWCFG_I2C_SLAVE_THERMAL_0               SYS_HWCFG_I2C_PHYSICAL_ADDR_W83782D
#define SYS_HWCFG_I2C_SLAVE_THERMAL_1               SYS_HWCFG_I2C_PHYSICAL_ADDR_W83782D
#define SYS_HWCFG_I2C_SLAVE_FAN_MONITOR_0           SYS_HWCFG_I2C_PHYSICAL_ADDR_W83782D
#define SYS_HWCFG_I2C_SLAVE_FAN_MONITOR_1           SYS_HWCFG_I2C_PHYSICAL_ADDR_W83782D

/* Winbond W83782D Controller Register Address */
#define SYS_HWCFG_MONITOR_CFG                       0x40
#define SYS_HWCFG_MONITOR_CHIP_ID                   0x58
#define SYS_HWCFG_MONITOR_CLKINSEL                  0x4B
#define SYS_HWCFG_FAN_INOUT                         0x4D

#define SYS_HWCFG_FAN_PWM_1                         0x5B
#define SYS_HWCFG_FAN_PWM_3                         0x5E    /* PWM_2 is not used */
#define SYS_HWCFG_FAN_PWM_4                         0x5F

#define SYS_HWCFG_FAN_PWM_CLKSEL                    0x5C    /* PWM 1 & 2 @ Bank 0; PWM 3 & 4 @ Bank 4 */

#define SYS_HWCFG_FAN_TACH_1                        0x28
#define SYS_HWCFG_FAN_TACH_2                        0x29
#define SYS_HWCFG_FAN_TACH_3                        0x2A

#define SYS_HWCFG_THERMAL_1                         0x27

#define SYS_HWCFG_THERMAL_ADDRESS_REG               0x4A
#define SYS_HWCFG_THERMAL_2_ADD_MASK                0x07
#define SYS_HWCFG_THERMAL_3_ADD_MASK                0x70
#define SYS_HWCFG_BANK_SEL                          0x4E
#define SYS_HWCFG_THERMAL_2_H                       0x50
#define SYS_HWCFG_THERMAL_2_L                       0x51
#define SYS_HWCFG_THERMAL_DIODE_SELECT              0x59
#define SYS_HWCFG_THERMAL_TYPE_SELECT               0x5D

/* kinghong: move from sys_adpt.h: SYS_ADPT_MAX_NBR_OF_THERMAL_PER_UNIT */
#define SYS_HWCFG_MAX_NBR_OF_THERMAL_PER_UNIT       2

#define SYS_HWCFG_MAX_NBR_OF_FAN_PER_UNIT               3

/* The followng definitions are for build only and useless */

#define SYS_HWCFG_SYSTEM_MODULE_1_BUS_ENABLE        0x00
#define SYS_HWCFG_SYSTEM_MODULE_1_BUS_DISABLE       0x04

#define SYS_HWCFG_SYSTEM_MODULE_2_BUS_ENABLE        0x00
#define SYS_HWCFG_SYSTEM_MODULE_2_BUS_DISABLE       0x02

#define SYS_HWCFG_SYSTEM_MODULE_1_BUS_MASK          0x04
#define SYS_HWCFG_SYSTEM_MODULE_2_BUS_MASK          0x02

/* End of useless definitons */

/* UART device name on linux.
 * it is necessary to define SYS_HWCFG_UART_CHANNEL2_DEVICE_NAME when
 * support second UART.
 */
//#define SYS_HWCFG_UART_CHANNEL1_DEVICE_NAME  "/dev/ttyS0"
//#undef  SYS_HWCFG_UART_CHANNEL2_DEVICE_NAME

#define SYS_HWCFG_FLASH_MTD_NAME_LEN        32
typedef struct SYS_HWCFG_FlashEntry_S 
{
	UI32_T  mtdnum;
    UI32_T  address;
    UI32_T  size;
    UI32_T  type;
    BOOL_T  writeable;
    UI8_T   name[SYS_HWCFG_FLASH_MTD_NAME_LEN];
} SYS_HWCFG_FlashEntry_T;

typedef struct PHYSICAL_EPLD_ADDR_S
{
    UI32_T start_addr;              /*the start address of EPLD*/
    UI32_T end_addr;                /*the end address of EPLD*/  
} PHYSICAL_EPLD_ADDR_T;


UI32_T SYS_HWCFG_GetFlashPartitionNumber(void);

UI32_T SYS_HWCFG_GetFlashBaseAddress(void);

SYS_HWCFG_FlashEntry_T *SYS_HWCFG_GetNextFlashPartitionEntry(SYS_HWCFG_FlashEntry_T *flash_entry_p);

/*add by michael.wang 2008-7-4*/
/*---------------------------------------------------------------------------------
 * FUNCTION NAME: SYS_HWCFG_GetGBIC_I2CChannelInfo
 *---------------------------------------------------------------------------------
 * PURPOSE: Get pca9548 device address and channel info.
 * INPUT:    port_index :GBIC port index
 * OUTPUT: dev:pca9548 device address
 *              channel:I2C device channel
 * RETUEN:  TRUE/FALSE
 * NOTES:   
 *---------------------------------------------------------------------------------
 */
BOOL_T SYS_HWCFG_GetGBIC_I2CChannelInfo(UI32_T  device_index, UI32_T *dev, UI8_T *channel);

/*add by michael.wang 2008-9-18*/
/*---------------------------------------------------------------------------------
 * FUNCTION NAME: SYS_HWCFG_SwitchChip_MiscInit
 *---------------------------------------------------------------------------------
 * PURPOSE: config EPLD important bit for module
 * INPUT:    
 * OUTPUT: 
 * RETUEN:  TRUE/FALSE
 * NOTES:   
 *---------------------------------------------------------------------------------
 */
BOOL_T SYS_HWCFG_SwitchChip_MiscInit();
BOOL_T SYS_HWCFG_Getepldaddr(PHYSICAL_EPLD_ADDR_T *epldaddr);

BOOL_T SYS_HWCFG_Getledproc(UI8_T *ledproc, int device_id);

#endif
/* End of SYS_HWCFG.H */



