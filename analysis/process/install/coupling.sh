#!/bin/bash


# This bash script takes a collection of root histograms in somename.root which have been specified in cross-section card xsection_tool.txt
# Calculates the uncertainty on the signal cross section and inputs this value into a tool which calculates various higgs couplings and higgs width
# Relies on prior installation of HiggsAnalysis/CombinedLimit and tlep-coupling and properly configured cards xsection_tool.txt, xstat_reader.C, and runTLEP_250_350_Standalone_Floating.py

folder=GENERATIONPATH
src=SRCPATH
coupling=COUPLINGPATH
higgsanalysis=HIGGSANALYSISPATH

function back {
    cd $folder
}

back
echo "This makes directories"
mkdir -p coupling
back

uncertainty=0.023 #XXX this changes the coupling uncertainty in the python coupling file

echo "Starting Coupling"
cp input/runTLEP_250_350_Standalone_Floating.py $src/$coupling/
cd $src/$coupling

echo "This defines the dummy varibales in coupling card"
sed -i 's:XXX:'$uncertainty':g'  runTLEP_250_350_Standalone_Floating.py

echo "This runs the coupling file"
python -i runTLEP_250_350_Standalone_Floating.py

echo "This will copy the produced graphs into coupling"
cp coupling_uncertainty.pdf $folder/coupling/
cp higgs_width.pdf $folder/coupling/


