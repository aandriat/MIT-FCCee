#!/bin/bash

# This bash script runs jet resolution script

folder=GENERATIONPATH  # Location of generation directory - do not change after running setup.sh

function back {
    cd $folder
}
back

echo "Jet Resolution"
rm -rf resolution
mkdir -p resolution
back

echo "This copies necessary files"
cp input/rootlogon.C resolution/
cp input/jet_resolution.C resolution/
back

cd resolution

echo "Starting parameter_tree"
root -l jet_resolution.C+ -q
back

echo "Done"
echo
echo
echo



