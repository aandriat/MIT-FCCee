#!/bin/bash

process=eebb #AAA
folder=/afs/cern.ch/work/a/aandriat/public/autogen #DDD #Change this to match where you placed the input folder
src=/afs/cern.ch/user/a/aandriat/CMSSW_7_6_3_patch2/src #Change this to match your install directory configuration and wherever $src is used throughout the file

#Initializes Cross Section. This is not the final value!
#Do not change
xsec=-1.0 #Actual value will be read from .lhe file and printed to terminal

function back {
    cd $folder
}

echo "Starting HiggsAnalysis"
cd $src/HiggsAnalysis/CombinedLimit
combine -M MaxLikelihoodFit --robustFit 1 --expectSignal=1 -t -1 $folder/input/xsection_tool.txt
back
