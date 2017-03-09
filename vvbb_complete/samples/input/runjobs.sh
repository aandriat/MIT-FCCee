#!/bin/bash 
#Joseph Curti 7/14/16
#
#Job level script for use in parallel sample generation
#Top level script is submitjobs.sh
#
#Once directories are setup properly, nothing needs to be specified here in runjobs.sh theoretically.
#
#Note directory paths (pythia and delphes) must be changed here to match your installations
#	and setup.
#cp input line must be changed to the path to your input folder containing your delphes card, pythia card, whizard card,
#	runjobs.sh, and submitjobs.sh
#Delphes card must be named delphes_card_ILD.tcl
#Pythia card must be named main41.cc
#Whizard card must be named $process.sin as process is defined in submitjobs.sh

echo "runjobs opened"  
outputDir=$1
scramdir=$2
val=$3

numberevents=$4 #CCC #per job
process=$5 #AAA
cme=$6 #BBB

mkdir $outputDir/$process
outputDir=$outputDir/$process
#All produced root samples go here ^ as well as the cross section information

#ulimit -v 3000000
workDir=`pwd`
echo `hostname`
echo "workDir: $workDir"
echo "scramdir: $scramdir"
echo "args:    $*"

echo "before CD step"
cd ${scramdir}/src
eval `scramv1 runtime -sh`
cd $workDir

#Copies input folder to working directory
#input needs to include a whizard card ($process.sin), a pythia card (main41.cc), a delphes card 
#(delphes_card_ILD.tcl), runjobs.sh, and submitjobs.sh
cp -r /afs/cern.ch/work/a/aandriat/public/eebb/final/350/samples/input . #change this to the path of input folder

xfilename=$outputDir/cross_sections.txt #Defines the place to store cross section info

# #Initializes Cross Section. This is not the final value!
# #Do not change
xsec=-1.0 #Actual value will be read from .lhe file and printed to terminal

function back {
	cd $workDir
}
function whizstart {
	source ${scramdir}/src/whizard/build/bin/whizard-setup.sh
}
back

echo "This makes directories"
mkdir -p whizard
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

echo "done with whizard variable step"

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
			xsec=${arrIN[3]}
			break
	fi
done < $filename
back
echo $xsec

echo "This will move generated .lhe file into whizard location"
mv whizard/gen/$process.lhe whizard
back

#Note this copies your pythia installation to the working directory so this path must be changed
echo "This will copy pythia card into pythia"
cp -r ${scramdir}/src/pythia/pythia8212/ . #change path to match your install!
cp input/main41.cc pythia8212/examples
back

echo "This defines dummy variables in pythia card"
sed -i 's:AAA:'$process':g'  ${workDir}/pythia8212/examples/main41.cc
sed -i 's:BBB:'$cme':g'  ${workDir}/pythia8212/examples/main41.cc
sed -i 's:CCC:'$numberevents':g'  ${workDir}/pythia8212/examples/main41.cc
sed -i 's:DDD:'$workDir':g'  ${workDir}/pythia8212/examples/main41.cc

echo "This will cd into pythia"
cd pythia8212/examples/
make main41
./main41
back

#Note this copies your delphes installation to the working directory so this path must be changed
echo "This will copy delphes card into its location"
cp -r ${scramdir}/src/delphes . #change to match your install!
cp input/delphes_card_ILD.tcl delphes/cards
cd delphes
./DelphesHepMC cards/delphes_card_ILD.tcl $workDir/delphes/$process.root $workDir/pythia/$process.dat
back

status=`echo $?`
echo "Status - $status"

echo "This copies the delphes sample into folder output"
job=_job
mv delphes/$process.root delphes/$process$job${val}.root
cp delphes/$process$job${val}.root $outputDir
back

echo "The calculated cross section for process $process is: $xsec"
echo "( $process, $xsec)" >> $xfilename
back
echo "Done"

exit $status
