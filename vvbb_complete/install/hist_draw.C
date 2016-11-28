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

#endif

using namespace TMath;
using namespace std;

//Main Function
void hist_draw(){

    TString file = "ee_350";
    TString root = ".root";
    TString name, histtype, filename;

    std::map<TString, Int_t > histnames ={ // Defines which samples to process}
  //          { "ee_uuh_ww", 0},
        { "ee_uuh_zh", 1},
        { "ee_uubb", 1},
        { "ee_uucc", 1},
        { "ee_qq", 1},
        { "ee_qqa", 1},
        { "ee_qqll", 1},
        { "ee_qqlv", 1},
        { "ee_uuqq", 1},
        { "ee_qqqq", 1},
        { "ee_uuh", 1},
        { "subtracted", 2},
        { "data_obs", 2}
    };

    std::map<Int_t, TString> histtypes ={ // Defines which samples to process and their corresponding cross sections {index, <"sample name", cross section>}
        { 2, "_recoilmassHIST"},
        { 1, "_massHIST"}
    };

    std::map<TString, TString > titles ={ // Defines which samples to process and their corresponding cross sections {index, <"sample name", cross section>}
        { "_recoilmassHIST", "Missing Mass of Di-Jet System @ #sqrt{s} = 350 GeV"},
        { "_massHIST" , "Di-Jet Mass @ #sqrt{s} = 350 GeV"}
    };

    std::map<TString, TString > xtitles ={ // Defines which samples to process and their corresponding cross sections {index, <"sample name", cross section>}
        { "_recoilmassHIST", "Missing Mass (GeV)"},
        { "_massHIST", "Di-Jet Mass (GeV)"}
    };

    std::map<TString, TString > ytitles ={ // Defines which samples to process and their corresponding cross sections {index, <"sample name", cross section>}
        { "_recoilmassHIST", "Count"},
        { "_massHIST", "Count"}
    };

    
    // Plots the signal and background di-jet mass and missing mass
    TCanvas *can = new TCanvas(file, file, 1280, 720);
    can->Divide(1, 2);

    for (Int_t counter=1; counter<3; counter++){
        histtype = histtypes[counter];
        can->cd(counter);

        filename = file+root;
        TFile *f = new TFile(filename); // Specifies the root files which contain the generated histograms

        std::map<TString, TH1D*> histograms;
        std::map<TString, Int_t>::iterator names;
        for ( names = histnames.begin(); names != histnames.end(); names++){
            if (names->second == 1){
                name = names->first + histtype;
                histograms.insert ( std::pair<TString, TH1D*> (names->first, (TH1D*)f->Get(name)));
            }
        }

        if (histnames["subtracted"]==2){
            histograms.insert ( std::pair<TString, TH1D*> ("subtracted", new TH1D("subtracted", "subtracted", 350/2, 0, 350)));
            *((histograms)["subtracted"]) = *((histograms)["ee_uuh"]) - *((histograms)["ee_uuh_zh"]);
            TString subname = "ee_uuh_ww"+histtype;
            (histograms)["subtracted"]->SetName(subname);
        }

        Double_t largest=0;
        std::map<TString, TH1D*>::iterator hists;
        for ( hists = histograms.begin(); hists != histograms.end(); hists++){
            Double_t newlargest = hists->second->GetMaximum();
            if (newlargest > largest){
                largest = newlargest;
            }
        }
        largest = 1.1*largest;

        std::map<TString, TH1D*>::iterator histsit;
        for (histsit = histograms.begin(); histsit != histograms.end(); histsit++){
            name = histsit->first;
            (histograms)[name]->SetTitle(titles[histtype]);
            (histograms)[name]->GetXaxis()->SetTitle(xtitles[histtype]);
            (histograms)[name]->GetYaxis()->SetTitle(ytitles[histtype]);
            (histograms)[name]->SetMinimum(0);
            (histograms)[name]->SetMaximum(largest);
        }

        // name = "ee_uuh_ww";
        // if ((histograms)[name]){
        // (histograms)[name]->SetLineColor(kRed);
        // (histograms)[name]->SetFillStyle(1001);
        // (histograms)[name]->SetFillColor(kRed);
        // (histograms)[name]->SetStats(kTRUE);
        // (histograms)[name]->Draw("hist same");
        // }

        name = "subtracted";
        if ((histograms)[name]){
        (histograms)[name]->SetLineColor(kRed);
        (histograms)[name]->SetFillStyle(1001);
        (histograms)[name]->SetFillColor(kRed);
        (histograms)[name]->SetStats(kTRUE);
        (histograms)[name]->Draw("hist same");
        }

        name = "ee_uuh_zh";
        (histograms)[name]->SetLineColor(kMagenta);
        (histograms)[name]->SetFillStyle(3004);
        (histograms)[name]->SetFillColor(kMagenta);
        (histograms)[name]->SetStats(kFALSE);
        (histograms)[name]->Draw("hist same");

        name = "ee_uubb";
        (histograms)[name]->SetLineColor(kBlue);
        (histograms)[name]->SetFillStyle(3005);
        (histograms)[name]->SetFillColor(kBlue);
        (histograms)[name]->SetStats(kFALSE);
        (histograms)[name]->Draw("hist same");

        name = "ee_uucc";
        (histograms)[name]->SetLineColor(kCyan);
        (histograms)[name]->SetFillStyle(3006);
        (histograms)[name]->SetFillColor(kCyan);
        (histograms)[name]->SetStats(kFALSE);
        (histograms)[name]->Draw("hist same");

        name = "ee_qq";
        (histograms)[name]->SetLineColor(kGreen);
        (histograms)[name]->SetFillStyle(3007);
        (histograms)[name]->SetFillColor(kGreen);
        (histograms)[name]->SetStats(kFALSE);
        (histograms)[name]->Draw("hist same");

        name = "ee_qqa";
        (histograms)[name]->SetLineColor(kYellow);
        (histograms)[name]->SetFillStyle(3008);
        (histograms)[name]->SetFillColor(kYellow);
        (histograms)[name]->SetStats(kFALSE);
        (histograms)[name]->Draw("hist same");

        name = "ee_qqll";
        (histograms)[name]->SetLineColor(kGreen+1);
        (histograms)[name]->SetFillStyle(3009);
        (histograms)[name]->SetFillColor(kGreen+1);
        (histograms)[name]->SetStats(kFALSE);
        (histograms)[name]->Draw("hist same");

        name = "ee_qqlv";
        (histograms)[name]->SetLineColor(kYellow+1);
        (histograms)[name]->SetFillStyle(3010);
        (histograms)[name]->SetFillColor(kYellow+1);
        (histograms)[name]->SetStats(kFALSE);
        (histograms)[name]->Draw("hist same");

        name = "ee_uuqq";
        (histograms)[name]->SetLineColor(kGreen+2);
        (histograms)[name]->SetFillStyle(3011);
        (histograms)[name]->SetFillColor(kGreen+2);
        (histograms)[name]->SetStats(kFALSE);
        (histograms)[name]->Draw("hist same");

        name = "ee_qqqq";
        (histograms)[name]->SetLineColor(kYellow+2);
        (histograms)[name]->SetFillStyle(3012);
        (histograms)[name]->SetFillColor(kYellow+2);
        (histograms)[name]->SetStats(kFALSE);
        (histograms)[name]->Draw("hist same");


        TLegend* leg = new TLegend(0.13,0.69,0.25,0.89);
        std::map<TString, TH1D*>::iterator histdrawer;
        for (histdrawer = histograms.begin(); histdrawer != histograms.end(); histdrawer++){
            name = histdrawer->first;
            //(histograms)[name]->Draw("hist same");
            leg->AddEntry((histograms)[name],name,"f");
        }
        leg->Draw();

        TFile *g = new TFile(filename, "update"); // Specifies the root files which contain the generated histograms
            if (histnames["data_obs"]==2){
                histograms.insert ( std::pair<TString, TH1D*> ("data_obs", new TH1D("data_obs", "data_obs", 350/2, 0, 350)));
                if (histnames["subtracted"]==2){
                    *((histograms)["data_obs"]) = *((histograms)["subtracted"]);
                } else {
                    *((histograms)["data_obs"]) = *((histograms)["ee_uuh_ww"]);
                }
                (histograms)["data_obs"]->SetName("data_obs");
                (histograms)["data_obs"]->Write();
            }
            if (histnames["subtracted"]==2){
                (histograms)["subtracted"]->Write();
            }
        g->Close();

    }
        // Saves as PDF
    can->Print("",".pdf");

}

