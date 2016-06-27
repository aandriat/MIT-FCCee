#!/bin/bash


# This bash script takes a collection of root histograms in somename.root which have been specified in cross-section card xsection_tool.txt
# Calculates the uncertainty on the signal cross section and inputs this value into a tool which calculates various higgs couplings and higgs width
# Relies on prior installation of HiggsAnalysis/CombinedLimit and tlep-coupling and properly configured cards xsection_tool.txt, xstat_reader.C, and runTLEP_250_350_Standalone_Floating.py

folder=/afs/cern.ch/work/a/aandriat/public/test/MIT-FCCee
src=/afs/cern.ch/user/a/aandriat/CMSSW_7_6_3_patch2/src
coupling=tlep-couplings
higgsanalysis=HiggsAnalysis/CombinedLimit

function back {
    cd $folder
}

back
echo "This makes directories"
mkdir -p xsection
mkdir -p coupling
back


echo "Starting HiggsAnalysis"
cd $src/$higgsanalysis
combine -M MaxLikelihoodFit --robustFit 1 --expectSignal=1 -t -1 $folder/input/xsection_tool.txt
cp higgsCombineTest.MaxLikelihoodFit.mH120.root $folder/xsection/
back

cp input/xstat_reader.C xsection/
cd xsection
root -l xstat_reader.C+ -q
back

filename="xsection/uncertainty.txt"
uncertainty=0 #XXX this changes the coupling uncertainty in the python coupling file
while read line
do
    uncertainty="$line"
    echo "Cross section uncertainty read from file - $uncertainty"
done < "$filename"
back

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


