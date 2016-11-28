#!/bin/bash

# This bash script runs parameter_tree and hist_draw together in an n-1 sample cutting configuration. Before using, modify parameter_tree to work with the samples you have generated with your prefered cuts and filename, and modigy hist_draw to match

folder=GENERATIONPATH  # Location of generation directory - do not change after running setup.sh
src=SRCPATH

function back {
    cd $folder
}
back

echo "Processing Delphes Samples into Parameter Trees"
mkdir -p samples
back

echo "This copies necessary files"
cp input/rootlogon.C samples/
cp input/parameter_functions.h samples/
cp input/parameter_tree.C samples/
back

cd samples

echo "Starting parameter_tree"
root -l parameter_tree.C+ -q
back

echo "Done"


