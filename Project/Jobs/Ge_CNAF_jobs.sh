#!/bin/bash

# reSub=(1 51 101 151 )
for((Target=1;Target<5101;Target+=100))
do
    echo dealing...Files begin from:$Target 
    sed "s/START/$Target/g"  $1.sh > $1$Target.sh
    chmod 774 $1$Target.sh
    bsub -q juno -o ${PWD}/logs/NEPTE_$Target.log -hl -M 4000000 ${PWD}/$1$Target.sh
    # echo ${PWD}/$1$Target.sh
done
# bsub -q juno -o myo3.log ${PWD}/CNAF_Atm.sh

#end