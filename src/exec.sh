#!/bin/bash


start=$SECONDS

echo "Running for several refractive index."

rm FiltroOutPut.txt

for ((i=10;i<=10;i=i+10)); do
    start_i=$SECONDS

    echo "Running $(printf '0.%d' $i)"
    ./filtro filtro.in $(printf '0.%d' $i) >/dev/null
    root -q -b -l 'signal.C('$(printf '0.%d' $i)')'
    duration=$(( SECONDS - start_i ))
    echo "Completed in $(printf '%d' $duration) seconds."
done

duration=$(( SECONDS - start ))

if (( $duration >= 3600 )) ; then
    let "hours=SECONDS/3600"
    let "minutes=(SECONDS%3600)/60"
    let "seconds=(SECONDS%3600)%60"
    echo "Completed in $hours hour(s), $minutes minute(s) and $seconds second(s)" 
elif (( $duration >= 60 )) ; then
    let "minutes=(SECONDS%3600)/60"
    let "seconds=(SECONDS%3600)%60"
    echo "Completed in $minutes minute(s) and $seconds second(s)"
else
    echo "Completed in $SECONDS seconds"
fi
