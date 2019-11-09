#!/bin/sh

cd ..
cd ..

make clean
make distclean
make mpiio

echo "[MPI_IO]"
time mpirun mpiio $1 $2

inpHash=`echo $(cksum $1) | awk '{print $1 " " $2}'`
    outpHash=`echo $(cksum $2) | awk '{print $1 " " $2}'`

    if [ "$inpHash" = "$outpHash" ]
    then
        echo "\033[42m[SUCCES] I/O is correct \033[0m"
    else
        echo "\033[41m[FAILED] I/O is not correct \033[0m"
    fi

rm $2
echo "[SAMTOOLS]"
time samtools view -@ 4 -h $1 -o $2
rm $2