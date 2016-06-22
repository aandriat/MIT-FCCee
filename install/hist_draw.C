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

#include "modules/Delphes.h"                   //   |
#include "ExRootAnalysis/ExRootTreeReader.h"   //   -  Include needed delphes headers
#include "classes/DelphesClasses.h"            //   |

#endif

using namespace TMath;
using namespace std;

//Main Function
void hist_draw(){

    TString file = "process";
    TString filename, name1, name2, name3, namesub;
    TString root = ".root";
    Double_t signal, overall, fraction;
    TString sfraction;
    Double_t max1, max2, max3, max4, largest;

    std::map<Int_t, TString > histnames ={ // Defines which samples to process and their corresponding cross sections {index, <"sample name", cross section>}
        { 1, "process_signal"},
        { 2, "process_background1"},
        { 3, "process_background2"},
        { 4, "process_background3"}
    };
    std::map<Int_t, TString > histtypes ={ // Defines which samples to process and their corresponding cross sections {index, <"sample name", cross section>}
        { 1, "_massHIST"},
        { 2, "_recoilmassHIST"}
    };

    std::map<Int_t, TString > titles ={ // Defines which samples to process and their corresponding cross sections {index, <"sample name", cross section>}
        { 1, "Reconstructed Mass of Di-Jet System @ #sqrt{s} = 350 Gev w/ cuts"},
        { 2, "Missing Mass of Di-Jet System @ #sqrt{s} = 350 Gev w/ cuts"}
    };

    std::map<Int_t, TString > xtitles ={ // Defines which samples to process and their corresponding cross sections {index, <"sample name", cross section>}
        { 1, "Reconstructed Mass (GeV)"},
        { 2, "Missing Mass (GeV)"}
    };

    std::map<Int_t, TString > ytitles ={ // Defines which samples to process and their corresponding cross sections {index, <"sample name", cross section>}
        { 1, "Particle Count"},
        { 2, "Particle Count"}
    };

    
    // Plots the signal and background di-jet mass and missing mass
    TCanvas *can = new TCanvas(file, file, 1280, 720);
    can->Divide(1,histtypes.size());

    for (Int_t i=1; i< histtypes.size()+1; i++){

        can->cd(i);

        filename = file+histtypes[i]+root;
        name1 = histnames[1]+histtypes[i];
        name2 = histnames[2]+histtypes[i];
        name3 = histnames[3]+histtypes[i];
        namesub = histnames[4]+histtypes[i];

        TFile *f = new TFile(filename); // Specifies the root files which contain the generated histograms

        TH1D *total = (TH1D*)f->Get(name1);
        TH1D *zh = (TH1D*)f->Get(name2);
        TH1D *vvbb = (TH1D*)f->Get(name3);

        TH1D* subtracted = new TH1D("subtracted", "subtracted", 100, 0, 350);  
        TH1D* data_obs = new TH1D("data_obs", "data_obs", 100, 0, 350);  
        *subtracted  = (*total) - (*zh);
        subtracted->SetName(namesub);
        *data_obs = *subtracted;
        data_obs->SetName("data_obs");

        // Outputs what fraction of the total process is the signal
        signal = subtracted->Integral();
        overall = total->Integral();
        fraction = signal/overall;
        sfraction = TString::Format("%.2f",fraction);


        max1 = total->GetMaximum();
        max2 = zh->GetMaximum();
        max3 = vvbb->GetMaximum();
        max4 = subtracted->GetMaximum();
        Double_t range[4] = {max1,max2,max3,max4};
        largest = 0;
        for(Int_t i=0; i<4; i++)
        {
            if(range[i]>largest)
            largest=range[i];
        }
        largest+=largest*0.1;
        

        total->SetTitle(titles[i]);
        total->GetXaxis()->SetTitle(xtitles[i]);
        total->GetYaxis()->SetTitle(ytitles[i]);
        total->SetMinimum(0);
        total->SetMaximum(largest);
        total->SetLineColor(kRed);
        total->SetStats(kFALSE);
        total->Draw("hist same");

        zh->SetTitle(titles[i]);
        zh->GetXaxis()->SetTitle(xtitles[i]);
        zh->GetYaxis()->SetTitle(ytitles[i]);
        zh->SetMinimum(0);
        zh->SetMaximum(largest);
        zh->SetLineColor(kBlue);
        zh->SetFillStyle(3004);
        zh->SetFillColor(kBlue);
        zh->SetStats(kFALSE);
        zh->Draw("hist same");

        vvbb->SetTitle(titles[i]);
        vvbb->GetXaxis()->SetTitle(xtitles[i]);
        vvbb->GetYaxis()->SetTitle(ytitles[i]);
        vvbb->SetMinimum(0);
        vvbb->SetMaximum(largest);
        vvbb->SetLineColor(kGreen);
        vvbb->SetFillStyle(3005);
        vvbb->SetFillColor(kGreen);
        vvbb->SetStats(kFALSE);
        vvbb->Draw("hist same");

        subtracted->SetTitle(titles[i]);
        subtracted->GetXaxis()->SetTitle(xtitles[i]);
        subtracted->GetYaxis()->SetTitle(ytitles[i]);
        subtracted->SetMinimum(0);
        subtracted->SetMaximum(largest);
        subtracted->SetLineColor(kMagenta);
        subtracted->SetFillStyle(3006);
        subtracted->SetFillColor(kMagenta);
        subtracted->Draw("hist same");


        TLegend* leg = new TLegend(0.13,0.79,0.25,0.89);
        leg->AddEntry(total,"Total","l");
        leg->AddEntry(zh,"ZH","f");
        leg->AddEntry(vvbb,"vvbb Background", "f");
        leg->AddEntry(subtracted,"WW Fusion and Interference", "f");
        leg->Draw();

        TPaveText* pt = new TPaveText(0.833,0.8,0.883,0.88, "NDC"); 
        pt->SetFillColor(0); // text is black on white
        pt->SetTextSize(0.03); 
        pt->SetTextAlign(22);
        pt->SetTextFont(42);
        pt->AddText(sfraction + " yield");
        pt->SetShadowColor(0);
        pt->Draw();


        TFile *g = new TFile(filename, "update"); // Specifies the root files which contain the generated histograms
        subtracted->Write();
        data_obs->Write();
        g->Close();

    }

    // Saves as PDF
    can->Print("",".pdf");
}

