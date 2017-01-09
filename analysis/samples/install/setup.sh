#!/bin/bash

# This bash script sets up the automated sample generation, processing, and analysis to work with your install directories. Modify paths and run only once.

echo "Setting up your sample generation"

# Change these paths to match the configuration of your install directories
PWD=$(pwd)
folder=$PWD # Full path of the MIT-FCCee folder
src=/afs/cern.ch/user/a/aandriat/CMSSW_7_6_3/src # Full path of the src folder in your CMSSW directory 
whizard=whizard # Location of whizard directory in src folder
pythia=pythia8215 # Location of pythia dirctory in src folder
delphes=Delphes # Location of delpges in src folder

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
cp install/generate.sh scripts

# Copies all files into input folder
cp install/delphes_card_ILD.tcl input
cp install/main41.cc input

# Replaces the necessary common variables 
cd scripts
sed -i 's:GENERATIONPATH:'$folder':g'  generate.sh
sed -i 's:SRCPATH:'$src':g'  generate.sh
sed -i 's:WHIZARDPATH:'$whizard':g'  generate.sh
sed -i 's:PYTHIAPATH:'$pythia':g'  generate.sh
sed -i 's:DELPHESPATH:'$delphes':g'  generate.sh
back

echo "Done"
