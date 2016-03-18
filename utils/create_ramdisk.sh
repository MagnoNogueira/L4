#!/bin/sh
DIRECTORY="/Volumes/ramdisk/"
if [ ! -d "$DIRECTORY" ]; then
NUMSECTORS=524288 # 1 sector=512 bytes, 256MB
diskutil erasevolume HFS+ "ramdisk" `hdiutil attach -nomount ram://$NUMSECTORS`
fi
