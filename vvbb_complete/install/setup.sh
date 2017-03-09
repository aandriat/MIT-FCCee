#!/bin/bash

# This bash script sets up the automated sample generation, processing, and analysis to work with your install directories. Modify paths and run only once.

echo "Setting up your sample analysis"

PWD=$(pwd)
folder=$PWD # Full path of the MIT-FCCee folderr
src=/afs/cern.ch/user/a/aandriat/CMSSW_7_6_3_patch2/src # Full path of the src folder in your CMSSW directory
samples=$folder/samples/final/
whizard=whizard/build # Location of whizard directory in src folder
pythia=pythia/pythia8212 # Location of pythia dirctory in src folder
delphes=delphes # Location of delpges in src folder
coupling=tlep-couplings # Location of tlep-couplings in src folder
higgsanalysis=HiggsAnalysis/CombinedLimit # Location of CombinedLimit in src folder
treepath=$folder/trees

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
cp install/process.sh scripts
cp install/graphics.sh scripts
cp install/n-1.sh scripts
cp install/treecutter.sh scripts
cp install/inverse_n-1.sh scripts
cp install/jet_resolution.sh scripts

# Copies all files into input folder
cp install/hist_draw.C input
cp install/HttStyles.cc input
cp install/HttStyles.h input
cp install/parameter_functions.h input
cp install/parameter_tree.C input
cp install/pretty_draw.C input
cp install/rootlogon.C input
cp install/runTLEP_250_350_Standalone_Floating.py input
cp install/tree_hist.C input
cp install/xsection_tool.txt input
cp install/xstat_reader.C input
cp install/n1_tree_hist.C input
cp install/n1_draw.C input
cp install/treecutter_tree.C input
cp install/jet_resolution.C input

# Replaces the necessary common variables 
cd scripts
sed -i 's:GENERATIONPATH:'$folder':g'  treemaker.sh
sed -i 's:SRCPATH:'$src':g'  treemaker.sh

sed -i 's:GENERATIONPATH:'$folder':g'  process.sh
sed -i 's:SRCPATH:'$src':g'  process.sh
sed -i 's:HIGGSANALYSISPATH:'$higgsanalysis':g'  process.sh

sed -i 's:GENERATIONPATH:'$folder':g'  graphics.sh

sed -i 's:GENERATIONPATH:'$folder':g' n-1.sh

sed -i 's:GENERATIONPATH:'$folder':g'  treecutter.sh
sed -i 's:SRCPATH:'$src':g'  treecutter.sh

sed -i 's:GENERATIONPATH:'$folder':g'  jet_resolution.sh

back

cd input
sed -i 's:GENERATIONPATH:'$folder':g'  parameter_tree.C
sed -i 's:SAMPLEPATH:'$samples':g' parameter_tree.C

sed -i 's:SRCPATH:'$src':g'  rootlogon.C
sed -i 's:DELPHESPATH:'$delphes':g'  rootlogon.C

sed -i 's:GENERATIONPATH:'$folder':g'  xsection_tool.txt

sed -i 's:GENERATIONPATH:'$folder':g'  tree_hist.C
sed -i 's:TREEPATH:'$treepath':g'  tree_hist.C


sed -i 's:SRCPATH:'$src':g' rootlogon.C
sed -i 's:DELPHESPATH:'$delphes':g' rootlogon.C

sed -i 's:GENERATIONPATH:'$folder':g'  pretty_draw.C

sed -i 's:TREEPATH:'$treepath':g' n1_tree_hist.C

sed -i 's:GENERATIONPATH:'$folder':g'  treecutter_tree.C
sed -i 's:SAMPLEPATH:'$samples':g' treecutter_tree.C
sed -i 's:TREEPATH:'$treepath':g' treecutter_tree.C

sed -i 's:TREEPATH:'$treepath':g' jet_resolution.C
sed -i 's:SAMPLEPATH:'$samples':g' jet_resolution.C

back

echo "Done"
