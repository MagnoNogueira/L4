#! /bin/bash

# Get CLI function from cli_cmd.c
#
# usage:
#     create_cli_func cli_oem_dir
#
# example
#     create_cli_func.sh ./oem/es4626f_flf_38
#

DEBUG='yes'

if [ ${CP_SRC_EXPORT_DEBUG:-'no'} == 'yes' ]; then
	DEBUG="yes"
fi

#	if [ $DEBUG == 'yes' ]; then
#		echo "Debug mode is enabled"
#	fi


SRC_DIR=$(cd $1; pwd)

[ $DEBUG == 'yes' ] && echo "\$SRC_DIR=$SRC_DIR"

cli_cmd_c_path="$SRC_DIR/cli_cmd.c"
[ $DEBUG == 'yes' ] && echo "\$cli_cmd_c_path=$cli_cmd_c_path"

out_file_c_path=$SRC_DIR/cli_api_all.c
out_file_h_path=$SRC_DIR/cli_api_all.h

if test ! -f "$cli_cmd_c_path"; then
	echo "$cli_cmd_c_path is not found"
	exit 1
fi

rm $out_file_c_path
rm $out_file_h_path

function_list=$(cat "$cli_cmd_c_path" | grep 'CLI_API_' | sed 's|,||g' | sort | uniq)

echo '#ifndef __CLI_API_ALL_H__' >>$out_file_h_path
echo '#define __CLI_API_ALL_H__' >>$out_file_h_path
echo '#include "cli_def.h"' >>$out_file_h_path

echo '#include <stdio.h>' >>$out_file_c_path
echo '#include "cli_msg.h"' >>$out_file_c_path
echo '#include "cli_api_all.h"' >>$out_file_c_path

for function in $function_list; do
	[ $DEBUG == 'yes' ] && echo "\$function=$function"
    cat <<E_O_DEF >>$out_file_c_path
UI32_T $function(UI16_T cmd_idx, char *arg[], CLI_TASK_WorkingArea_T *ctrl_P)
{
    printf("%s\r\n", __FUNCTION__);
    return CLI_NO_ERROR;
}
E_O_DEF

    cat <<E_O_DEC >>$out_file_h_path
UI32_T $function(UI16_T cmd_idx, char *arg[], CLI_TASK_WorkingArea_T *ctrl_P);
E_O_DEC

done

echo '#endif' >>$out_file_h_path

echo "Done"
