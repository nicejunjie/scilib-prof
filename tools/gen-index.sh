#!/bin/bash 

file=$1

fg_name=`basename $(pwd)`
fg_name=`basename $(dirname $(realpath $file))`


cat $file | sed 's/#.*//g' | grep -v ^$ | awk -F\( '{print $1}' | awk '{print $NF}' | sed 's/_*$//' | sed 's/$/_index,/' >index1.h

echo "#define ${fg_name} \\" >  index2.h
#cat $file |sed 's/#.*//g'|grep -v ^$|awk -F\( '{print $1}'|awk -v VAR=$fg_name '{printf "X(\"%s\", \"dbi_%s\", \"%s\"),\\\n", $NF,$NF, VAR}'  >> ${fg_name}-index2.h

cat $file | sed 's/#.*//g' | grep -v ^$ | awk -F\( '{print $1}' | \
awk -v VAR=$fg_name 'BEGIN {ORS=""}
{
    if (NR > 1) print ",\\\n"
    printf "X(\"%s\", \"dbi_%s\", \"%s\")", $NF, $NF, VAR
}
END {print "\n"}' >> index2.h


