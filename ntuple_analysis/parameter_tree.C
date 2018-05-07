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
#include <TTreeReader.h>
#include <TTreeReaderValue.h>
#include <TTreeReaderArray.h>

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
    std::array<TString, 7> branchnames = {(TString)"Muon",(TString)"Electron",(TString)"Photon",
                    (TString)"Jet", (TString)"Particle", (TString)"JetOinc", (TString)"JetInc" };
                                       // (TString)"Tower",(TString)"Track",(TString)"EFlowTrack",(TString)"EFlowPhoton",(TString)"EFlowNeutralHadron"};
    std::map<TString, TClonesArray*> branches;
    for (int i=0; i<(int)branchnames.size(); i++){
        branches.insert ( std::pair<TString, TClonesArray*> (branchnames[i], treeReader->UseBranch(branchnames[i])));
    }

    cout << "Created branches" << endl;
    // Variables to store particles
    Electron *electron = 0;
    Muon *muon = 0;
    Jet *jet = 0;

    Double_t numberOfEntries = treeReader->GetEntries();
    Double_t norm_const = (crosssection*luminosity)/numberOfEntries; // Normalizes samples based on cross section and luminosity

    TFile f(filename, "update");
    f.cd();
    TTree *ttree = new TTree(sample, sample);
    cout << "Initialized Tree" << endl;
    

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
    cout << "Initialized branches" << endl;
    //Timer
    double duration = 0;
    double durationnew = 0;
    for (Int_t iEntry = 0; iEntry < numberOfEntries; iEntry++){
        treeReader->ReadEntry(iEntry);
        TString skip = "";

        if (!(branches["Jet"])) {
            cout << "Could not open branchJet" << endl;
            continue;
        }
        if (!(branches["Photon"])) {
            cout << "Could not open branch" << endl;
            continue;
        }
        if (!(branches["Electron"])) {
            cout << "Could not open branch" << endl;
            continue;
        }
        if (!(branches["Muon"])) {
            cout << "Could not open branch" << endl;
            continue;
        }
        if (!(branches["Particle"])) {
            cout << "Could not open branch" << endl;
            continue;
        }

        durationnew = (( std::clock() - start ) / (double) CLOCKS_PER_SEC)/60;
        if (durationnew - duration > 1){
            cout << "Still running - it has been "<< durationnew << " minutes" <<'\n';
            duration = durationnew;
        }

        if (particletype["muon"]==1){
            // Event selection process on Jets
            skip = parameter_functions(branches["Muon"], muon, "muon", branches, cut, CME, ttree, parameters, norm_const, numberOfEntries);
            if (skip == "fill" || skip == "skip") continue;
        }
        if (particletype["electron"]==1){
            // Event selection process on Jets
            skip = parameter_functions(branches["Electron"], electron, "electron", branches, cut, CME, ttree, parameters, norm_const, numberOfEntries);
            if (skip == "fill" || skip == "skip") continue;
        }
        if (particletype["jet"]==1){
            // Event selection process on Jets
            skip = parameter_functions(branches["Jet"], jet, "jet", branches, cut, CME, ttree, parameters, norm_const, numberOfEntries);
            if (skip == "fill" || skip == "skip") continue;
        }
    }

    // TFile f(filename, "update");
    cout << "Before write step" << endl;
    ttree->CloneTree()->Write();
    f.Close();
    cout << "After write step" << endl;

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
    Int_t luminosity = 5000; // 10000 fb^-1 Defines luminosity
    // TString filename = "ntuples/zhSMzzwwTree41.root"; // Filename for to be created Tree collection
    // TString filename = "ntuples/bbHinvILD.root";
    // TString filename = "ntuples/bbHinvILDexcScaling.root";
    TString filename = "ntuples/Hinv_ZHZZWW.root";
    // TString filename = "ntuples/zhzzwwSM0350GeV.root";
    TString directory = "/afs/cern.ch/work/j/jcurti/public/parallel_sample_gen/finalized_samples/"; // Directory of stored delphes samples

    // TString filename = "ntuples/zhSM21.root"; //
    //This is the list of variables that will be stored as branches in the created TTree
    //To add your own variable to the tree:
    //  add its name to the map here
    std::map<TString, Int_t> variables ={ // Initializes variables, number doesn't matter.}
        {"numUnweightedEvents", 0},
        {"eventWeight" , 0},

        { "recoilmass" , 0},
        { "scaleddivism" , 0},
        { "scaledrecoilmass" , 0},
        { "recoilmassME" , 0},
        { "recoilmassMP" , 0},

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
        { "btag", 0},
        { "tautag", 0},
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
        { "numElectrons" , 0},
        { "numMuons" , 0},
        { "numJets" , 0},
        { "numPhotons" , 0},
        { "PT1" , 0},
        { "PT2" , 0},
        { "genPT1" , 0},
        { "genPT2" , 0},
        { "PTresolution1" , 0},
        { "PTresolution2" , 0},
        { "FSR" , 0},
        { "flavor" , 0}, //flavor of lepton 0 = electron, 1 = muon
        { "jetveto2" , 0},
        { "num_photons_over10GevPT" , 0},
        { "FSRv2" , 0},

        { "INCeta1" , 0},
        { "INCeta2" , 0},
        { "INCphi1" , 0},
        { "INCphi2" , 0},
        { "INCPT1" , 0},
        { "INCPT2" , 0},
        { "INCacoplanarity" , 0},
        { "INClabangle" , 0},
        { "INCbtag" , 0},
        { "INCtautag" , 0},
        { "INCscaledrecoilmass" , 0},
        { "INCscaleddivism" , 0},
        { "INCdivism" , 0},
        { "INCrecoilmass" , 0},
        { "INCdivispt" , 0},
        { "INCdivispz" , 0},
        { "numOIncJets" , 0},
        { "numIncJets" , 0},

        { "INCfoundjets" , 0}


    };

    std::map<TString, Double_t> samples ={ // Defines which samples to process and their corresponding cross sections {"sample name", cross section}
    // { "eeZHinv", 20.5510948}, //3.7 mil events
    // { "eeZH_SM", 20.5555},
    // { "eeZZ", 2648.680783*.101},// 5.1 mil events
    // { "eeZZnoISR", 2843*.101},

    // { "eeZHinv11", 20.54}, //1 mil events
    // { "eeZZ11", 2647.4*.101},// 3 mil events
    // { "eeWW11", 12069.3*.3171} //5 mil events

    // { "eeZHinv21", 20.55}, //1 mil events
    // { "eeZZ21", 2655.5*.101},// 3 mil events
    // { "eeWW21", 12073.8*.3171} //5 mil events

    // { "eeZH_SM41", 20.55},
    // { "eeZHinv41", 20.55}, //1 mil events
    // { "eeZZ41", 2650.1*.101},// 2 mil events
    // { "eeWW41", 12066.6*.3171} //5 mil events

    // { "eeZH_SM12", 20.55}
    // { "eeZHinv12", 20.55}, //1 mil events
    // { "eeZZ12", 2647*.101},// 3 mil events
    // { "eeWW12", 12079.99*.3171} //5 mil events

    // { "eeZH_SM11", 20.553}
    // { "eeZH_SM21", 20.55}

    // { "eeZH_SM14", 20.55}
    // { "eeZHinv14", 20.55}, //1 mil events
    // { "eeZZ14", 2642*.101},// 3 mil events
    // { "eeWW14", 12076*.3171}

    // { "eeZH_SM0", 20.55}
    // { "eeZHinv0", 20.55}, //1 mil events
    // { "eeZZ0", 2646.4*.101},// 3 mil events
    // { "eeWW0", 12071.5*.3171}

    // { "eeZHinv0", 20.55}, //1 mil events
    // { "eeZH_SM0_350GeV", 16.466}, //500k
    // { "eeZZ0_350GeV", 2351.4*.101},// 1 mil
    // { "eeWW0_350GeV", 10313.5*.3171}, //5 mil


    // { "eeWW", 12069.71738*.3171} //7.1 mil events
    // { "eeWWnoISR", 12600*.3171}

    // { "eeZH_SM_AramDelphes", 20.553} //0.8 mil events

    // { "eeZHbbinv", 29.5}, //.1 mil events
    // { "eeZZbbinv", 575*.2} //.1 mil events

    // { "eeZHbbinvExc", 29.53}, //.5 mil events
    // { "eeZZbbinvExc", 556.7*.2}, //.5 mil events


    // { "eeZHbbinvExcILD", 29.51}, //.4 mil events
    // { "eeZZbbinvExcILD", 342.13*.2}, //.4 mil events
    // { "ZinvHbb", 39.84*.584}, //.8 mil events
    // { "WWexc", 16651}, //1.5 mil events

        // { "zhINCtest", 29.51}, //20k events
    // { "wwINCtest", 16651}, //20k events
    // { "jetTest3", 16651}, //20k events
    // { "WW2jet", 16652}, //20k events
    // { "ZHbbinv2jet", 29.5}, //20k events

    // { "WW2jet", 16652}, //20k events
    // { "ZHbbinv2jet", 29.5}, //20k events


    //{ "WW2jet_Large", 16641.8043677808}, //20k events
    { "eeZZ_ZinvZjets", 2035.0682654736*.2}, //20k events
    { "eeZH_ZjetsHinv", 135.3489155678}, // events
    { "eeWW_inc_merge2_1star", 16647.2607203218}, //20k events                                                                                                     
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
            processsimulation(process->first, process->second, CME, luminosity, filename, directory, variables, particletype, start);
        }
    }

    cout << "FSR count: " << FSR_count << endl;
    double duration;
    duration = (( std::clock() - start ) / (double) CLOCKS_PER_SEC)/60;
    cout<< "Process completed in "<< duration << " minutes" <<'\n';
}

