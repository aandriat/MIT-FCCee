#!/bin/bash
#Joseph Curti 7/14/16
#
#Top level script for parallel sample generation using Whizard to Pythia to Delphes
#Job level script is runjobs.sh
#
#Produces numJobs number of samples with perJob number of events each. These are outputted to the output
#	directory in a folder with the name of the process specified in runjobs.sh i.e. $outputDir/$process
#Note the root files are not merged by these scripts. You can use the hadd command from the terminal to merge
#	the individual samples to a single root file after completion.
#
#Specify the total number of events here as well as the events per job
#Specify the process name and the center of mass energy here
#Specify the queue your jobs will be submitted to (-q option of the bsub line) (default: 2 days)
#
#Note directory paths (outputDir and script) must be changed to match your installations
#	and setup.
#
#Once your jobs have all been submitted you can disconnect from lxplus and your jobs will still complete and
#	process normally
#
#This can be run from anywhere (so long as it is in a folder with the rest of the inputs) 
#	and output will still direct to the output directory specified.
#Logs are stored in the folder $outputDir/logs/$process which is made by this script if it does not exist
#
#Different processes with different names can be run simultaneously and will not interfere.
#Warning: running the same process (same name) while jobs are still running will result in interference!!!

maxEvents=70
perJob=10
process=eeZZ #AAA
cme=240 #BBB

numJobs=maxEvents/perJob

outputDir=/afs/cern.ch/work/j/jcurti/public/parallel_sample_gen/output #change to your desired output location

#note workDir here in submitjobs.sh becomes scramdir in runjobs.sh!
#Do not need to change. Automatically gets CMSSW
workDir=$CMSSW_BASE            
echo $workDir
mkdir -p $outputDir/logs #makes log directory if doesn't already exist
mkdir -p $outputDir/logs/$process #makes a specific subdirectory for this process' jobs
rm -r $outputDir/logs/$process/* #empties the log directory for this process if already exists

#change to the location of your runjobs.sh
#Recommended to use full path to runjobs.sh
script=/afs/cern.ch/work/j/jcurti/public/parallel_sample_gen/input/runjobs.sh 

if [ ! "$CMSSW_BASE" ]; then
  echo "-------> error: define cms environment."
  exit 1
fi

#Job submission loop
val=1
while((val<=numJobs)); do
    echo $val
    bsub -o $outputDir/logs/$process/out.%J -q 2nd $script $outputDir $workDir $val $perJob $process $cme
    val=$(($val+1))
done

exit 0

