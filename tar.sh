#!/bin/bash
cd "/Users/swei/Documents/Work/code/"
i=1
NOW=$(date +"%Y_%m_%d")

while [ $i -lt 100 ]; do
    printf -v FILE 'l4_%s_%02d.tar.bz2' $NOW $i

    if [ -f "$FILE" ]; then
        ((i++))
        continue
    else
        break
    fi
done

#echo $FILE
tar -cjf "$FILE" --exclude="*.git*" --exclude="*.DS_Store" --exclude=".*" ./l4
