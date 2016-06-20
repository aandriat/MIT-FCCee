#!/bin/bash

clear

process=eebb_uu #AAA
cme=350 #BBB
numberevents=1000 #CCC
folder=/afs/cern.ch/work/a/aandriat/public/autogen #DDD #Change this to match where you placed the input folder
src=/afs/cern.ch/user/a/aandriat/CMSSW_7_6_3_patch2/src #Change this to match your install directory configuration and wherever $src is used throughout the file


#Initializes Cross Section. This is not the final value!
#Do not change
xsec = -1.0 #Actual value will be read from .lhe file and printed to terminal

function back {
    cd $folder
}

function whizstart {
    source $src/whizard/build/bin/whizard-setup.sh
}

back

echo "The shell started "

echo "This makes directories"
mkdir -p whizard/gen
mkdir -p pythia
mkdir -p delphes
mkdir -p output
back

echo "This copies whizard card into the work whizard location"
cp input/$process.sin whizard/gen
back

echo "This defines the dummy varibales in whizard card"
sed -i 's:AAA:'$process':g'  whizard/gen/$process.sin
sed -i 's:BBB:'$cme':g'  whizard/gen/$process.sin
sed -i 's:CCC:'$numberevents':g'  whizard/gen/$process.sin

echo "This cds into the gen whizard location and generates whizard sample"
cd whizard/gen
whizstart
whizard $process.sin
echo "Done generating"

echo "Reading calculated cross section"
filename=$process.lhe
num=0
while read -r line
do
    num=$((num+1))
    IN="$line"
    IFS='"'; arrIN=($IN); unset IFS
    if [ "$num" -eq "8" ]; then       
            # echo "${arrIN[3]}"
            xsec=${arrIN[3]}
            break
    fi
done < $filename
back

echo "This will move generated .lhe file into whizard location"
mv whizard/gen/$process.lhe whizard
back

echo "This will copy pythia card into pythia"
cp input/main41.cc $src/pythia/pythia8212/examples
back

echo "This defines dummy variables in pythia card"
sed -i 's:AAA:'$process':g'  $src/pythia/pythia8212/examples/main41.cc
sed -i 's:BBB:'$cme':g'  $src/pythia/pythia8212/examples/main41.cc
sed -i 's:CCC:'$numberevents':g'  $src/pythia/pythia8212/examples/main41.cc
sed -i 's:DDD:'$folder':g'  $src/pythia/pythia8212/examples/main41.cc

echo "This will cd into pythia"
cd $src/pythia/pythia8212/examples
make main41
./main41
back

echo "This will copy delphes card into its location"
cp input/delphes_card_ILD.tcl $src/delphes/cards
back

echo "This will cd into delphes and make sample"
cd $src/delphes
./DelphesHepMC cards/delphes_card_ILD.tcl $folder/delphes/$process.root $folder/pythia/$process.dat
back

echo "This deletes the old root file if it exists"
rm output/$process.root
back

echo "This copies the delphes sample into folder output"
cp delphes/$process.root output
back

echo "This will delete all intermediate directories"
rm -r -f whizard
rm -r -f pythia
rm -r -f delphes

echo ""
echo "The calculated cross section is: $xsec"
back