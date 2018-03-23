#!/bin/bash
echo "algo,fichier,temps" > ./results.csv

for algo in {"progdyn","tabou","vorace","vorace","vorace","vorace","vorace","vorace","vorace","vorace","vorace","vorace"}; do
    for ex in $(ls dataset); do
        t=$(./tp.sh -e dataset/${ex} -a $algo -t)
        echo $algo,$ex,$t
    done
done >> results.csv