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
    TFile *f = new TFile("eebb_350_massHIST.root"); // Specifies the root files which contain the generated histograms
    TFile *g = new TFile("eebb_350_recoilmassHIST.root"); // Specifies the root files which contain the generated histograms
    Double_t signal, overall, fraction;
    TString sfraction;

    TH1D* subtracted = new TH1D("subtracted", "subtracted", 100, 0, 350);    
    TH1D* subtracted2 = new TH1D("subtracted2", "subtracted2", 100, 0, 350);   

    // Plots the signal and background di-jet mass and missing mass
    TCanvas *can = new TCanvas("eebb_350", "eebb_350", 1280, 720);
    can->Divide(1,2);

    //Di-Jet Mass
    can->cd(1);

    TH1D *total = (TH1D*)f->Get("eebb_uu_350_massHIST");
    TH1D *zh = (TH1D*)f->Get("eebb_uu_zh_350_massHIST");
    TH1D *ww = (TH1D*)f->Get("eebb_uu_ww_350_massHIST");
    *subtracted  = (*total) - (*zh);
    subtracted->SetName("eebb_uu_sub_350_massHIST");

    // Outputs what fraction of the total process is the signal
    signal = ww->Integral();
    overall = total->Integral();
    fraction = signal/overall;
    sfraction = TString::Format("%.2f",fraction);

    total->GetXaxis()->SetTitle("Reconstructed Mass (GeV)");
    total->GetYaxis()->SetTitle("Particle Count");
    total->SetLineColor(kRed);
    total->SetMaximum(12000);
    total->SetMinimum(0);
    total->SetStats(kFALSE);
    total->SetTitle("Reconstructed Mass of Di-Jet System @ #sqrt{s} = 350 Gev w/o cuts");
    total->Draw("hist same");

    zh->GetXaxis()->SetTitle("Reconstructed Mass (GeV)");
    zh->GetYaxis()->SetTitle("Particle Count");
    zh->SetLineColor(kBlue);
    zh->SetFillStyle(3004);
    zh->SetFillColor(kBlue);
    zh->SetStats(kFALSE);
    zh->Draw("hist same");

    ww->GetXaxis()->SetTitle("Reconstructed Mass (GeV)");
    ww->GetYaxis()->SetTitle("Particle Count");
    ww->SetLineColor(kGreen);
    ww->SetFillStyle(3005);
    ww->SetFillColor(kGreen);
    ww->SetStats(kFALSE);
    ww->Draw("hist same");

    subtracted->GetXaxis()->SetTitle("Reconstructed Mass (GeV)");
    subtracted->GetYaxis()->SetTitle("Particle Count");
    subtracted->SetLineColor(kMagenta);
    subtracted->SetLineStyle(2);
    subtracted->SetStats(kFALSE);
    subtracted->Draw("hist same");


    TLegend* leg = new TLegend(0.13,0.79,0.25,0.89);
    leg->AddEntry(total,"Total","l");
    leg->AddEntry(zh,"ZH","f");
    leg->AddEntry(ww,"WW Fusion and Interference", "f");
    leg->AddEntry(subtracted,"subtracted", "l");
    leg->Draw();

    TPaveText* pt = new TPaveText(0.833,0.8,0.883,0.88, "NDC"); 
    pt->SetFillColor(0); // text is black on white
    pt->SetTextSize(0.03); 
    pt->SetTextAlign(22);
    pt->SetTextFont(42);
    pt->AddText(sfraction + " yield");
    pt->SetShadowColor(0);
    pt->Draw();

    // Missing Mass
    can->cd(2);

    TH1D *total2 = (TH1D*)g->Get("eebb_uu_350_recoilmassHIST");
    TH1D *zh2 = (TH1D*)g->Get("eebb_uu_zh_350_recoilmassHIST");
    TH1D *ww2 = (TH1D*)g->Get("eebb_uu_ww_350_recoilmassHIST");
    *subtracted2  = (*total2) - (*zh2);
    subtracted2->SetName("eebb_uu_sub_350_recoilmassHIST");

    // Outputs what fraction of the total process is the signal
    signal = ww2->Integral();
    overall = total2->Integral();
    fraction = signal/overall;
    sfraction = TString::Format("%.2f",fraction);

    total2->GetXaxis()->SetTitle("Missing Mass (GeV)");
    total2->GetYaxis()->SetTitle("Particle Count");
    total2->SetLineColor(kRed);
    total2->SetMaximum(2500);
    total2->SetMinimum(0);
    total2->SetStats(kFALSE);
    total2->SetTitle("Missing Mass of Di-Jet System @ #sqrt{s} = 350 Gev w/o cuts");
    total2->Draw("hist same");

    zh2->GetXaxis()->SetTitle("Missing Mass (GeV)");
    zh2->GetYaxis()->SetTitle("Particle Count");
    zh2->SetLineColor(kBlue);
    zh2->SetFillStyle(3004);
    zh2->SetFillColor(kBlue);
    zh2->SetStats(kFALSE);
    zh2->Draw("hist same");

    ww2->GetXaxis()->SetTitle("Missing Mass (GeV)");
    ww2->GetYaxis()->SetTitle("Particle Count");
    ww2->SetLineColor(kGreen);
    ww2->SetFillStyle(3005);
    ww2->SetFillColor(kGreen);
    ww2->SetStats(kFALSE);
    ww2->Draw("hist same");

    subtracted2->GetXaxis()->SetTitle("Missing Mass (GeV)");
    subtracted2->GetYaxis()->SetTitle("Particle Count");
    subtracted2->SetLineColor(kMagenta);
    subtracted2->SetLineStyle(2);
    subtracted2->SetStats(kFALSE);
    subtracted2->Draw("hist same");


    TLegend* leg2 = new TLegend(0.13,0.79,0.25,0.89);
    leg2->AddEntry(total2,"Total","l");
    leg2->AddEntry(zh2,"ZH","f");
    leg2->AddEntry(ww2,"WW Fusion and Interference", "f");
    leg2->AddEntry(subtracted2,"subtracted", "l");
    leg2->Draw();

    TPaveText* pt2 = new TPaveText(0.833,0.8,0.883,0.88, "NDC"); 
    pt2->SetFillColor(0); // text is black on white
    pt2->SetTextSize(0.03); 
    pt2->SetTextAlign(22);
    pt2->SetTextFont(42);
    pt2->AddText(sfraction + " yield");
    pt2->SetShadowColor(0);
    pt2->Draw();    

    // Saves as PDF
    can->Print("",".pdf");
}

