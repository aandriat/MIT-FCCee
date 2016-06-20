// E+E- Delphes Sample Drawing for ee->bbvv final state
// Alexander Andriatis
// 17 June 2016

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
#include "TMath.h"              //  -
#include <TLatex.h> 
#include <TPaveText.h>

#include "modules/Delphes.h"                   //   |
#include "ExRootAnalysis/ExRootTreeReader.h"   //   -  Include needed delphes headers
#include "classes/DelphesClasses.h"            //   |

#endif

using namespace TMath;
using namespace std;

//Main Function
void hist_draw(){
    TFile *f = new TFile("eebb.root"); 
    
    //defines histograms for bjet reconstructed mass
    TH1D* h1_visible = (TH1D*)f->Get("eebb_uu_zh_massHIST");
    TH1D* h1_missing = (TH1D*)f->Get("eebb_uu_zh_recoilmassHIST");
    TH1D* h2_visible = (TH1D*)f->Get("eebb_uu_massHIST");
    TH1D* h2_missing = (TH1D*)f->Get("eebb_uu_recoilmassHIST");

    TH1D* h_difference1_visible = new TH1D("h_difference1_visible", "h_difference1_visible", 100, 0, 350);    
    TH1D* h_difference1_missing = new TH1D("h_difference1_missing", "h_difference1_missing", 100, 0, 350);   

    *h_difference1_visible = (*h2_visible)-(*h1_visible);
    h_difference1_visible->SetName("h_difference1_visible");
    *h_difference1_missing = (*h2_missing)-(*h1_missing);
    h_difference1_missing->SetName("h_difference1_missing");

    Double_t signal = h_difference1_visible->Integral();
    Double_t total = h2_visible->Integral();
    Double_t fraction = signal/total;
    TString sfraction = TString::Format("%.2f",fraction);

    // Reconstructed Higgs Mass 
    TCanvas *can = new TCanvas("FCC BJet uu_10 350 nocut", "FCC BJet uu_10 350 nocut", 1280, 720);
    can->Divide(1,2);
    can->cd(1);
    h2_visible->GetXaxis()->SetTitle("Reconstructed Mass (GeV)");
    h2_visible->GetYaxis()->SetTitle("Particle Count");
    h2_visible->SetLineColor(kRed);
    h2_visible->SetMaximum(20000);
    h2_visible->SetMinimum(0);
    h2_visible->SetStats(kFALSE);
    h2_visible->SetTitle("Reconstructed Mass of Di-Jet System @ #sqrt{s} = 350 Gev w/o cuts");
    h2_visible->Draw("hist");

    h1_visible->GetXaxis()->SetTitle("Reconstructed Mass (GeV)");
    h1_visible->GetYaxis()->SetTitle("Particle Count");
    h1_visible->SetLineColor(kBlue);
    h1_visible->SetFillStyle(3004);
    h1_visible->SetFillColor(kBlue);
    h1_visible->SetStats(kFALSE);
    h1_visible->Draw("hist same");

    h_difference1_visible->GetXaxis()->SetTitle("Reconstructed Mass (GeV)");
    h_difference1_visible->GetYaxis()->SetTitle("Particle Count");
    h_difference1_visible->SetLineColor(kGreen);
    h_difference1_visible->SetFillStyle(3005);
    h_difference1_visible->SetFillColor(kGreen);
    h_difference1_visible->SetStats(kFALSE);
    h_difference1_visible->Draw("hist same");

    TLegend* leg = new TLegend(0.13,0.79,0.25,0.89);
    leg->AddEntry(h2_visible,"Signal","l");
    leg->AddEntry(h1_visible,"HZ","f");
    leg->AddEntry(h_difference1_visible,"WW Fusion and Interference", "f");
    leg->Draw();

    TPaveText* pt = new TPaveText(0.833,0.8,0.883,0.88, "NDC"); 
    pt->SetFillColor(0); // text is black on white
    pt->SetTextSize(0.03); 
    pt->SetTextAlign(22);
    pt->SetTextFont(42);
    pt->AddText(sfraction + " yield");
    pt->SetShadowColor(0);
    pt->Draw();

    can->cd(2);

    h2_missing->GetXaxis()->SetTitle("Missing Mass (GeV)");
    h2_missing->GetYaxis()->SetTitle("Particle Count");
    h2_missing->SetLineColor(kRed);
    h2_missing->SetMaximum(12000);
    h2_missing->SetMinimum(0);
    h2_missing->SetStats(kFALSE);
    h2_missing->SetTitle("Missing Mass of Di-Jet System @ #sqrt{s} = 350 Gev w/o cuts");
    h2_missing->Draw("hist");

    h1_missing->GetXaxis()->SetTitle("Missing Mass (GeV)");
    h1_missing->GetYaxis()->SetTitle("Particle Count");
    h1_missing->SetLineColor(kBlue);
    h1_missing->SetFillStyle(3004);
    h1_missing->SetFillColor(kBlue);
    h1_missing->SetStats(kFALSE);
    h1_missing->Draw("hist same");

    h_difference1_missing->GetXaxis()->SetTitle("Missing Mass (GeV)");
    h_difference1_missing->GetYaxis()->SetTitle("Particle Count");
    h_difference1_missing->SetLineColor(kGreen);
    h_difference1_missing->SetFillStyle(3005);
    h_difference1_missing->SetFillColor(kGreen);
    h_difference1_missing->SetStats(kFALSE);
    h_difference1_missing->Draw("hist same");

    TLegend* leg2 = new TLegend(0.13,0.79,0.25,0.89);
    leg2->AddEntry(h2_missing,"Signal","l");
    leg2->AddEntry(h1_missing,"HZ","f");
    leg2->AddEntry(h_difference1_missing,"WW Fusion and Interference", "f");
    leg2->Draw();

    can->Print("",".pdf");
}

