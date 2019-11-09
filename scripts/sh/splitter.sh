#!/bin/sh
echo "Filename $1"
echo "Slice size $2"
let head=$(sed -n -e '/^@/p' $1 | wc -l)
let lines=$(sed -n "$=" $1)
let parts=$((($lines - $head) / $2 ))
let dest=$(basename $1 .sam)
dest=$(basename $1 .sam)
mkdir -p $dest
let last=$((($lines - $head) % $2))
if [ "$last" -gt 0 ]
then
    parts=$(($parts + 1))
fi
for i in $(seq 1 $parts)
do
    out=$dest\/$dest\_$i\.sam
    head $1 -n $head > $out
    < $1 tail -n +"$(($head + 1 +$2*($i - 1)))" | head -n "$2" >> $out
    samtools view -Sb $out -o $dest\/$dest\_$i\.bam
    rm $out
done
