#!/bin/bash
declare -i nMeas=10
declare -i L=$1
Tmin=1.5
dT=0.1
Tmax=$(bc<<<"$Tmin+$dT*$nMeas")
declare -i MCS=1000000

for ((c = 0;c<$nMeas;c=c+1)); 
do
    T=$(bc<<<"$Tmin+$c*$dT")
    ./isingWolff2D $L $T $MCS &
    echo -------------------$L $T $MCS ----------------------
done;
