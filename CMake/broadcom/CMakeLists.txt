
set(PROJECT_NAME L4)
project(${PROJECT_NAME})

cmake_minimum_required(VERSION 2.6)

#set(SDK_FOLDER sdk_5.5.2)
set(SDK_FOLDER sdk_6.4.1)

#set(GMOCK_SRC vendor/gmock-1.7.0)
#set(GMOCK_LIB ../${PROJECT_NAME}.build/gmock)

#set(LIBXML_SRC ../${PROJECT_NAME}/vendor/libxml2/)
#set(LIBXML_LIB ../${PROJECT_NAME}/vendor/libxml2/lib)

set (PRJ_SRC ../../src)

IF( APPLE)
  set(CMAKE_OSX_ARCHITECTURES $(ARCHS_STANDARD_32_BIT))
  #  set(CMAKE_CXX_FLAGS "${CMAXE_CXX_FLAGS} -Wall -DSYS_HWCFG_LITTLE_ENDIAN_CPU")
  #  set(CMAKE_C_FLAGS "${CMAXE_C_FLAGS} -Wall -DSYS_HWCFG_LITTLE_ENDIAN_CPU")
  set(CMAKE_CXX_FLAGS "${CMAXE_CXX_FLAGS} -DSYS_HWCFG_LITTLE_ENDIAN_CPU -DRTOS_STRINGS")
  set(CMAKE_C_FLAGS "${CMAXE_C_FLAGS} -DSYS_HWCFG_LITTLE_ENDIAN_CPU -DRTOS_STRINGS")
  #add_definitions(-DSYS_HWCFG_LITTLE_ENDIAN_CPU)

  SET(CMAKE_OSX_ARCHITECTURES "i386")
ENDIF (APPLE)


include_directories(${PRJ_SRC}/test
                    ${PRJ_SRC}/test/core/mgmt/sysrsc
                    ${PRJ_SRC}/test/driver/swdrv/broadcom
                    ${PRJ_SRC}/test/driver/swdrv
                    ${PRJ_SRC}/test/apps/cli
                    ${PRJ_SRC}/test/apps/cli/helper
                    ${PRJ_SRC}/sysinclude/oem/es4626f_flf_38
                    ${PRJ_SRC}/sysinclude
                    ${PRJ_SRC}/sysinclude/mibconstants
                    ${PRJ_SRC}/cmnlib/user/include
                    ${PRJ_SRC}/cmnlib/common/include
                    ${PRJ_SRC}/user/core/mgmt/sysrsc/include
                    ${PRJ_SRC}/user/core/mgmt/sys_callback/include
                    ${PRJ_SRC}/kernel/driver/dev_driver/broadcom
                    ${PRJ_SRC}/kernel/driver/dev_driver/broadcom/${SDK_FOLDER}
                    ${PRJ_SRC}/kernel/driver/dev_driver/broadcom/${SDK_FOLDER}/include
                    ${PRJ_SRC}/kernel/driver/dev_driver/broadcom/${SDK_FOLDER}/include/shared
                    ${PRJ_SRC}/kernel/driver/dev_driver/broadcom/${SDK_FOLDER}/src/bcm
                    ${PRJ_SRC}/kernel/driver/dev_driver/broadcom/${SDK_FOLDER}/bcm/esw
                    ${PRJ_SRC}/user/apps/cli/oem/es3510ma_flf_38
                    ${PRJ_SRC}/user/apps/cli
                    ${PRJ_SRC}/user/apps/cli/include
                    ${PRJ_SRC}/user/core/qos_v2/include
                    ${PRJ_SRC}/user/core/qos_v2/cos/include
                    ${PRJ_SRC}/user/core/qos_v2/rule_mgr/include
                    ${PRJ_SRC}/user/core/qos_v2
                    ${PRJ_SRC}/user/core/qos_v2/cos
                    ${PRJ_SRC}/user/core/qos_v2/rule_mgr
                    ${PRJ_SRC}/user/core/security/include
                    ${PRJ_SRC}/user/driver/devdrv/include
                    ${PRJ_SRC}/user/driver/devdrv/include/esw
                    ${PRJ_SRC}/user/driver/iscdrv/include
                    ${PRJ_SRC}/user/driver/swdrv/include
                    ${PRJ_SRC}/user/driver/swdrv/broadcom
                    ${PRJ_SRC}/user/driver/swdrv/broadcom/esw
                    ${PRJ_SRC}/user/driver/swdrv/broadcom/esw/oem/es4626f_flf_38
                    ${PRJ_SRC}/user/driver/swdrv
                    ${PRJ_SRC}/user/core/qos_v2/include
                    ${PRJ_SRC}/user/core/qos_v2/rule_mgr/include
                    ${PRJ_SRC}/fakelib
                    #${PRJ_SRC}/user/core/mgmt/dai/include
                    #${PRJ_SRC}/user/core/mgmt/dai
                    ${PRJ_SRC}/user/core/mgmt/time_range/include
                    )

#link_directories(${GMOCK_LIB} ${GMOCK_LIB}/gtest)

# add_definitions(-std=c++0x)

#if(MSVC)
#set(CMAKE_CXX_FLAGS "${CMAXE_CXX_FLAGS} -Wall -DGTEST_HAS_PTHREAD=0 -D_VARIADIC_MAX=10")
#set(CMAKE_CXX_FLAGS_DEBUG "-D_DEBUG /Od /MTd /ZI")
#endif()

#if(APPLE)
#set(CMAKE_CXX_FLAGS "${CMAXE_CXX_FLAGS} -Wall -DGTEST_HAS_PTHREAD=1")
#endif()


file(GLOB LIBS
    ${PRJ_SRC}/user/apps/cli/cli_lib.c
    ${PRJ_SRC}/user/apps/cli/cli_tbl.c
    ${PRJ_SRC}/user/apps/cli/cli_api.c
    ${PRJ_SRC}/user/apps/cli/cli_api_ethernet.c
    ${PRJ_SRC}/user/apps/cli/cli_api_ethernet_lib.c
    ${PRJ_SRC}/fakelib/stktplg_shom.c
    ${PRJ_SRC}/cmnlib/common/datastru/l_link_list.c
    ${PRJ_SRC}/cmnlib/common/datastru/l_sort_lst.c
    ${PRJ_SRC}/cmnlib/common/memmgmt/l_pt.c
    ${PRJ_SRC}/cmnlib/user/kernel/null/sysfun.c
    ${PRJ_SRC}/user/core/mgmt/sysrsc/sysrsc_mgr.c
    ${PRJ_SRC}/cmnlib/common/convert/l_inet.c
    ${PRJ_SRC}/cmnlib/common/convert/l_ipv4v6.c
    ${PRJ_SRC}/cmnlib/user/memmgmt/l_mm.c
    ${PRJ_SRC}/cmnlib/common/l_rstatus.c
    ${PRJ_SRC}/cmnlib/common/convert/l_base64.c
    ${PRJ_SRC}/cmnlib/common/l_stdlib.c
    ${PRJ_SRC}/fakelib/backdoor_mgr.c
    ${PRJ_SRC}/fakelib/sys_time.c
    ${PRJ_SRC}/user/core/mgmt/sysrsc/sysrsc_mgr_private.c
    ${PRJ_SRC}/fakelib/stktplg_pom.c
    ${PRJ_SRC}/fakelib/stktplg_om.c
    ${PRJ_SRC}/fakelib/swctrl.c
    ${PRJ_SRC}/fakelib/swctrl_pom.h
    ${PRJ_SRC}/fakelib/swctrl_pom.c
    ${PRJ_SRC}/user/driver/iscdrv/isc.c
    )

file(GLOB TESTS
    ${PRJ_SRC}/test/*.c
    ${PRJ_SRC}/test/*.h
    ${PRJ_SRC}/test/core/qos_v2/l4_main.c)

file(GLOB CLI_TEST_HELPER
    ${PRJ_SRC}/test/apps/cli/helper/*)

file(GLOB CLI_TEST_CASES
    ${PRJ_SRC}/test/apps/cli/*.c)

file(GLOB DRIVER_TESTS
    ${PRJ_SRC}/test/driver/swdrv/broadcom/*_utest.c)

file(GLOB DRIVER_TEST_CASES
    ${PRJ_SRC}/test/driver/swdrv/broadcom/*_utest_cases.c)

file(GLOB L4_TESTS
    ${PRJ_SRC}/test/core/qos_v2/*_utest.c)

file(GLOB L4_TEST_CASES
    ${PRJ_SRC}/test/core/qos_v2/*_utest_cases.c)

file(GLOB DRIVER_LIB
    ${PRJ_SRC}/user/driver/swdrv/rule_lib_array.c
    ${PRJ_SRC}/user/driver/swdrv/rule_lib_array.h
    ${PRJ_SRC}/user/driver/swdrv/rule_om_exp.c)

file(GLOB DRIVER_SRC
    ${PRJ_SRC}/user/driver/swdrv/broadcom/*.c
    ${PRJ_SRC}/user/driver/swdrv/broadcom/esw/*.c
    ${PRJ_SRC}/user/driver/swdrv/broadcom/esw/oem/es4626f_flf_38/*)

file(GLOB DRIVER_HEADER
    ${PRJ_SRC}/user/driver/swdrv/include/*.h
    ${PRJ_SRC}/user/driver/swdrv/broadcom/esw/*.h)

file(GLOB L4_SRC
    ${PRJ_SRC}/user/core/qos_v2/*.c
    ${PRJ_SRC}/user/core/qos_v2/rule_mgr/*.c)

file(GLOB L4_HEADER
    ${PRJ_SRC}/user/core/qos_v2/include/*
    ${PRJ_SRC}/user/core/qos_v2/rule_mgr/include/*)

file(GLOB CLI_SRC
    ${PRJ_SRC}/user/apps/cli/cli_api_acl_qosv2.c
    ${PRJ_SRC}/user/apps/cli/cli_api_l4_ds_qosv2.c)

file(GLOB CLI_HEADER
    ${PRJ_SRC}/user/apps/cli/cli_api_acl.h
    ${PRJ_SRC}/user/apps/cli/cli_api_l4_ds.h)

file(GLOB DEVDRV_SRC
    ${PRJ_SRC}/kernel/driver/dev_driver/broadcom/*.c
    ${PRJ_SRC}/kernel/driver/dev_driver/broadcom/${SDK_FOLDER}/dev_rm.c)

file(GLOB_RECURSE SDK_SRC
    ${PRJ_SRC}/kernel/driver/dev_driver/broadcom/${SDK_FOLDER}/src/*.c)

file(GLOB SDK_HEADER
    ${PRJ_SRC}/kernel/driver/dev_driver/broadcom/${SDK_FOLDER}/bcm.h
    ${PRJ_SRC}/kernel/driver/dev_driver/broadcom/${SDK_FOLDER}/include/bcm/error.h
    ${PRJ_SRC}/kernel/driver/dev_driver/broadcom/${SDK_FOLDER}/include/bcm/types.h
    ${PRJ_SRC}/kernel/driver/dev_driver/broadcom/${SDK_FOLDER}/include/bcm/field.h
    ${PRJ_SRC}/kernel/driver/dev_driver/broadcom/${SDK_FOLDER}/include/bcm_int/common/field.h
    ${PRJ_SRC}/kernel/driver/dev_driver/broadcom/${SDK_FOLDER}/include/bcm_int/control.h
    ${PRJ_SRC}/kernel/driver/dev_driver/broadcom/${SDK_FOLDER}/include/bcm_int/dispatch.h
    ${PRJ_SRC}/kernel/driver/dev_driver/broadcom/${SDK_FOLDER}/include/bcm_int/esw/field.h
    ${PRJ_SRC}/kernel/driver/dev_driver/broadcom/${SDK_FOLDER}/include/bcm_int/esw_dispatch.h
    ${PRJ_SRC}/kernel/driver/dev_driver/broadcom/${SDK_FOLDER}/include/customer/accton_bcm.h
    ${PRJ_SRC}/kernel/driver/dev_driver/broadcom/${SDK_FOLDER}/include/customer/accton_field.h
    ${PRJ_SRC}/kernel/driver/dev_driver/broadcom/${SDK_FOLDER}/include/shared/error.h
    ${PRJ_SRC}/kernel/driver/dev_driver/broadcom/${SDK_FOLDER}/include/shared/util.h
    ${PRJ_SRC}/kernel/driver/dev_driver/broadcom/${SDK_FOLDER}/include/soc/drv.h
    ${PRJ_SRC}/kernel/driver/dev_driver/broadcom/${SDK_FOLDER}/include/soc/field.h
)

file(GLOB TIME_RANGE_SRC
    ${PRJ_SRC}/user/core/mgmt/time_range/*.c
    ${PRJ_SRC}/user/core/mgmt/time_range/include/*
    )

source_group(lib FILES ${LIBS})
source_group(src\\driver FILES ${DRIVER_SRC})
source_group(src\\driver\\include FILES ${DRIVER_HEADER})
source_group(src\\driver\\lib FILES ${DRIVER_LIB})
source_group(src\\l4 FILES ${L4_SRC})
source_group(src\\l4\\include FILES ${L4_HEADER})
source_group(src\\cli FILES ${CLI_SRC})
source_group(src\\cli\\include FILES ${CLI_HEADER})
source_group(src\\devdrv FILES ${DEVDRV_SRC})
source_group(src\\devdrv\\sdk FILES ${SDK_SRC})
source_group(src\\devdrv\\sdk\\include FILES ${SDK_HEADER})
source_group(src\\time_range FILES ${TIME_RANGE_SRC})
source_group(test FILES ${TESTS})
source_group(test\\l4 FILES ${L4_TESTS})
source_group(test\\l4\\test_case FILES ${L4_TEST_CASES})
source_group(test\\cli\\test_case FILES ${CLI_TEST_CASES})
source_group(test\\cli\\test_helper FILES ${CLI_TEST_HELPER})
source_group(test\\driver FILES ${DRIVER_TESTS})
source_group(test\\driver\\test_case FILES ${DRIVER_TEST_CASES})

set_source_files_properties(${L4_TESTS}
   PROPERTIES HEADER_FILE_ONLY TRUE)

set_source_files_properties(${L4_TEST_CASES}
   PROPERTIES HEADER_FILE_ONLY TRUE)

set_source_files_properties(${CLI_TEST_CASES}
   PROPERTIES HEADER_FILE_ONLY TRUE)

set_source_files_properties(${DRIVER_TESTS}
   PROPERTIES HEADER_FILE_ONLY TRUE)

set_source_files_properties(${CLI_TESTS}
   PROPERTIES HEADER_FILE_ONLY TRUE)

set_source_files_properties(${DRIVER_TEST_CASES}
   PROPERTIES HEADER_FILE_ONLY TRUE)

set_source_files_properties(${DRIVER_SRC}
   PROPERTIES HEADER_FILE_ONLY TRUE)

set_source_files_properties(${L4_SRC}
   PROPERTIES HEADER_FILE_ONLY TRUE)

set_source_files_properties(${CLI_SRC}
   PROPERTIES HEADER_FILE_ONLY TRUE)

set_source_files_properties(${DEVDRV_SRC}
   PROPERTIES HEADER_FILE_ONLY TRUE)

set_source_files_properties(${SDK_SRC}
   PROPERTIES HEADER_FILE_ONLY TRUE)

add_executable(${PROJECT_NAME}
   ${LIBS}
   ${TESTS}
   ${L4_TESTS}
   ${L4_TEST_CASES}
   ${CLI_TEST_CASES}
   ${CLI_TEST_HELPER}
   ${DRIVER_TESTS}
   ${DRIVER_TEST_CASES}
   ${CLI_SRC}
   ${CLI_HEADER}
   ${L4_SRC}
   ${L4_HEADER}
   ${DRIVER_SRC}
   ${DRIVER_HEADER}
   ${DRIVER_LIB}
   ${DEVDRV_SRC}
   ${SDK_SRC}
   ${SDK_HEADER}
   ${TIME_RANGE_SRC})

# target_link_libraries(cgi_demo pthread)
#target_link_libraries(${PROJECT_NAME} gmock)
#target_link_libraries(${PROJECT_NAME} gtest)
#target_link_libraries(${PROJECT_NAME} libxml2)

if(MSVC)
    target_link_libraries(${PROJECT_NAME} ws2_32)
endif()
