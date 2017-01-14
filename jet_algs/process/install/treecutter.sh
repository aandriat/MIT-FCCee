#!/bin/bash

# This bash script runs parameter_tree and hist_draw together in an n-1 sample cutting configuration. Before using, modify parameter_tree to work with the samples you have generated with your prefered cuts and filename, and modigy hist_draw to match

folder=GENERATIONPATH  # Location of generation directory - do not change after running setup.sh
src=SRCPATH

function back {
    cd $folder
}
back

echo "Cutting Trees"
echo "This makes treecutter directory"
rm -rf treecutter
mkdir -p treecutter
back

echo "This copies necessary files"
cp input/rootlogon.C treecutter/
cp input/treecutter_tree.C treecutter/
back

cd treecutter

echo "Starting parameter_tree"
root -l treecutter_tree.C+ -q
back

echo "Done"
echo
echo
echo



