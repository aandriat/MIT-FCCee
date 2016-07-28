    // E+E- Delphes Sample Processing for ee->bbvv final state
// Alexander Andriatis and Joseph Curti
// 20 June 2016

// Takes a collection of delphes samples and creates a single .root collection of histograms as well as a crudely generated histogram collection for initial analysis

#if !defined(__CINT__) || defined(__MAKECINT__)

#include <TROOT.h>              //  -
#include <TSystem.h>            //   |
#include <TFile.h>              //   |
#include <TTree.h>              //   |
#include <TClonesArray.h>       //   |
#include <TChain.h>             //   |
#include <TCanvas.h>            //   |
#include <TStyle.h>             //   |
#include <TLegend.h>            //    -  Include tools for root in general  
#include <TH1.h>                //   |
#include <TF1.h>   
#include <TH2.h>                //   |
#include <iostream>             //   |
#include <fstream>              //   |
#include <cmath>                //   |
#include <fstream>              //   |
#include <TString.h>            //   |
#include <TLorentzVector.h>     //   |
#include <TMath.h>              //  -
#include <TLatex.h> 
#include <TPaveText.h>
#include <cstdio>
#include <ctime>

#include "parameter_functions.h"
#include "modules/Delphes.h"                   //   |
#include "ExRootAnalysis/ExRootTreeReader.h"   //   -  Include needed delphes headers
#include "classes/DelphesClasses.h"            //   |

#endif

using namespace TMath;
using namespace std;

TString root = ".root";


// Takes as input a given process with corresponding luminosity, as well as sample CME, cuts, directory location, and output file name - saves corresponding histogram in root file.
void processsimulation(TString sample, Double_t crosssection, TLorentzVector CME, Double_t luminosity, TString filename, TString directory, std::map<TString, Int_t> cut, std::map<TString, Int_t> particletype, std::clock_t start){
    
    cout << "Processing " << sample << endl;
    // Defines which sample to read from
    TString samplename = directory + sample + root;
    TChain chain("Delphes");
    chain.Add(samplename); 
    ExRootTreeReader *treeReader = new ExRootTreeReader(&chain); // Reads sample from delphes root fiile

    //Branches to be read from the collision file
    std::array<TString, 4> branchnames = {(TString)"Muon",(TString)"Electron",(TString)"Photon",(TString)"Jet"};
                                       // (TString)"Tower",(TString)"Track",(TString)"EFlowTrack",(TString)"EFlowPhoton",(TString)"EFlowNeutralHadron"};
    std::map<TString, TClonesArray*> branches;
    for (int i=0; i<(int)branchnames.size(); i++){
        branches.insert ( std::pair<TString, TClonesArray*> (branchnames[i], treeReader->UseBranch(branchnames[i])));
    }

    // Variables to store particles
    Electron *electron = 0;
    Muon *muon = 0;
    Jet *jet = 0;

    Double_t numberOfEntries = treeReader->GetEntries();
    Double_t norm_const = (crosssection*luminosity)/numberOfEntries; // Normalizes samples based on cross section and luminosity

    TTree *ttree = new TTree(sample, sample);
    

    //Creates a branch for each variable listed in "parameters" in the main function
    std::map<TString, Float_t> *parameters = new std::map<TString, Float_t>;
    std::map<TString, Int_t>::iterator setparameters;
    for ( setparameters = cut.begin(); setparameters != cut.end(); setparameters++){
        parameters->insert( std::pair<TString, Float_t> (setparameters->first, 0));
    }

    std::map<TString, Float_t>::iterator setbranch;
    for (setbranch = parameters->begin(); setbranch != parameters->end(); setbranch++){
        TString type = "/F";
        TString vartype = setbranch->first + type;
        ttree->Branch(setbranch->first, &(*parameters)[setbranch->first],vartype);
    }

    //Timer
    double duration = 0;
    double durationnew = 0;
    for (Int_t iEntry = 0; iEntry < numberOfEntries; iEntry++){
        treeReader->ReadEntry(iEntry);
        TString skip = "";

        durationnew = (( std::clock() - start ) / (double) CLOCKS_PER_SEC)/60;
        if (durationnew - duration > 1){
            cout << "Still running - it has been "<< durationnew << " minutes" <<'\n';
            duration = durationnew;
        }

        if (particletype["muon"]==1){
            // Event selection process on Jets
            skip = parameter_functions(branches["Muon"], muon, "muon", branches, cut, CME, ttree, parameters);
            if (skip == "fill" || skip == "skip") continue;
        }
        if (particletype["electron"]==1){
            // Event selection process on Jets
            skip = parameter_functions(branches["Electron"], electron, "electron", branches, cut, CME, ttree, parameters);
            if (skip == "fill" || skip == "skip") continue;
        }
        if (particletype["jet"]==1){
            // Event selection process on Jets
            skip = parameter_functions(branches["Jet"], jet, "jet", branches, cut, CME, ttree, parameters);
            if (skip == "fill" || skip == "skip") continue;
        }
    }

    TFile f(filename, "update");
    ttree->Write();
    f.Close();

    ofstream myfile;
    myfile.open ("normalization.txt", ios::app);
        cout << "Normalization constant for " << sample << " is " << norm_const << endl;
        cout << "The sample " << sample << " has unweighted " << numberOfEntries << " entries" << endl;
        myfile << sample << " " << norm_const << "\n"; //write to file
        myfile << sample << " " << numberOfEntries << "\n";
    myfile.close();
}

// For a simulated collision with multiple physics processes as individual delphes files, calculates relevant histograms into one root file. 
void parameter_tree(){
    TLorentzVector CME = {0,0,0,240}; // Defines center of mass energy
    Int_t luminosity = 10000; // Defines luminosity
    TString filename = "ntuples/zhSM_AramDelphesTree.root"; // Filename for to be created Tree collection
    TString directory = "/afs/cern.ch/work/j/jcurti/public/parallel_sample_gen/finalized_samples/"; // Directory of stored delphes samples

    //This is the list of variables that will be stored as branches in the created TTree
    //To add your own variable to the tree:
    //  add its name to the map here
    std::map<TString, Int_t> variables ={ // Initializes variables, number doesn't matter.}

        { "recoilmass" , 0},
        // { "vism" , 0},
        // { "vise" , 0},
        // { "vispt" , 0},
        // { "vispz" , 0},
        // { "ivise" , 0},
        // { "ivispt" , 0},
        // { "ivispz" , 0},
        { "divism" , 0},
        { "divise" , 0},
        { "divispt" , 0},
        { "divispz" , 0},
        { "diie" , 0},
        { "diipt" , 0},
        { "diipz" , 0},
        { "acoplanarity", 0},
        { "labangle", 0},
        // { "btag", 0},
        // { "numtracks", 0},
        // { "numjets", 0},
        { "costheta1", 0},
        { "costheta2", 0},

        { "jetveto" , 0},
        { "photonjetveto" , 0},
        { "eta1" , 0},
        { "eta2" , 0},
        { "phi1" , 0},
        { "phi2" , 0},
        { "charge1" , 0},
        { "charge2" , 0},
        { "FSR" , 0},
        { "num_photons_over10GevPT" , 0}
    };

    std::map<TString, Double_t> samples ={ // Defines which samples to process and their corresponding cross sections {"sample name", cross section}
    // { "eeZHinv", 20.5510948}, //3.7 mil events
    // { "eeZH_SM", 20.5555},
    // { "eeZZ", 2648.680783*.101},// 5.1 mil events
    // { "eeZZnoISR", 2843*.101},
    // { "eeWW", 12069.71738*.3171} //7.1 mil events
    // { "eeWWnoISR", 12600*.3171}

    { "eeZH_SM_AramDelphes", 20.553} //0.8 mil events

    };

    std::map<TString, Int_t> particletype ={ // Initializes particles {particle type, 1 = on}
    { "muon", 1 },
    { "electron", 1 },
    { "jet", 0 }
    };

    std::clock_t start;
    start = std::clock();

    // Runs over all selected samples and creates histograms of di-particle mass and missing mass
    std::map<TString, Double_t>::iterator process;
    for ( process = samples.begin(); process != samples.end(); process++){
        cout << "Found process " << process->first << endl;
        if (process->second != 0.0){
            processsimulation(process->first, process->second, CME, luminosity, filename, directory, variables, particletype, start);
        }
    }

    double duration;
    duration = (( std::clock() - start ) / (double) CLOCKS_PER_SEC)/60;
    cout<< "Process completed in "<< duration << " minutes" <<'\n';
}

