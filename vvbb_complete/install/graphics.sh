#!/bin/bash

# This bash script runs the tree plotter

folder=GENERATIONPATH  # Location of generation directory - do not change after running setup.sh

function back {
    cd $folder
}
back

mkdir -p graphics

cp input/HttStyles.cc graphics
cp input/HttStyles.h graphics
cp input/pretty_draw.C graphics
cp input/rootlogon.C graphics

echo "Starting pretty plotter"
cd graphics
root -l pretty_draw.C+ -q
back

echo "Done"


