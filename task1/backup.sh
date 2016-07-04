#!/bin/bash

if [ $# -eq 2 -o $# -eq 3 ]; then
	DATA="$1"
	LIST="/tmp/backlist.txt"
	DST="$2"
	FILE="./backup"
	while read line; do EXCL="$EXCL --exclude $line"; done < "$FILE"
	DATE=`date +%Y-%m-%d`
	LAST=$(find $DST -type f -print0|xargs -0 ls -drt|tail -n 1)
	TIME=$(date +%d -r "$LAST")
	if [ $# -eq 3 ]; then
		if [ ! -f "$DST/data_full.tar" ] ; then
        		tar cvf "$DST/data_full.tar" $DATA $EXCL
        		rm -f $DST/data_diff*
		else
        		find $DATA -depth -type f \( -ctime $TIME -o -mtime $TIME \) -print > $LIST
        		tar cvfT  "$DST/data_diff_$DATE.tar" "$LIST" 
			rm "/tmp/backlist.txt"
		fi
	else
		if [ ! -d "$DST/FULL" ] ; then
			mkdir "$DST/FULL"
			cp -av "$DATA/" "$DST/FULL"
		else
			mkdir "$DST/DIFF_$DATE"
			find $DATA -depth -type f \( -ctime $TIME -o -mtime $TIME \) -print -exec cp '{}' "$DST\DIFF_$DATE" \;
		fi
	fi
else
	echo "Wrong parameters"
fi
