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

#include "modules/Delphes.h"                   //   |
#include "ExRootAnalysis/ExRootTreeReader.h"   //   -  Include needed delphes headers
#include "classes/DelphesClasses.h"            //   |

#endif

using namespace TMath;
using namespace std;

TString root = ".root";

TString get_total_cuts(std::map<TString, std::pair<TString, Int_t> > cut_set){
    TString total = "";
    std::map<TString, std::pair<TString, Int_t> >::iterator whichparam;
    for ( whichparam = cut_set.begin(); whichparam != cut_set.end(); whichparam++){
        if (whichparam->second.second == 1)
        {
            if (total == "")
            {
                total += whichparam->second.first;
            }
            else {
                total += " && " + whichparam->second.first;
            }            
        }
    }
    return total;
}


void processtree(TString sample, Double_t weight, TLorentzVector CME, TString filename, TString directory, TString cut, std::clock_t start){

    //Open File
    TFile *f = new TFile(directory);
    TTree *ttree = (TTree*)f->Get(sample);

    TFile g(filename, "update");

    TTree *T2 = ttree->CopyTree(cut); 
    T2->Write();

    ofstream myfile;
    myfile.open ("event_counter.txt", ios::app);
        cout << "The sample " << sample << " has " << T2->GetEntries() << " entries" << endl;
        myfile << sample << " " << T2->GetEntries() << "\n"; //write to file
    myfile.close();

    g.Close();
}

// For a simulated collision with multiple physics processes as individual delphes files, calculates relevant histograms into one root file. 
void treecutter_tree(){
    TLorentzVector CME = {0,0,0,350}; // Defines center of mass energy
    Int_t luminosity = 2600; // Defines luminosity
    TString filename = "ee_350.root"; // Filename for created histogram collection
    TString directory = "TREEPATH/ee_350.root"; // Directory of stored delphes samples

    TString recoilmass =    "(recoilmass > 0 && recoilmass < 350)";
    TString vism =          "(vism > 99 && vism < 140)";
    TString vispt =         "(vispt > 1 && vispt < 140)";
    TString vispz =         "(vispz > 0 && vispz < 130)";
    TString acoplanarity =  "(acoplanarity > 0 && acoplanarity < 1.58)";
    TString labangle  =     "(labangle > 1.54 && labangle < 3)";
    TString numtracks =     "(numtracks > 6 && numtracks < 40)";
    TString numjets =       "(numjets == 2)";
    TString btag =          "(btag == 2)";
    TString numleptons =    "(numleptons == 0)";
    TString costheta1 =     "(costheta1 > 0 && costheta1 < 0.989)";
    TString costheta2 =     "(costheta2 > 0 && costheta2 < 0.988)";
    TString ivism =         "(ivism > 0 && ivism < 350)";

    std::map<TString, std::pair<TString, Int_t> > cuts ={
    { "recoilmass", std::pair<TString, Int_t> (recoilmass,      1)},
    { "vism", std::pair<TString, Int_t> (vism,                  0)},
    { "vispt", std::pair<TString, Int_t> (vispt,                0)},
    { "vispz", std::pair<TString, Int_t> (vispz,                0)}, 
    { "acoplanarity", std::pair<TString, Int_t> (acoplanarity,  0)},
    { "labangle", std::pair<TString, Int_t> (labangle,          0)},
    { "numjets", std::pair<TString, Int_t> (numjets,            1)},
    { "numtracks", std::pair<TString, Int_t> (numtracks,        0)},
    { "btag", std::pair<TString, Int_t> (btag,                  1)},
    { "numleptons", std::pair<TString, Int_t> (numleptons,      1)},
    { "costheta1", std::pair<TString, Int_t> (costheta1,        0)},
    { "costheta2", std::pair<TString, Int_t> (costheta2,        0)},
    { "ivism", std::pair<TString, Int_t> (ivism,                0)}
    };

    std::map<TString, Double_t> samples ={ // Defines which samples to process and their corresponding cross sections {"sample name", cross section}
    { "ee_uuh_ww", 0},
    { "ee_uuh", 1},
    { "ee_uuh_zh", 0},
    {"ee_qq", 0},
    {"ee_tt", 0},
    {"ee_ww", 0},
    {"ee_zz", 0}
    // { "ee_uubb", 1},
    // { "ee_uucc", 1},
    // { "ee_uuqq", 0},
    // { "ee_qqll", 0},
    // { "ee_qqlv", 0},
    // { "ee_qqqq", 0},
    // { "ee_qq",  0},
    // { "ee_qqa", 0}
    };


    std::clock_t start;
    start = std::clock();

    // Runs over all selected samples and creates histograms of di-particle mass and missing mass
    TString total_cut = get_total_cuts(cuts);
    cout << "Total cuts: " + total_cut << endl;
    std::map<TString, Double_t>::iterator process;
    for ( process = samples.begin(); process != samples.end(); process++){
        cout << "Found process " << process->first << endl;
        if (process->second != 0.0){
            processtree(process->first, process->second, CME, filename, directory, total_cut, start);
        }
    }
    

    double duration;
    duration = (( std::clock() - start ) / (double) CLOCKS_PER_SEC)/60;
    cout<< "Process completed in "<< duration << " minutes" <<'\n';
}

