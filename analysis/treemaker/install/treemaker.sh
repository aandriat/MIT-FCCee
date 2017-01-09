#!/bin/bash

# This bash script runs parameter_tree and hist_draw together in an n-1 sample cutting configuration. Before using, modify parameter_tree to work with the samples you have generated with your prefered cuts and filename, and modigy hist_draw to match

folder=GENERATIONPATH  # Location of generation directory - do not change after running setup.sh
src=SRCPATH

function back {
    cd $folder
}
back

echo "Processing Delphes Samples into Parameter Trees"
echo "This makes analysis directory"
rm -rf analysis
mkdir -p analysis
back

echo "This copies necessary files"
cp input/rootlogon.C analysis/
cp input/parameter_functions.h analysis/
cp input/parameter_tree.C analysis/
back

cd analysis

echo "Starting parameter_tree"
root -l parameter_tree.C+ -q
back

echo "Done"
echo
echo
echo



