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


1. PROGRAM INSTALLATION
The instructions to install required programs were provided by Aram and are replicated here below with patches:

CMSSW Setup:
export SCRAM_ARCH=slc6_amd64_gcc491
cmsrel CMSSW_7_6_3_patch2
cd CMSSW_7_6_3_patch2/src/
cmsenv

After Installing CMSSW install all other programs in the CMSSW/src folder

Whizard Setup
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


Pythia Setup
-Download the latest pythia8 and follow the instructions
-http://home.thep.lu.se/~torbjorn/Pythia.html
tar xvfz pythia8219.tgz
cd pythia8219
./configure --with-hepmc2=/afs/cern.ch/cms/external/lcg/external/HepMC/2.06.08/x86_64-slc6-gcc48-opt
make

Delphes Setup:
git clone https://github.com/delphes/delphes.git
cd delphes
make

Cross-Section Uncertainty (HiggsAnalysis/CombinedLimit) setup 
You can find the fitting framework we use in CMS (also used for the Higgs discovery) here 
https://twiki.cern.ch/twiki/bin/viewauth/CMS/SWGuideHiggsAnalysisCombinedLimit
You can set it up as follows:
export SCRAM_ARCH=slc6_amd64_gcc481
scram project CMSSW_7_1_22
cd CMSSW_7_1_22/src/
cmsenv
git clone https://github.com/cms-analysis/HiggsAnalysis-CombinedLimit.git HiggsAnalysis/CombinedLimit
scram b

Higgs Coupling Uncertainty (tlep-coupling) setup
Once we successfully perform the steps above and obtain the expected uncertainty on the zh cross section we can translate this to a statement about the higgs couplings.
The fitting code is here:
git clone https://github.com/arapyan/tlep-couplings.git
Follow the instructions in the 'README.md'. 
In runTLEP_250_350_Standalone_Floating.py you can find the list of the constraints. 
For more details you can take a look in this paper:
https://arxiv.org/abs/1308.6176



2. GENERATING SAMPLES
From the individual steps of sample generation a bash script was compiled to expedite the process

To set up:
git clone https://github.com/aandriat/MIT-FCCee.git

Change the paths of your directories to match your installation configuration, then
bash install/setup.sh

The sripts folder (visible after setup) contains:
generate.sh - A bash script to generate a delphes sample based on a given physics process. Can quickly change whizard_card name, center of mass energy, and number of samples
process.sh - A bash sript which runs hist_process.C and hist_draw.C, macros which take a collection of delphes samples, construct histograms of reconstructed and missing mass, and save .pdfs
coupling.sh - A bash sript which calculates cross section uncertainty of a signal by fitting it against backgrounds, and uses this to calcualte the uncertainty of Higgs-particle couplings and higgs width. Saves respective .pdfs

The input folder (visible after setup) contains:

whizard_card.sin - A sample input whizard card. To create your custom sample, modify as necessary but leave the dummy variables as they are. Save your custom card as myprocess.sin. In generate.sh set process=myprocess
main41.cc - An input to pythia specifying how the output from whizard decays. For your custom samples, change the physics, but leave dummy variables as they are - generate.sh will take care of them.
delphes_card_ILD.tcl - An input to delphes simulating a detector which takes as input the particles created by pythia. For custom samples, change its properties. No dummy variables.

hist_process.C - A macro that takes multiple delphes samples and creates histogram files by reconstructing system mass and missing mass, subject to cuts
hist_draw.C - A macro which takes the histogram root file and makes the histograms pretty
hist_functions.h - A helper file for hist_process.C defining how to calculate and create histograms and what cuts to apply
rootlogon.C - Specifies where to find the delphes libraries necessary for working with delphes sampels

xsection_tool.txt - Input card into the HiggsAnalysis tool which speicifes which signal and background samples to use to calculate a signal cross-section uncertainty
xstat_reader.C - Reads the calculated uncertainty and passes it to the coupling card
runTLEP_250_350_Standalone_Floating.py - Input to the couplign calculation tool which defines which processes are used for the coupling uncertainty and their respective uncertainties. Saves pdf of coupling uncertainty and higgs width



3. RUNNING SCRIPTS
Assuming everything has been correctly defined and you have modified all relevant files to reflect your physics simulation and processes:
1. Create whizard cards for each sample in your simulation.
2. bash scripts/generate.sh for each sample.
3. Copy cross section information corresponding to each sample from cross_sections.txt into hist_process.C
4. bash scripts/process.sh
5. bash scripts/coupling.sh

Voila! In the following folders you will now find the following:
whizard - Contains the .lhe file of each whizard generated sample
pythia - Containts the .dat file of each pythia processed sample
delphes - Contains the .root file that contains the delphes output for each sample
analysis - Contains the _massHIST.root and _recoilmassHIST.root files which contain all the histograms for your samples as well as process.pdf version of the histograms
xsection - Contains information on the cross section uncertainty of your sample
coupling - Contains graph of uncertainty in various higgs couplings and the calculated higgs width
