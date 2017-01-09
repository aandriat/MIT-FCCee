#!/bin/bash

# This bash script takes a whizard card called somename.sin which defines a physics simulation and using whizard, pythia, and delphes creates a simulated delphes sample called somename.root
# Relies on cmsenv, prior installation of whizard, pythia, and delphes, and properly configured cards somename.sin, main41.cc, and delphes_card_ILD.tcl

folder=GENERATIONPATH #DDD #Do not chage after running setup.sh
src=SRCPATH #Do not change after running setup.sh
whizard=WHIZARDPATH #Do not change after running setup.sh
pythia=PYTHIAPATH #Do not change after running setup.sh
delphes=DELPHESPATH #Do not change after running setup.sh

process=somename #AAA
cme=350  #BBB
numberevents=100 #CCC

#Initializes Cross Section. This is not the final value!
#Do not change
xsec=1.0 #Actual value will be read from .lhe file and printed to terminal
xfilename=delphes/cross_sections.txt #Defines the place to store cross section info


function back {
    cd $folder
}

function whizstart {
    source $src/$whizard/bin/whizard-setup.sh
}

back

echo "The shell started "

echo "This makes directories"
mkdir -p whizard/gen
mkdir -p pythia
mkdir -p delphes
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

echo "This will remove the junk gen folder"
rm -r -f whizard/gen
back


echo "This will copy pythia card into pythia"
cp input/main41.cc $src/$pythia/examples
back

echo "This defines dummy variables in pythia card"
sed -i 's:AAA:'$process':g'  $src/$pythia/examples/main41.cc
sed -i 's:BBB:'$cme':g'  $src/$pythia/examples/main41.cc
sed -i 's:CCC:'$numberevents':g'  $src/$pythia/examples/main41.cc
sed -i 's:DDD:'$folder':g'  $src/$pythia/examples/main41.cc

echo "This will cd into pythia"
cd $src/$pythia/examples
make main41
./main41
back

echo "This will copy delphes card into its location"
cp input/delphes_card_ILD.tcl $src/$delphes/cards
back

echo "This will remove the old delphes sample if it exists"
rm delphes/$process.root
back

echo "This will cd into delphes and make sample"
cd $src/$delphes
./DelphesHepMC cards/delphes_card_ILD.tcl $folder/delphes/$process.root $folder/pythia/$process.dat
back

echo "The calculated cross section for process $process is: $xsec"
echo "( $process, $xsec)" >> $xfilename
back
echo "Done"
