// E+E- Delphes Sample Drawing for ee->bbvv final state
// Alexander Andriatis
// 17 June 2016

// Takes a .root collection of histograms created by hist_process.C and saves a pretty histogram as .pdf

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
#include <iostream>
#include <fstream>
#include <algorithm>
#include <string>
#include <sstream>
#include <TH1F.h>
#include <TFile.h>
#include <TROOT.h>
#include <TString.h>
#include <TSystem.h>
#include <Rtypes.h>

#include <TMath.h>
#include <TAxis.h>
#include <TCanvas.h>
#include <TLegend.h>
#include <TAttLine.h>
#include <TPaveText.h>
#include <TColor.h>

#include "TGraphAsymmErrors.h"

#include "TTree.h"
#include "TCanvas.h"

#include "HttStyles.h"

#endif

using namespace TMath;
using namespace std;

//Main Function
void pretty_draw(){

    TString file = "ee_350";
    TString root = ".root";
    TString filename;

    // Plots the signal and background di-jet mass and missing mass
    setTDRStyle();

    TCanvas *can0 = MakeCanvas("Recoil_Mass", "Recoil_Mass", 600, 600);

        can0->cd();
        filename = "GENERATIONPATH/analysis/" + file+root;
        TFile *f = new TFile(filename); // Specifies the root files which contain the generated histograms

        TH1D *ee_uuh_recoilmassHIST = (TH1D*)f->Get("ee_uuh_recoilmassHIST");
        TH1D *ee_uuh_zh_recoilmassHIST = (TH1D*)f->Get("ee_uuh_zh_recoilmassHIST");
        TH1D *ee_uubb_recoilmassHIST = (TH1D*)f->Get("ee_uubb_recoilmassHIST");
        TH1D *ee_uucc_recoilmassHIST = (TH1D*)f->Get("ee_uucc_recoilmassHIST");

        TH1D *ee_uuh_ww_recoilmassHIST = new TH1D("subtracted", "subtracted", 350/2, 0, 350);
        *ee_uuh_ww_recoilmassHIST = *ee_uuh_recoilmassHIST - *ee_uuh_zh_recoilmassHIST;
        ee_uuh_ww_recoilmassHIST->SetName("ee_uuh_ww_recoilmassHIST");

        InitHist(ee_uuh_ww_recoilmassHIST, "Missing Mass (GeV)", "Events / 2 GeV", TColor::GetColor(248,41,104), 1001);
        InitHist(ee_uuh_zh_recoilmassHIST, "Missing Mass (GeV)", "Events / 2 GeV", TColor::GetColor(248,206,104), 1001);
        InitHist(ee_uubb_recoilmassHIST, "Missing Mass (GeV)", "Events / 2 GeV", TColor::GetColor(125,206,206), 1001);
        InitHist(ee_uucc_recoilmassHIST, "Missing Mass (GeV)", "Events / 2 GeV", TColor::GetColor(125,206,78), 1001);

        ee_uuh_ww_recoilmassHIST->SetMinimum(0);
        ee_uuh_ww_recoilmassHIST->SetMaximum(ee_uuh_ww_recoilmassHIST->GetMaximum()*1.1);
        ee_uuh_ww_recoilmassHIST->GetXaxis()->SetRangeUser(50, 250);

        ee_uuh_ww_recoilmassHIST->Draw("hist same");
        ee_uuh_zh_recoilmassHIST->Draw("hist same");
        ee_uubb_recoilmassHIST->Draw("hist same");
        ee_uucc_recoilmassHIST->Draw("hist same");

        TLegend* leg;
          leg = new TLegend(0.25, 0.70,0.95, 0.85);
          leg->SetBorderSize( 0 );
          leg->SetFillStyle( 0 );
          leg->SetFillColor(kWhite);
          leg->AddEntry( ee_uuh_ww_recoilmassHIST , "WW",  "F" );
          leg->AddEntry( ee_uuh_zh_recoilmassHIST , "ZH",  "F" );
          leg->AddEntry( ee_uubb_recoilmassHIST , "b#bar{b}",  "F" );
          leg->AddEntry( ee_uucc_recoilmassHIST , "c#bar{c}",  "F" );
          leg->Draw("same");
     
        TPaveText* pt = new TPaveText(0.3,0.55,0.4, 0.65, "tlbrNDC"); // NDC sets coords// relative to pad dimensions
            pt->SetFillStyle(0); 
            pt->SetTextSize(0.05);
            pt->SetBorderSize(0);
            pt->SetTextColor(1);
            pt->AddText("#sqrt{s} = 350 GeV");
            pt->AddText("2.6 ab^{-1}");
            pt->Draw();

        can0->RedrawAxis();
        can0->Print("",".pdf");

    TCanvas *can1 = MakeCanvas("Di-Jet_Mass", "Di-Jet_Mass", 600, 600);
    
        can1->cd();
        filename = "GENERATIONPATH/analysis/" + file+root;
        TFile *g = new TFile(filename); // Specifies the root files which contain the generated histograms

        TH1D *ee_uuh_massHIST = (TH1D*)f->Get("ee_uuh_massHIST");
        TH1D *ee_uuh_zh_massHIST = (TH1D*)f->Get("ee_uuh_zh_massHIST");
        TH1D *ee_uubb_massHIST = (TH1D*)f->Get("ee_uubb_massHIST");
        TH1D *ee_uucc_massHIST = (TH1D*)f->Get("ee_uucc_massHIST");

        TH1D *ee_uuh_ww_massHIST = new TH1D("subtracted", "subtracted", 350/2, 0, 350);
        *ee_uuh_ww_massHIST = *ee_uuh_massHIST - *ee_uuh_zh_massHIST;
        ee_uuh_ww_massHIST->SetName("ee_uuh_ww_massHIST");

        InitHist(ee_uuh_ww_massHIST, "Di-Jet Mass (GeV)", "Events / 2 GeV", TColor::GetColor(248,41,104), 1001);
        InitHist(ee_uuh_zh_massHIST, "Di-Jet Mass (GeV)", "Events / 2 GeV", TColor::GetColor(248,206,104), 1001);
        InitHist(ee_uubb_massHIST, "Di-Jet Mass (GeV)", "Events / 2 GeV", TColor::GetColor(125,206,206), 1001);
        InitHist(ee_uucc_massHIST, "Di-Jet Mass (GeV)", "Events / 2 GeV", TColor::GetColor(125,206,78), 1001);

        ee_uuh_ww_massHIST->SetMinimum(0);
        ee_uuh_ww_massHIST->SetMaximum(ee_uuh_ww_massHIST->GetMaximum()*1.1);
        ee_uuh_ww_massHIST->GetXaxis()->SetRangeUser(70, 140);

        ee_uuh_ww_massHIST->Draw("hist same");
        ee_uuh_zh_massHIST->Draw("hist same");
        ee_uubb_massHIST->Draw("hist same");
        ee_uucc_massHIST->Draw("hist same");

        TLegend* leg2;
          leg2 = new TLegend(0.25, 0.70,0.95, 0.85);
          leg2->SetBorderSize( 0 );
          leg2->SetFillStyle( 0 );
          leg2->SetFillColor(kWhite);
          leg2->AddEntry( ee_uuh_ww_massHIST , "WW",  "F" );
          leg2->AddEntry( ee_uuh_zh_massHIST , "ZH",  "F" );
          leg2->AddEntry( ee_uubb_massHIST , "b#bar{b}",  "F" );
          leg2->AddEntry( ee_uucc_massHIST , "c#bar{c}",  "F" );
          leg2->Draw("same");
     
        TPaveText* pt2 = new TPaveText(0.3,0.55,0.4, 0.65, "tlbrNDC"); // NDC sets coords// relative to pad dimensions
            pt2->SetFillStyle(0); 
            pt2->SetTextSize(0.05);
            pt2->SetBorderSize(0);
            pt2->SetTextColor(1);
            pt2->AddText("#sqrt{s} = 350 GeV");
            pt2->AddText("2.6 ab^{-1}");
            pt2->Draw();

        can1->RedrawAxis();
        can1->Print("",".pdf");

}

