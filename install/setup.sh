#!/bin/bash

# This bash script sets up the automated sample generation, processing, and analysis to work with your install directories. Modify paths and run only once.

echo "Setting up your sample generation"

# Change these paths to match the configuration of your install directories
folder=/afs/cern.ch/work/a/aandriat/public/test/MIT-FCCee # Full path of the MIT-FCCee folder
src=/afs/cern.ch/user/a/aandriat/CMSSW_7_6_3_patch2/src # Full path of the src folder in your CMSSW directory 
whizard=whizard/build # Location of whizard directory in src folder
pythia=pythia/pythia8212 # Location of pythia dirctory in src folder
delphes=delphes # Location of delpges in src folder
coupling=tlep-couplings # Location of tlep-couplings in src folder
higgsanalysis=HiggsAnalysis/CombinedLimit # Location of CombinedLimit in src folder

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
cp install/process.sh scripts
cp install/coupling.sh scripts

# Copies all files into input folder
cp install/hist_process.C input
cp install/hist_draw.C input
cp install/hist_functions.h input
cp install/rootlogon.C input
cp install/delphes_card_ILD.tcl input
cp install/main41.cc input
cp install/runTLEP_250_350_Standalone_Floating.py input
cp install/whizard_card.sin input
cp install/xstat_reader.C input
cp install/xsection_tool.txt input

# Replaces the necessary common variables 
cd scripts
sed -i 's:GENERATIONPATH:'$folder':g'  generate.sh
sed -i 's:SRCPATH:'$src':g'  generate.sh
sed -i 's:WHIZARDPATH:'$whizard':g'  generate.sh
sed -i 's:PYTHIAPATH:'$pythia':g'  generate.sh
sed -i 's:DELPHESPATH:'$delphes':g'  generate.sh

sed -i 's:GENERATIONPATH:'$folder':g'  process.sh

sed -i 's:GENERATIONPATH:'$folder':g'  coupling.sh
sed -i 's:SRCPATH:'$src':g'  coupling.sh
sed -i 's:COUPLINGPATH:'$coupling':g'  coupling.sh
sed -i 's:HIGGSANALYSISPATH:'$higgsanalysis':g'  coupling.sh
back

cd input
sed -i 's:GENERATIONPATH:'$folder':g'  xsection_tool.txt

sed -i 's:GENERATIONPATH:'$folder':g'  hist_process.C

sed -i 's:SRCPATH:'$src':g'  rootlogon.C
sed -i 's:DELPHESPATH:'$delphes':g'  rootlogon.C
back

echo "Done"
