#!/bin/sh

cleaner ()
{
    rm $1.sam
    rm $1.bam
    rm $2.bam
    rm $2.sam
}

checkFile ()
{
    GraySortFile=$1
    SamtoolsFile=$2
    
    GraySortFile=`echo $1 | sed 's/'bam$'/sam/'`
    samtools view -h $1 -o $GraySortFile
  
    SamtoolsFile=`echo $2 | sed 's/'bam$'/sam/'`
    samtools view -h $2 -o $SamtoolsFile

    GraySortHash=`echo $(cksum $GraySortFile) | awk '{print $1 " " $2}'`
    SamtoolsHash=`echo $(cksum $SamtoolsFile) | awk '{print $1 " " $2}'`

    echo "GraySortHash: " $GraySortHash
    echo "SamtoolsHash: " $SamtoolsHash

    if [ "$GraySortHash" = "$SamtoolsHash" ]
    then
        echo "\033[42m[SUCCES] I/O is correct \033[0m"
    else
        echo "\033[41m[FAILED] I/O is not correct \033[0m"
    fi
}

processFile ()
{
    fileNameOutGraySort=`echo $1 | sed -e 's/'.bam$'/outGraySort/'`
    fileNameOutSamtools=`echo $1 | sed -e 's/'.bam$'/outSamtools/'`    
    
    ./bamex -o $fileNameOutGraySort.bam -i $1 --in-format bam --out-format bam
    samtools sort -o $fileNameOutSamtools.bam $fileNameOutGraySort.bam
    checkFile $fileNameOutGraySort.bam $fileNameOutSamtools.bam

    cleaner $fileNameOutSamtools $fileNameOutGraySort
}

cd ..
cd ..

make bamex
processFile $1
