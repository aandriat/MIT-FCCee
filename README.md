# MIT-FCCee

Welcome to the MIT-FCCee GitHub Repository!

Current contributors are:
Alexander Andriatis
Joseph Curti
Kevin Tang
Molly Kaplan
Andres Rios

The current functionality of this repository is sample generation using whizard,pythia,and delphes for e+e- simulation, histogram drawing, cross section and coupling uncertainties


--SAMPLE GENERATION INSTRUCTIONS--

An initial sample generation procedure was provided by Aram Apyan, which installs Whizard, Pythia, and Delphes.
The sample generation procedure involved many directory changes and individual steps, so a streamlining process was called for.

This sample generation method is a one-step bash script from whizard card to delphes sample, with quick turaround time for modifications in center of mass energy and number of events, and is designed to be shared flexibly among users. 


#1. PROGRAM INSTALLATION
The instructions to install required programs were provided by Aram and are replicated here below with patches:

#CMSSW Setup:
export SCRAM_ARCH=slc6_amd64_gcc491
cmsrel CMSSW_7_6_3_patch2
cd CMSSW_7_6_3_patch2/src/
cmsenv

After Installing CMSSW install all other programs in the CMSSW/src folder

#Delphes Setup:
git clone https://github.com/delphes/delphes.git
cd delphes
make

#Whizard Setup
-Download the latest version from https://whizard.hepforge.org/
-Also the manual here is ueful: https://whizard.hepforge.org/manual.pdf
-One warning is that you need ocaml [https://ocaml.org] language and it is not available on lxplus. You can either install it yourself or use my installation here
-/afs/cern.ch/work/a/arapyan/public/ocaml
export PATH=/afs/cern.ch/work/a/arapyan/public/ocaml/ocaml-4.02.3/bin:$PATH
mkdir whizard
your-src-directory/configure --prefix=whizard
make
make check
make install


#Pythia Setup
-Download the latest pythia8 and follow the instructions
-http://home.thep.lu.se/~torbjorn/Pythia.html
tar xvfz pythia8219.tgz
cd pythia8219
./configure --with-hepmc2=/afs/cern.ch/cms/external/lcg/external/HepMC/2.06.08/x86_64-slc6-gcc48-opt
make

#2. GENERATING SAMPLES
From the individual steps of sample generation a bash script was compiled to expedite the process

git clone https://github.com/aandriat/MIT-FCCee.git
cd input
bash generate.sh

The input folder contains:
generate.sh - A bash script which allows the definition of basic sample parameters, such as center of mass energy and number of samples, as well as the working directory and install directory. Changes the relevant parameters in the three sample cards throughout each sample, specified by dummy variables (AAA)(BBB).... Runs commands to generate samples.

whizard_card.sin - An input to whizard specifying the physics of the simulation. To create your custom sample, modify as necessary but leave the dummy variables as they are. Save your custom card as myprocess.sin. In generate.sh set process=myprocess

main41.cc - An input to pythia specifying how the output from whizard decays. For your custom samples, change the physics, but leave dummy variables as they are - generate.sh will take care of them.

delphes_card_ILD.tcl - An input to delphes simulating a detector which takes as input the particles created by pythia. For custom samples, change its properties. No dummy variables.


#3. ANALYZING SAMPLES
The generate.sh process will save samples as delphes .root files in the output folder. Use these as an imput to sample analysis. Also saves cross sections for each process in cross_sections.txt

In analysis there are four files:
rootlogon.C specifies the location of the delphes installation - change this once when starting to use the programs
hist_process.C takes the delphes samples in the output folder, subjects them to various cuts and calcualtions, and outputs histograms of the leading di-particle reconstructed mass and recoil mass.
hist_functions.C is a helper file to histprocess, cointaing the cuts and calculations

#4. MAKING HISTOGRAMS
hist_draw.C includes many stylistic choices for attractive histogram generation and should serve as a platform for case-by-case drawing needs

#5. CROSS SECTION AND COUPLING UNCERTAINTY
xsection.sh and coupling.sh are simple commands to save cd time between sample and process. Hopefully will become more automated with future updates
