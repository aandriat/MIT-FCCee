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

#endif

using namespace TMath;
using namespace std;

TString root = ".root";


TString get_total_cuts(std::map<TString, std::pair<TString, Int_t> > cut_set, Int_t all){
    TString total = "";
    std::map<TString, std::pair<TString, Int_t> >::iterator whichparam;
    for ( whichparam = cut_set.begin(); whichparam != cut_set.end(); whichparam++){
        if (whichparam->second.second == 1 || all == 1)
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


// Takes as input a given process with corresponding luminosity, as well as sample CME, cuts, directory location, and output file name - saves corresponding histogram in root file.
void processtree(TString sample, Double_t weight, TLorentzVector CME, TString filename, TString directory, TString param, TString cut, std::clock_t start, Int_t all){

    //Open File
    TFile *f = new TFile(directory);
    TTree *ttree = (TTree*)f->Get(sample);

    TString recohistname, paramhistname;
    if (all==0){
        recohistname = "_recoilmassHIST";
        paramhistname = "_parameterHIST";
    }
    if (all==1){
        recohistname = "_recoilmassHIST_all";
        paramhistname = "_parameterHIST_all";
    }

    TString histname = sample + recohistname;
    TH1D *recoilmassHIST   = new TH1D(histname,histname,CME.M()/2,0,CME.M());
    //recoilmassHIST->SetCanExtend(kAllAxes);
    ttree->Project(histname, "recoilmass", cut);

    histname = sample + paramhistname;
    TH1D *parameterHIST   = new TH1D(histname,histname,CME.M()/2,1,-1);
    //parameterHIST->SetCanExtend(kAllAxes);
    ttree->Project(histname, param, cut);

    //Normalize Hist
//    recoilmassHIST->Scale(1/recoilmassHIST->GetMaximum());
//    parameterHIST->Scale(1/parameterHIST->GetMaximum()) ;

    //Normalize Hist
    recoilmassHIST->Scale(weight);
    parameterHIST->Scale(weight) ;

    // Writes yield of each histogram for each cut to text file
    ofstream myfile;
    Double_t yield = recoilmassHIST->Integral(); 
    myfile.open ("yield.txt", ios::app);
        cout << "Yield for " << sample << " for Cut " << param << " is " << yield << endl;
        myfile << param << " " << all << " " << sample << " " << yield << "\n"; //write to file
    myfile.close();

    if (recoilmassHIST->Integral() == 0){
        recoilmassHIST->Fill(0);
    }
    if (parameterHIST->Integral() == 0){
        parameterHIST->Fill(0);
    }

    TFile g(filename, "update");
    recoilmassHIST->Write();
    parameterHIST->Write();
    g.Close();
}

// For a simulated collision with multiple physics processes as individual delphes files, calculates relevant histograms into one root file. 
void n1_tree_hist(){
    TLorentzVector CME = {0,0,0,350}; // Defines center of mass energy
    TString filename = "ee_350_cutSSS.root"; // Filename for created histogram collection
    TString directory = "TREEPATH/ee_350.root"; // Directory of stored delphes samples

    TString recoilmass =    "(recoilmass > 0 && recoilmass < 350)";
    TString vism =          "(vism > 102 && vism < 188)";
    TString vispt =         "(vispt > 0 && vispt < 350)";
    TString vispz =         "(vispz > 0 && vispz < 350)";
    TString acoplanarity =  "(acoplanarity > 0 && acoplanarity < 1.58)";
    TString labangle  =     "(labangle > 1.37 && labangle < 3.56)";
    TString numtracks =     "(numtracks > 3 && numtracks < 37)";
    TString numjets =       "(numjets == 2)";
    TString btag =          "(btag == 2)";
    TString numleptons =    "(numleptons == 0)";
    TString costheta1 =     "(costheta1 > 0 && costheta1 < 0.989)";
    TString costheta2 =     "(costheta2 > 0 && costheta2 < 0.988)";

    std::map<TString, std::pair<TString, Int_t> > cuts ={ 
    { "recoilmass", std::pair<TString, Int_t> (recoilmass,           1)},
    { "vism", std::pair<TString, Int_t> (vism,           QQQ)},
    { "vispt", std::pair<TString, Int_t> (vispt,          WWW)},
    { "vispz", std::pair<TString, Int_t> (vispz,          EEE)}, 
    { "acoplanarity", std::pair<TString, Int_t> (acoplanarity,   RRR)},
    { "labangle", std::pair<TString, Int_t> (labangle,       TTT)},
    { "numjets", std::pair<TString, Int_t> (numjets,        YYY)},
    { "numtracks", std::pair<TString, Int_t> (numtracks,      UUU)},
    { "btag", std::pair<TString, Int_t> (btag,           III)},
    { "numleptons", std::pair<TString, Int_t> (numleptons, OOO)},
    { "costheta1", std::pair<TString, Int_t> (costheta1, PPP)},
    { "costheta2", std::pair<TString, Int_t> (costheta2, LLL)}
    };
    
    std::map<TString, Double_t> samples ={ // Defines which samples to process and their corresponding cross sections {"sample name", weight}
//    {"ee_uuh_ww", 0.463372},
    {"ee_uuh", 0.937759},
    {"ee_uuh_zh", 0.484296},
    {"ee_uubb", 2.05756},
    {"ee_uucc", 2.28987}
//    {"ee_qq", 691.383},
//    {"ee_qqa", 1284.99},
//    {"ee_qqqq", 14.7282},
//    {"ee_qqtt", 14.7282},
//    {"ee_qqll", 17.9719},
//    {"ee_qqlv", 220.225},
//    {"ee_uuqq", 6.30769}    
    }; 
  

    std::clock_t start;
    start = std::clock();

    TString param;
    std::map<TString, std::pair<TString, Int_t> >::iterator whichparam;
    for ( whichparam = cuts.begin(); whichparam != cuts.end(); whichparam++){
        if (whichparam->first != "recoilmass" && whichparam->second.second == 0){
            param = whichparam->first;
        }
    }

    // Runs over all selected samples and creates histograms of di-particle mass and missing mass
    for (Int_t i=0; i<2; i++){
        TString total_cut = get_total_cuts(cuts, i);
        cout << "Total cuts: " + total_cut << endl;
        std::map<TString, Double_t>::iterator process;
        for ( process = samples.begin(); process != samples.end(); process++){
            cout << "Found process " << process->first << endl;
            if (process->second != 0.0){
                processtree(process->first, process->second, CME, filename, directory, param, total_cut, start, i);
            }
        }
    }

    double duration;
    duration = (( std::clock() - start ) / (double) CLOCKS_PER_SEC)/60;
    cout<< "Process completed in "<< duration << " minutes" <<'\n';
}
