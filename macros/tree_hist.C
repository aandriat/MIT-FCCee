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
#include <TColor.h>
#include <cstdio>
#include <ctime>
#include "HttStyles.h"
#include "modules/Delphes.h"                   //   |
#include "ExRootAnalysis/ExRootTreeReader.h"   //   -  Include needed delphes headers
#include "classes/DelphesClasses.h"            //   |

#endif

using namespace TMath;
using namespace std;

TString root = ".root";
Double_t MASS_Z = 91.1876;
Double_t MASS_H = 125.09;

void draw_hist(TString input_name, Double_t lumi = 10.0, TString xtitle = ""){
    TFile *f = new TFile(input_name); 

    TH1D* ZHSignalHist = (TH1D*)f->Get("eeZHinv_recoilmassHIST");
    TH1D* ZZ_Hist = (TH1D*)f->Get("eeZZ_recoilmassHIST");
    TH1D* WW_Hist = (TH1D*)f->Get("eeWW_recoilmassHIST");

    ZHSignalHist->Scale(lumi/10.0);
    ZZ_Hist->Scale(lumi/10.0);
    WW_Hist->Scale(lumi/10.0);

    Int_t rebin = 1;
    ZHSignalHist->Rebin(rebin); //Merges x bins into 1
    ZZ_Hist->Rebin(rebin);
    WW_Hist->Rebin(rebin);

    Int_t numBins = ZHSignalHist->GetSize() - 2;
    Double_t xmin = ZHSignalHist->GetXaxis()->GetXmin();
    Double_t xmax = ZHSignalHist->GetXaxis()->GetXmax();
    Double_t GeVperBin = TMath::Abs(xmax-xmin)/((Double_t) numBins);

    TString str_gev;
    if (GeVperBin - ((Int_t) GeVperBin) == 0){
        str_gev = TString::Format("%.0f",GeVperBin);
    }
    else{
        str_gev = TString::Format("%.1f",GeVperBin);
    }

    TString ytitle = "Events / " + str_gev + " GeV";
    cout << xtitle << endl;
    cout << ytitle << endl;

    TH1D* Combined = new TH1D("ZHZZHist", "ZHZZHist", numBins, xmin, xmax); 
    *Combined = (*ZHSignalHist) + (*ZZ_Hist) + (*WW_Hist);
    TH1D* ZZWWcombined = new TH1D("ZHZZHist2", "ZHZZHist2", numBins, xmin, xmax); 
    *ZZWWcombined = (*ZZ_Hist) + (*WW_Hist);
    // InitHist(ZZWWcombined, xtitle, ytitle, kYellow - 7, 1001); //kBlue - 3
    // InitHist(WW_Hist, xtitle, ytitle, kRed - 7, 1001); //kRed - 3
    // // InitHist(Combined, xtitle, ytitle, kBlue - 7, 1001);
    // InitSignal(Combined);
    // Combined->SetLineColor(kBlue + 1);

    InitHist(ZZWWcombined, xtitle, ytitle, kYellow - 4, 1001); //kBlue - 3
    InitHist(WW_Hist, xtitle, ytitle, kBlue - 4, 1001); //kRed - 3
    InitSignal(Combined);
    Combined->SetLineColor(kRed);

    Combined->GetXaxis()->SetTitle(xtitle);
    Combined->GetYaxis()->SetTitle(ytitle);
    Combined->GetYaxis()->SetTitleOffset(1.35);
    // Combined->SetMaximum(22100); //Sets y axis max
    // Combined->SetMinimum(0);     //Sets y axis min

    Double_t ZHyield = ZHSignalHist->Integral();
    Double_t ZZyield = ZZ_Hist->Integral();
    Double_t WWyield = WW_Hist->Integral();
    Double_t CombinedYield = Combined->Integral();
    cout << endl << "ZH Signal Yield " << ZHyield << endl;
    cout << "ZZ Background Yield " << ZZyield << endl;
    cout << "WW Background Yield " << WWyield << endl;
    cout << "Combined Yield " << CombinedYield << endl << endl; 

    // cout << endl << "ZH Signal Yield     ZZ Background Yield     WW Background Yield    Combined Yield" << endl;
    // cout <<  ZHyield << " " << ZZyield << " "<< WWyield << " " << CombinedYield << endl << endl;
    

    // TCanvas *can = new TCanvas("ILD ZH & ZZ Whizard", "ILD ZH & ZZ Whizard", 800, 600);
    TCanvas *can = MakeCanvas("canv", "histograms", 800, 600);
    // can->Divide(1,1); //Can be used to split canvas into multiple pads
    // can->cd(1);
  
    Combined->Draw("hist same");
    ZZWWcombined->Draw("hist same");
    WW_Hist->Draw("hist same");
 
    TLegend* leg = new TLegend(0.75, 0.65, 0.95, 0.90);
    SetLegendStyle(leg);
    leg->AddEntry(Combined,"ZH","l");
    leg->AddEntry(ZZWWcombined,"ZZ","f");
    leg->AddEntry(WW_Hist,"WW","f");
    leg->Draw();

    //Codes for a text box on the plot to display information    
    TPaveText* pt = new TPaveText(0.2,0.886,0.7, 1, "tlbrNDC"); // NDC sets coords// relative to pad dimensions
    pt->SetFillStyle(0); 
    pt->SetTextSize(0.0231884); 
    pt->SetTextAlign(12);
    pt->SetBorderSize(0);
    pt->SetTextFont(62);
    pt->SetTextColor(1);

    TString str_lumi;
    if (lumi - ((Int_t) lumi) == 0){
        str_lumi = TString::Format("%.0f",lumi);
    }
    else{
        str_lumi = TString::Format("%.1f",lumi);
    }
    pt->AddText("ILD, " + str_lumi + " ab^{-1} at #sqrt{s} = 240 GeV");
    pt->Draw();

    // Yield Text Box
    // TPaveText* pt2 = new TPaveText(0.81,0.74,0.90,0.857971, "tlbrNDC");
    // pt2->SetFillStyle(0); 
    // pt2->SetTextSize(0.0231884); 
    // pt2->SetTextAlign(30);
    // pt2->SetBorderSize(0);
    // pt2->SetTextFont(62);
    // pt2->SetTextColor(1);
    // TString sfraction = TString::Format("%.f",ZHyield);
    // pt2->AddText("ZH yield:  " + sfraction);
    // sfraction = TString::Format("%.f",ZZyield);
    // pt2->AddText("ZZ yield: " + sfraction);
    // sfraction = TString::Format("%.f",WWyield);
    // pt2->AddText("WW yield: " + sfraction);
    // pt2->Draw();       //to draw your text object

    can->RedrawAxis();
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
void processtree(TString sample, Double_t weight, TLorentzVector CME, TString filename, TString directory, TString cut, Int_t nbins, Double_t xmin, Double_t xmax,std::clock_t start){

    //Open File
    TFile *f = new TFile(directory);
    TTree *ttree = (TTree*)f->Get(sample);

    TString recohistname = "_recoilmassHIST";
    TString histname = sample + recohistname;
    TH1D *recoilmassHIST = new TH1D(histname,histname, nbins, xmin, xmax);
    ttree->Project(histname, "recoilmass", cut);

    //Normalize Hist
    recoilmassHIST->Scale(weight);

    TFile g(filename, "update");
    recoilmassHIST->Write();
    g.Close();
}

// For a simulated collision with multiple physics processes as individual delphes files, calculates relevant histograms into one root file. 
void tree_awesomeHist(){
    TLorentzVector CME = {0,0,0,240}; // Defines center of mass energy
    TString filename = "hists/treeHists.root"; // Filename for created histogram collection
    TString directory = "ntuples/zhINVzzwwFSRv2.root"; //Input tree

    Double_t lum = 10;
    setTDRStyle();
    Double_t xmin = 80; 
    Double_t xmax = 160;
    Int_t nbins = 200;

    Double_t GeVperBin = (xmax - xmin)/((Double_t) nbins);
    TString str_gev = TString::Format("%.1f", GeVperBin);
    
    TString xtitle = "Missing Mass [GeV]";
    TString ytitle = "Events / " + str_gev + " GeV";

    //data_obs needed for combine tool
    TH1D* data_obs = new TH1D("data_obs", "data_obs", nbins, xmin, xmax); 
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
    TString photonjetveto = "!( (photonjetveto == 1) && (numJets > 2) )";
    TString jetveto2 = "(jetveto2 == 0)";
    TString PTcut = "( (PT1 >= 20) && (PT2 >= 20) )";
    TString lepton = "( ( (flavor == 0) && (numElectrons == 2) && (numMuons == 0) ) || ( (flavor == 1) && (numElectrons == 0) && (numMuons == 2) ) )";

    //List the cuts here and turn them on and off with 0 or 1
    std::map<Int_t, std::pair<TString, Int_t> > cuts ={ 
    { 1, std::pair<TString, Int_t> (jetveto,          0)},
    { 2, std::pair<TString, Int_t> (num_FSRcand,      0)},
    { 3, std::pair<TString, Int_t> (charge,           1)},
    { 4, std::pair<TString, Int_t> (labangle,         1)}, 
    { 5, std::pair<TString, Int_t> (acoangle,         1)},
    { 6, std::pair<TString, Int_t> (diparticlept,     1)},
    { 7, std::pair<TString, Int_t> (diparticlepl,     1)},
    { 8, std::pair<TString, Int_t> (zmass,            1)},
    { 9, std::pair<TString, Int_t> (lepton,           1)},

    { 10, std::pair<TString, Int_t> (zmass2,          0)},
    { 11, std::pair<TString, Int_t> (photonjetveto,   0)},
    { 12, std::pair<TString, Int_t> (jetveto2,        0)},
    { 13, std::pair<TString, Int_t> (PTcut,           0)}
    
    };

    //This combines the above defined cuts that are on (have value 1) into a single string to be passed
    // as a parameter
    TString total_cut = get_total_cuts(cuts); // + " && (flavor == 0)" //This can be added to look at e or mu only
    cout << "Total cuts: " + total_cut << endl;

    //Define your samples name here as they are in your sample directory
    //Note the number is the normalization constant for that sample, not the cross section!
    //weight = xsec*lumi/total_unweighted_events

    std::map<TString, Double_t> samples ={ // Defines which samples to process and their corresponding cross sections {"sample name", weight}
    // { "eeZHinv",  0.0529862}, //3.7 mil events
    { "eeZH_SM", 0.257656}, //0.8 mil events
    // { "eeZH_SM_AramDelphes", 0.255364} //0.8 mil events

    { "eeZZ", 0.523514},// 5.1 mil events
    // { "eeZZnoISR", 2.96024},

    { "eeWW", 5.38153} //7.1 mil events
    // { "eeWWnoISR", 20.7019}
    };

    std::clock_t start;
    start = std::clock();

    // Runs over all selected samples and creates histograms of di-particle mass and missing mass
    for (Int_t i=0; i<1; i++){
        std::map<TString, Double_t>::iterator process;
        for ( process = samples.begin(); process != samples.end(); process++){
            cout << "Found process " << process->first << endl;
            if (process->second != 0.0){
                processtree(process->first, process->second, CME, filename, directory, total_cut, nbins, xmin, xmax, start);
            }
        }
    }

    double duration;
    duration = (( std::clock() - start ) / (double) CLOCKS_PER_SEC)/60;
    cout<< "Process completed in "<< duration << " minutes" <<'\n';

    draw_hist(filename, lum, xtitle); //draws the histogram if you want to see it
    //Otherwise histograms can be drawn later from the created histogram collection
}








