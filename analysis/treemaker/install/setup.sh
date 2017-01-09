#!/bin/bash

# This bash script sets up the automated sample generation, processing, and analysis to work with your install directories. Modify paths and run only once.

echo "Setting up your sample generation"

PWD=$(pwd)
folder=$PWD # Full path of the MIT-FCCee folder
src=/afs/cern.ch/user/a/aandriat/CMSSW_7_6_3/src # Full path of the src folder in your CMSSW directory 
samples=/afs/cern.ch/work/a/aandriat/public/MIT-FCCee/samples/delphes
delphes=Delphes

# Defines a function to return to top of MIT-FCCee folder
function back {
    cd $folder
}
back

# Folder to contain input cards
mkdir -p input

#Folder to contain bash scripts
mkdir -p scripts

# Copies bash scripts to scripts
cp install/treemaker.sh scripts


# Copies all files into input folder
cp install/parameter_tree.C input
cp install/parameter_functions.h input
cp install/rootlogon.C input

# Replaces the necessary common variables 
cd scripts
sed -i 's:GENERATIONPATH:'$folder':g'  treemaker.sh
sed -i 's:SRCPATH:'$src':g'  treemaker.sh

back

cd input
sed -i 's:GENERATIONPATH:'$folder':g'  parameter_tree.C
sed -i 's:SAMPLEPATH:'$samples':g' parameter_tree.C

sed -i 's:SRCPATH:'$src':g'  rootlogon.C
sed -i 's:DELPHESPATH:'$delphes':g'  rootlogon.C

back

echo "Done"
