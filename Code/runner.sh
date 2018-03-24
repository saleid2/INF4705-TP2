#!/bin/bash
echo "algo,fichier,temps,hauteur" > ./results.csv

for algo in {"vorace","vorace","vorace","vorace","vorace","vorace","vorace","vorace","vorace","vorace","progdyn","tabou"}; do
    for ex in $(ls dataset); do
        t=$(./tp.sh -e dataset/${ex} -a $algo -t)
        echo $algo,$ex,$t
    done
done >> results.csv