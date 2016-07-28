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
Double_t MASS_Z = 91.1876;
Double_t MASS_H = 125.09;

void draw_hist(TString input_name, Double_t lumi){
    TFile *f = new TFile(input_name); 

    TH1D* ZHSignalHist = (TH1D*)f->Get("eeZH_SM_recoilmassHIST");
    TH1D* ZZ_Hist = (TH1D*)f->Get("eeZZ_recoilmassHIST");
    TH1D* WW_Hist = (TH1D*)f->Get("eeWW_recoilmassHIST");

    // ZHSignalHist->Scale(.35);
    // ZZ_Hist->Scale(.35);
    // WW_Hist->Scale(.35);

    // ZHSignalHist->Rebin(5); //Merges x bins into 1
    // ZZ_Hist->Rebin(5);
    // WW_Hist->Rebin(5);

    TH1D* Combined = new TH1D("ZHZZHist", "ZHZZHist", 300, 40, 160); 
    TH1D* ZZWWcombined = new TH1D("ZHZZHist2", "ZHZZHist2", 300, 40, 160); 
    *Combined = (*ZHSignalHist) + (*ZZ_Hist) + (*WW_Hist); //Creates a combined histogram by adding the 2 processes


    Double_t ZHyield = ZHSignalHist->Integral();
    Double_t ZZyield = ZZ_Hist->Integral();
    Double_t WWyield = WW_Hist->Integral();
    Double_t CombinedYield = Combined->Integral();
    
    cout << endl << "ZH Signal Yield " << ZHyield << endl;
    cout << "ZZ Background Yield " << ZZyield << endl;
    cout << "WW Background Yield " << WWyield << endl;
    cout << "Combined Yield " << CombinedYield << endl << endl; 

    cout << endl << "ZH Signal Yield     ZZ Background Yield     WW Background Yield    Combined Yield" << endl;
    cout <<  ZHyield << " " << ZZyield << " "<< WWyield << " " << CombinedYield << endl << endl;
    
    //Total background histogram for drawing
    
    *ZZWWcombined = (*ZZ_Hist) + (*WW_Hist);

    TCanvas *can = new TCanvas("ILD ZH & ZZ Whizard", "ILD ZH & ZZ Whizard", 1280, 720);
    can->SetGrid();
    // can->Divide(1,1); //Can be used to split canvas into multiple pads
    // can->cd(1);

    //Set general plot properties on the process to be drawn first
    // Combined->SetTitle("ZH, ZZ, & WW Missing Mass (H to Inv BR = 100\%)");
    Combined->SetTitle("ZH, ZZ, & WW Recoil Mass");
    // Combined->GetXaxis()->SetTitle("Missing Mass [GeV]");
    Combined->GetXaxis()->SetTitle("Recoil Mass [GeV]");
    Combined->GetYaxis()->SetTitle("Events / 0.4 GeV");
    Combined->GetYaxis()->SetTitleOffset(1.3);
    // Combined->SetMaximum(22100); //Sets y axis max
    // Combined->SetMinimum(0);     //Sets y axis min

    Combined->SetLineColor(kRed);
    Combined->SetLineWidth(2);
    Combined->Draw("hist same");
    Combined->SetStats(0); //Turns off stat box

    ZZWWcombined->SetLineColor(kBlue);
    ZZWWcombined->SetFillStyle(3004);
    ZZWWcombined->SetFillColor(kBlack);
    ZZWWcombined->SetLineWidth(2);
    ZZWWcombined->Draw("hist same");

    WW_Hist->SetLineColor(kViolet);
    WW_Hist->SetFillStyle(3005);
    WW_Hist->SetFillColor(kBlack);
    WW_Hist->SetLineWidth(2);
    WW_Hist->Draw("hist same");

    TLegend* leg = new TLegend(0.128527,0.778261,0.272727,0.888406);
    //leg->SetHeader("The Legend Title");
    leg->AddEntry(Combined,"Signal + Backgrounds","l");
    leg->AddEntry(ZZWWcombined,"ZZ Background","l");
    leg->AddEntry(WW_Hist,"WW Background","l");
    leg->AddEntry(ZZWWcombined,"All Backgrounds","f");
    leg->Draw();

    //Codes for a text box on the plot to display information
    TString sfraction = TString::Format("%.f",ZHyield);
    TPaveText* pt = new TPaveText(0.722571,0.747826,0.851881,0.857971, "NDC"); // NDC sets coords
                                                                               // relative to pad dimensions
    pt->SetFillColor(0); // text is black on white
    pt->SetTextSize(0.0231884); 
    pt->SetTextAlign(12);
    TString str_lumi = TString::Format("%.f",lumi);
    pt->AddText("Luminosity: " + str_lumi + " ab^{-1}");
    pt->AddText("ZH Signal yield: " + sfraction);
    sfraction = TString::Format("%.f",ZZyield);
    pt->AddText("ZZ yield: " + sfraction);
    sfraction = TString::Format("%.f",WWyield);
    pt->AddText("WW yield: " + sfraction);
    pt->SetShadowColor(0);
    pt->SetTextFont(132);
    pt->Draw();       //to draw your text object

    can->Print("",".gif"); //Automatically saves plot to a .gif file with the same name as the histogram
}

TString get_total_cuts(std::map<Int_t, std::pair<TString, Int_t> > cut_set){
    TString total = "";
    for(Int_t i=1; i < ((Int_t) cut_set.size())+1; i++){
        if (cut_set[i].second == 1)
        {
            if (total == "")
            {
                total += cut_set[i].first;
            }
            else {
                total += " && " + cut_set[i].first;
            }            
        }
    }
    return total;
}

// Takes as input a given process with corresponding luminosity, as well as sample CME, cuts, directory location, and output file name - saves corresponding histogram in root file.
void processtree(TString sample, Double_t weight, TLorentzVector CME, TString filename, TString directory, TString cut, std::clock_t start){

    //Open File
    TFile *f = new TFile(directory);
    TTree *ttree = (TTree*)f->Get(sample);

    TString recohistname = "_recoilmassHIST";
    TString histname = sample + recohistname;
    TH1D *recoilmassHIST = new TH1D(histname,histname, 300, 40, 160);
    ttree->Project(histname, "recoilmass", cut);

    //Normalize Hist
    recoilmassHIST->Scale(weight);

    TFile g(filename, "update");
    recoilmassHIST->Write();
    g.Close();
}

// For a simulated collision with multiple physics processes as individual delphes files, calculates relevant histograms into one root file. 
void tree_hist(){
    TLorentzVector CME = {0,0,0,240}; // Defines center of mass energy
    TString filename = "hists/treeHists.root"; // Filename for created histogram collection
    TString directory = "ntuples/zhSMzzwwLeadingTree3.root"; //Input tree

    TH1D* data_obs = new TH1D("data_obs", "data_obs", 300, 40, 160); 

    TFile f(filename, "recreate");
    data_obs->Write();
    f.Close();

    //Format your cuts as TStrings
    //Note cuts should be defined as what is accepted

    TString jetveto = "(jetveto == 0)";
    TString num_FSRcand = "(num_photons_over10GevPT <= 1)";
    TString charge = "(charge1 == -1*charge2)";
    TString labangle = "(labangle > (100*TMath::Pi()/180.0) )"; //acollinearity
    TString acoangle = "(acoplanarity > (10*TMath::Pi()/180.0) )";
    TString diparticlept = "(divispt > 10)";
    TString diparticlepl = "(divispz < 50)";
    TString zmass = "( (divism <= (91.1876 + 4)) && (divism >= (91.1876 - 4)) )";

    TString zmass2 = "( (divism <= (91.1876 + 5)) && (divism >= (91.1876 - 5)) )";
    TString photonjetveto = "(photonjetveto == 0)";

    //List the cuts here and turn them on and off with 0 or 1
    std::map<Int_t, std::pair<TString, Int_t> > cuts ={ 
    { 1, std::pair<TString, Int_t> (jetveto,          0)},
    { 2, std::pair<TString, Int_t> (num_FSRcand,      0)},
    { 3, std::pair<TString, Int_t> (charge,           1)},
    { 4, std::pair<TString, Int_t> (labangle,         1)}, 
    { 5, std::pair<TString, Int_t> (acoangle,         1)},
    { 6, std::pair<TString, Int_t> (diparticlept,     1)},
    { 7, std::pair<TString, Int_t> (diparticlepl,     1)},
    { 8, std::pair<TString, Int_t> (zmass,            0)},

    { 9, std::pair<TString, Int_t> (zmass2,           1)},
    { 10, std::pair<TString, Int_t> (photonjetveto,   0)}
    };

    //This combines the above defined cuts that are on (have value 1) into a single string to be passed
    // as a parameter
    TString total_cut = get_total_cuts(cuts);
    cout << "Total cuts: " + total_cut << endl;

    //Define your samples name here as they are in your sample directory
    //Note the number is the normalization constant for that sample, not the cross section!
    //weight = xsec*lumi/total_unweighted_events

    std::map<TString, Double_t> samples ={ // Defines which samples to process and their corresponding cross sections {"sample name", weight}
    // { "eeZHinv",  0.0529862}, //3.7 mil events
    { "eeZH_SM", 0.257656}, //0.8 mil events
    { "eeZZ", 0.523514},// 5.1 mil events
    // { "eeZZnoISR", 2843*.101},
    { "eeWW", 5.38153} //7.1 mil events
    // { "eeWWnoISR", 12600*.3171}

    // { "eeZH_SM_AramDelphes", 0.255364} //0.8 mil events

    };

    std::clock_t start;
    start = std::clock();

    // Runs over all selected samples and creates histograms of di-particle mass and missing mass
    for (Int_t i=0; i<1; i++){
        std::map<TString, Double_t>::iterator process;
        for ( process = samples.begin(); process != samples.end(); process++){
            cout << "Found process " << process->first << endl;
            if (process->second != 0.0){
                processtree(process->first, process->second, CME, filename, directory, total_cut, start);
            }
        }
    }

    double duration;
    duration = (( std::clock() - start ) / (double) CLOCKS_PER_SEC)/60;
    cout<< "Process completed in "<< duration << " minutes" <<'\n';

    draw_hist(filename, 10); //draws the histogram if you want to see it
    //Otherwise histograms can be drawn later from the created histogram collection
}








