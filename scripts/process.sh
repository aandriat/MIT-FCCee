#!/bin/bash

# This bash script runs hist_process and hist_draw together. Before using, modify hist_process to work with the samples you have generated with your prefered cuts and filename, and modigy hist_draw to match

folder=/afs/cern.ch/work/a/aandriat/public/test/MIT-FCCee # Location of generation directory - do not change after running setup.sh

function back {
    cd $folder
}
back

echo "This makes analysis directory"
mkdir -p analysis
back

echo "This copies necessary files"
cp input/rootlogon.C analysis/
cp input/hist_functions.h analysis/
cp input/hist_process.C analysis/
cp input/hist_draw.C analysis/
back

cd analysis
echo "Starting hist_process"
root -l hist_process.C+ -q
echo "Starting hist_draw"
root -l hist_draw.C+ -q
back
