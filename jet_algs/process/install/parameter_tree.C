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
    
    cout << "Processng " << sample << endl;
    // Defines which sample to read from
    TString samplename = directory + sample + root;
    TChain chain("Delphes");
    chain.Add(samplename); 
    ExRootTreeReader *treeReader = new ExRootTreeReader(&chain); // Reads sample from delphes root fiile

    //Branches to be read from the collision file
    std::array<TString, 9> branchnames = {(TString)"Muon",(TString)"Electron",(TString)"Photon",(TString)"Jet",(TString)"Tower",(TString)"Track",(TString)"EFlowTrack",(TString)"EFlowPhoton",(TString)"EFlowNeutralHadron"};
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
        myfile << sample << " " << norm_const << "\n"; //write to file
    myfile.close();

    ofstream myfile2;
    myfile2.open ("event_counter.txt", ios::app);
        cout << "The sample " << sample << " has " << ttree->GetEntries() << " entries" << endl;
        myfile2 << sample << " " << ttree->GetEntries() << "\n"; //write to file
    myfile2.close();

}

// For a simulated collision with multiple physics processes as individual delphes files, calculates relevant histograms into one root file. 
void parameter_tree(){
    TLorentzVector CME = {0,0,0,350}; // Defines center of mass energy
    Int_t luminosity = 2600; // Defines luminosity
    TString filename = "ee_350.root"; // Filename for created histogram collection
    TString directory = "SAMPLEPATH"; // Directory of stored delphes samples

    std::map<TString, Int_t> cut ={ // Initializes cuts {cut name, integer does not matter here}
        { "recoilmass" , 0},
        { "vism" , 0},
        { "vise" , 0},
        { "vispt" , 0},
        { "vispz" , 0},
        { "divism", 0},     
        { "acoplanarity", 0},
        { "labangle", 0},
        { "btag", 0},
        { "numleptons", 0},
        { "numtracks", 0},
        { "numjets", 0},
        { "costheta1", 0},
        { "costheta2", 0}
    };

    std::map<TString, Double_t> samples ={ // Defines which samples to process and their corresponding cross sections {"sample name", cross section}
//    { "ee_uuh_ww", 30.68*0.5809},
    { "ee_uuh", 52.227*0.5809},
    { "ee_uuh_zh", 26.975*0.5809},
    { "ee_uubb", 66.532},
    { "ee_uucc", 72.965}
//    { "ee_uuqq", 203.44},
//    { "ee_qqll", 170.45},
//    { "ee_qqlv", 3719.51},
//    { "ee_qqqq", 566.47},
//    { "ee_qqtt", 566.47},
//    { "ee_qq",  21155},
//    { "ee_qqa", 36432}
    };

    std::map<TString, Int_t> particletype ={ // Initializes particles {particle type, 1 = on}
    { "muon", 0 },
    { "electron", 0 },
    { "jet", 1 }
    };

    std::clock_t start;
    start = std::clock();

    // Runs over all selected samples and creates histograms of di-particle mass and missing mass
    std::map<TString, Double_t>::iterator process;
    for ( process = samples.begin(); process != samples.end(); process++){
        cout << "Found process " << process->first << endl;
        if (process->second != 0.0){
            processsimulation(process->first, process->second, CME, luminosity, filename, directory, cut, particletype, start);
        }
    }

    double duration;
    duration = (( std::clock() - start ) / (double) CLOCKS_PER_SEC)/60;
    cout<< "Process completed in "<< duration << " minutes" <<'\n';
}

