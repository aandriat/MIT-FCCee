#!/bin/bash

# This bash script runs tree_hist and hist_draw together in an n-1 sample cutting configuration. Before using, modify tree_hist to work with the samples you have generated with your prefered cuts and filename, and modigy hist_draw to match
#  

folder=GENERATIONPATH  # Location of generation directory - do not change after running setup.sh
src=SRCPATH
higgsanalysis=HIGGSANALYSISPATH

function back {
    cd $folder
}
back

echo "Starting n-1 analysis"
echo "This makes analysis and xsection directory"
rm -r analysis
rm -r xsection
rm $src/$higgsanalysis/output.txt
mkdir -p analysis
mkdir -p xsection
back

echo "This copies necessary files"
cp input/tree_hist.C analysis/
cp input/hist_draw.C analysis/
cp input/xsection_tool.txt xsection/
back

cd analysis
echo "Starting tree_hist"
root -l tree_hist.C+ -q
echo "Starting hist_draw"
root -l hist_draw.C+ -q
back


echo "Starting HiggsAnalysis"
cd $src/$higgsanalysis
combine -M MaxLikelihoodFit --robustFit 1 --rMax 3 --expectSignal=1 --saveNormalizations -t -1 $folder/xsection/xsection_tool.txt > output.txt
cp higgsCombineTest.MaxLikelihoodFit.mH120.root $folder/xsection/
cp output.txt $folder/xsection/

mv mlfit.root mlfitm.root
python test/mlfitNormsToText.py mlfitm.root 
python diffNuisances.py -A -a  -f text mlfitm.root > mlfitm.txt
cp mlfitm.txt $folder/xsection/
back

echo "Done"



