// E+E- Delphes Sample Processing for ee->bbvv final state
// Alexander Andriatis and Joseph Curti
// 20 June 2016

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

#include "hist_functions.h"
#include "modules/Delphes.h"                   //   |
#include "ExRootAnalysis/ExRootTreeReader.h"   //   -  Include needed delphes headers
#include "classes/DelphesClasses.h"            //   |

#endif

using namespace TMath;
using namespace std;

// Takes as input a given process with corresponding luminosity, as well as sample CME, cuts, directory location, and output file name - saves corresponding histogram in root file.
void processsimulation(std::pair<TString, Double_t> sample, TString filename, std::map<TString, Int_t> precut, std::map<TString, Int_t> cut, std::map<TString, Int_t> particletype, TLorentzVector CME, Int_t luminosity, TString directory){
	
    // Defines which sample to read from
    TString root = ".root";
	TString samplename = directory + sample.first + root;
	TChain chain("Delphes");
    chain.Add(samplename); 
    ExRootTreeReader *treeReader = new ExRootTreeReader(&chain); // Reads sample from delphes root fiile

    Int_t numbins = CME.M()/2; // Calculates number of bins for 2GeV/bin
    TString masshist = "_massHIST";
    TString recoilmasshist = "_recoilmassHIST";
    TString namemasshist = sample.first + masshist;
    TString namerecoilmasshist = sample.first + recoilmasshist;
    TH1D* massHIST = new TH1D(namemasshist, namemasshist, numbins, 0, CME.M()); // Histogram of visible di-particle mass
    TH1D* recoilmassHIST = new TH1D(namerecoilmasshist, namerecoilmasshist, numbins, 0, CME.M()); // Histogram of missing mass
    TH1D* data_obs = new TH1D("data_obs", "data_obs", numbins, 0, CME.M());
    TH1D* recoil_data_obs = new TH1D("recoil_data_obs", "recoil_data_obs", numbins, 0, CME.M());
   


    //Branches to be read from the collision file
    TClonesArray *branchMuon = treeReader->UseBranch("Muon");
    TClonesArray *branchElectron = treeReader->UseBranch("Electron");
    TClonesArray *branchPhoton = treeReader->UseBranch("Photon");
    TClonesArray *branchJet = treeReader->UseBranch("Jet");
    TClonesArray *branchTower = treeReader->UseBranch("Tower");
    TClonesArray *branchTrack = treeReader->UseBranch("Track");

    // Variables to store particles
    Electron *electron = 0;
    Muon *muon = 0;
    Jet *jet = 0;
    Tower *tower = 0;
    Track *track = 0;
	TString skip = "";

	Int_t numberOfEntries = treeReader->GetEntries();
	Double_t norm_const = (sample.second*luminosity)/numberOfEntries; // Normalizes histograms based on cross section and luminosity

	for (Int_t iEntry = 0; iEntry < numberOfEntries; iEntry++){
        skip = "none";
		treeReader->ReadEntry(iEntry);

        if (particletype["muon"]==1){
            // Event selection process on Muons
            skip = hist_fill(branchMuon, muon, "muon", massHIST, recoilmassHIST, norm_const, branchTrack, branchTower, branchJet, branchPhoton,  precut, cut, CME);
            if (skip == "fill" || skip == "skip") continue;
        }

        if (particletype["electron"]==1){
            // Event selection process on Electrons
            skip = hist_fill(branchElectron, electron, "electron", massHIST, recoilmassHIST, norm_const, branchTrack, branchTower, branchJet, branchPhoton, precut, cut, CME);
            if (skip == "fill" || skip == "skip") continue;
        }

        if (particletype["jet"]==1){
            // Event selection process on Jets
            skip = hist_fill(branchJet, jet, "jet", massHIST, recoilmassHIST, norm_const, branchTrack, branchTower, branchJet, branchPhoton, precut, cut, CME);
            if (skip == "fill" || skip == "skip") continue;
        }
    }
    // Writes the histograms to a root file
    
    *data_obs = *massHIST;
    data_obs->SetName("data_obs");
    TString massfilename = filename + masshist + root;
    TFile f(massfilename,"update");
    massHIST->Write();
    data_obs->Write();
    f.Close();

    *recoil_data_obs = *recoilmassHIST;
    recoil_data_obs->SetName("data_obs");
    TString recoilmassfilename = filename + recoilmasshist + root;
    TFile g(recoilmassfilename,"update");
    recoilmassHIST->Write();
    recoil_data_obs->Write();
    g.Close();
}

// For a simulated collision with multiple physics processes as individual delphes files, calculates relevant histograms into one root file. 
void hist_process(){
	TLorentzVector CME = {0,0,0,350}; // Defines center of mass energy
	Int_t luminosity = 2600; // Defines luminosity
	TString filename = "eebb_350"; // Filename for created histogram collection
	TString directory = "/afs/cern.ch/work/a/aandriat/public/autogen/output/"; // Directory of stored delphes samples

	std::map<TString, Int_t> precut ={ // Initializes precuts {cut name, 1 = on}
    { "numjets", 1 },
    { "isoparticles", 0 }
	};

	std::map<TString, Int_t> cut ={ // Initializes cuts {cut name, 1 = on}
    { "photon", 0 },
    { "charge", 0 },
    { "zmass", 0 },
    { "labangle", 0 },
    { "acoangle", 0 },
    { "diparticlept", 0 },
    { "diparticlepl", 0 },
    { "vismass", 0 },
    { "visenergy", 0 },
    { "vispt", 0 },
    { "btag", 0 },
    { "visz", 0 },
    { "aglecut", 0 },
    { "numtracks", 0 },
    { "numevis", 0 },
    { "recoilmass", 0 },
    { "reconstructedmass", 0 },
	};

    std::map<Int_t, std::pair<TString, Double_t> > samples ={ // Defines which samples to process and their corresponding cross sections {index, <"sample name", cross section>}
    { 1, std::pair<TString, Double_t> ("eebb_uu_350", 52.30)},
    { 2, std::pair<TString, Double_t> ("eebb_uu_zh_350", 27.06)},
    { 3, std::pair<TString, Double_t> ("eebb_uu_ww_350", 52.30-27.06)}
    };
    // ("eebb_uu_240", 45.86)
    // ("eebb_uu_zh_240", 39.78)
    // ("eebb_uu_ww_240", 45.86-39.78)
    // ("eebb_uu_350", 52.30)
    // ("eebb_uu_zh_350", 27.06)
    // ("eebb_uu_ww_350", 52.30-27.06)

    // { 1, std::pair<TString, Double_t> ("eebb_uu_240", 45.86)},
    // { 2, std::pair<TString, Double_t> ("eebb_uu_zh_240", 39.78)},
    // { 3, std::pair<TString, Double_t> ("eebb_uu_ww_240", 45.86-39.78)}


    std::map<TString, Int_t> particletype ={ // Initializes particles {particle type, 1 = on}
    { "muon", 0 },
    { "electron", 0 },
    { "jet", 1 }
    };

    // Runs over all selected samples and creates histograms of di-particle mass and missing mass
	for(Int_t i=1; i < samples.size()+1; i++){
		processsimulation(samples[i],filename, precut, cut, particletype, CME, luminosity, directory);
	}

    hist_draw(filename, samples); // Crudely plots all histograms and saves as PDF
}

