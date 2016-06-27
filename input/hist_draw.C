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


    TString file = "madgraph";
    TString filename, name1, name2;
    TString root = ".root";
    Double_t signal, overall, fraction;
    TString sfraction;
    Double_t largest;

    std::map<Int_t, TString > histnames ={ // Defines which samples to process and their corresponding cross sections {index, <"sample name", cross section>}
        { 1, "zh_test"},
        { 2, "ww_test"}
    };

    std::map<Int_t, TString > histtypes ={ // Defines which samples to process and their corresponding cross sections {index, <"sample name", cross section>}
        { 1, "_massHIST"},
        { 2, "_recoilmassHIST"}
    };

    std::map<Int_t, TString > titles ={ // Defines which samples to process and their corresponding cross sections {index, <"sample name", cross section>}
        { 1, "Madgraph Reconstructed Mass of Di-Jet System @ #sqrt{s} = 350 Gev w/0 cuts"},
        { 2, "Madgraph Missing Mass of Di-Jet System @ #sqrt{s} = 350 Gev w/0 cuts"}
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

        TFile *f = new TFile(filename); // Specifies the root files which contain the generated histograms

        TH1D *zh = (TH1D*)f->Get(name1);        
        TH1D *ww = (TH1D*)f->Get(name2);
        TH1D* data_obs = new TH1D("data_obs", "data_obs", 100, 0, 350); 

        *data_obs = (*zh) + (*ww);
        data_obs->SetName("data_obs");

        // Outputs what fraction of the total process is the signal
        signal = ww->Integral();
        overall = data_obs->Integral();
        fraction = signal/overall;
        sfraction = TString::Format("%.2f",fraction);

        largest = data_obs->GetMaximum();
        largest+=0.1*largest;

        zh->SetTitle(titles[i]);
        zh->GetXaxis()->SetTitle(xtitles[i]);
        zh->GetYaxis()->SetTitle(ytitles[i]);
        zh->SetMinimum(0);
        zh->SetMaximum(largest);
        zh->SetLineColor(kRed);
        zh->SetFillStyle(3004);
        zh->SetFillColor(kRed);
        zh->SetStats(kFALSE);
        zh->Draw("hist same");

        ww->SetTitle(titles[i]);
        ww->GetXaxis()->SetTitle(xtitles[i]);
        ww->GetYaxis()->SetTitle(ytitles[i]);
        ww->SetMinimum(0);
        ww->SetMaximum(largest);
        ww->SetLineColor(kBlue);
        ww->SetFillStyle(3005);
        ww->SetFillColor(kBlue);
        ww->SetStats(kFALSE);
        ww->Draw("hist same");

        TLegend* leg = new TLegend(0.13,0.69,0.25,0.89);
        leg->AddEntry(zh,"ZH","f");
        leg->AddEntry(ww,"WW Fusion","f");

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
        data_obs->Write();
        g->Close();

    }

    // Saves as PDF
    can->Print("",".pdf");
}



//     TString file = "ee_350_cut";
//     TString filename, name1, name2, name3, name4, name5, name6, name7, name8, namesub;
//     TString root = ".root";
//     Double_t signal, overall, fraction;
//     TString sfraction;
//     Double_t largest;

//     std::map<Int_t, TString > histnames ={ // Defines which samples to process and their corresponding cross sections {index, <"sample name", cross section>}
//         { 1, "ee_qq"},
//         { 2, "ee_qqll"},
//         { 3, "ee_qqlv"},
//         { 4, "ee_qqqq"},
//         { 5, "ee_uubb"},
//         { 6, "ee_uuh"},
//         { 7, "ee_uuh_zh"},
//         { 8, "ee_uuqq"},
//         { 9, "ee_uuh_ww"}
//     };

//     std::map<Int_t, TString > histtypes ={ // Defines which samples to process and their corresponding cross sections {index, <"sample name", cross section>}
//         { 1, "_massHIST"},
//         { 2, "_recoilmassHIST"}
//     };

//     std::map<Int_t, TString > titles ={ // Defines which samples to process and their corresponding cross sections {index, <"sample name", cross section>}
//         { 1, "Reconstructed Mass of Di-Jet System @ #sqrt{s} = 350 Gev w/ cuts"},
//         { 2, "Missing Mass of Di-Jet System @ #sqrt{s} = 350 Gev w/ cuts"}
//     };

//     std::map<Int_t, TString > xtitles ={ // Defines which samples to process and their corresponding cross sections {index, <"sample name", cross section>}
//         { 1, "Reconstructed Mass (GeV)"},
//         { 2, "Missing Mass (GeV)"}
//     };

//     std::map<Int_t, TString > ytitles ={ // Defines which samples to process and their corresponding cross sections {index, <"sample name", cross section>}
//         { 1, "Particle Count"},
//         { 2, "Particle Count"}
//     };

    
//     // Plots the signal and background di-jet mass and missing mass
//     TCanvas *can = new TCanvas(file, file, 1280, 720);
//     can->Divide(1,histtypes.size());

//     for (Int_t i=1; i< histtypes.size()+1; i++){

//         can->cd(i);

//         filename = file+histtypes[i]+root;
//         name1 = histnames[1]+histtypes[i];
//         name2 = histnames[2]+histtypes[i];
//         name3 = histnames[3]+histtypes[i];
//         name4 = histnames[4]+histtypes[i];
//         name5 = histnames[5]+histtypes[i];
//         name6 = histnames[6]+histtypes[i];
//         name7 = histnames[7]+histtypes[i];
//         name8 = histnames[8]+histtypes[i];
//         namesub = histnames[histnames.size()]+histtypes[i];

//         TFile *f = new TFile(filename); // Specifies the root files which contain the generated histograms

//         TH1D *qq = (TH1D*)f->Get(name1);        
//         TH1D *qqll = (TH1D*)f->Get(name2);
//         TH1D *qqlv = (TH1D*)f->Get(name3);
//         TH1D *qqqq = (TH1D*)f->Get(name4);
//         TH1D *uubb = (TH1D*)f->Get(name5);
//         TH1D *uuh = (TH1D*)f->Get(name6);
//         TH1D *uuh_zh = (TH1D*)f->Get(name7);
//         TH1D *uuqq = (TH1D*)f->Get(name8);

//         TH1D* uuh_ww = new TH1D("subtracted", "subtracted", 100, 0, 350);
//         TH1D* data_obs = new TH1D("data_obs", "data_obs", 100, 0, 350); 

//         *uuh_ww  = (*uuh) - (*uuh_zh);
//         uuh_ww->SetName(namesub);

//         *data_obs = (*qq) + (*qqll) + (*qqlv) + (*qqqq) + (*uubb) + *(uuh) + *(uuqq);
//         data_obs->SetName("data_obs");

//         // Outputs what fraction of the total process is the signal
//         signal = uuh_ww->Integral();
//         overall = data_obs->Integral();
//         fraction = signal/overall;
//         sfraction = TString::Format("%.2f",fraction);

//         largest = data_obs->GetMaximum();
//         largest+=0.1*largest;

//         data_obs->SetTitle(titles[i]);
//         data_obs->GetXaxis()->SetTitle(xtitles[i]);
//         data_obs->GetYaxis()->SetTitle(ytitles[i]);
//         data_obs->SetMinimum(0);
//         data_obs->SetMaximum(largest);
//         data_obs->SetLineColor(kRed);
//         data_obs->SetLineStyle(2);
//         data_obs->SetStats(kFALSE);
//         data_obs->Draw("hist same");

//         qq->SetTitle(titles[i]);
//         qq->GetXaxis()->SetTitle(xtitles[i]);
//         qq->GetYaxis()->SetTitle(ytitles[i]);
//         qq->SetMinimum(0);
//         qq->SetMaximum(largest);
//         qq->SetLineColor(kGreen);
//         qq->SetStats(kFALSE);
//         qq->Draw("hist same");

//         qqll->SetTitle(titles[i]);
//         qqll->GetXaxis()->SetTitle(xtitles[i]);
//         qqll->GetYaxis()->SetTitle(ytitles[i]);
//         qqll->SetMinimum(0);
//         qqll->SetMaximum(largest);
//         qqll->SetLineColor(kGreen-1);
//         qqll->SetStats(kFALSE);
//         qqll->Draw("hist same");

//         qqlv->SetTitle(titles[i]);
//         qqlv->GetXaxis()->SetTitle(xtitles[i]);
//         qqlv->GetYaxis()->SetTitle(ytitles[i]);
//         qqlv->SetMinimum(0);
//         qqlv->SetMaximum(largest);
//         qqlv->SetLineColor(kGreen+1);
//         qqlv->SetStats(kFALSE);
//         qqlv->Draw("hist same");

//         qqqq->SetTitle(titles[i]);
//         qqqq->GetXaxis()->SetTitle(xtitles[i]);
//         qqqq->GetYaxis()->SetTitle(ytitles[i]);
//         qqqq->SetMinimum(0);
//         qqqq->SetMaximum(largest);
//         qqqq->SetLineColor(kGreen-2);
//         qqqq->SetStats(kFALSE);
//         qqqq->Draw("hist same");

//         uubb->SetTitle(titles[i]);
//         uubb->GetXaxis()->SetTitle(xtitles[i]);
//         uubb->GetYaxis()->SetTitle(ytitles[i]);
//         uubb->SetMinimum(0);
//         uubb->SetMaximum(largest);
//         uubb->SetLineColor(kGreen+2);
//         uubb->SetStats(kFALSE);
//         uubb->Draw("hist same");

//         uuqq->SetTitle(titles[i]);
//         uuqq->GetXaxis()->SetTitle(xtitles[i]);
//         uuqq->GetYaxis()->SetTitle(ytitles[i]);
//         uuqq->SetMinimum(0);
//         uuqq->SetMaximum(largest);
//         uuqq->SetLineColor(kGreen-3);
//         uuqq->SetStats(kFALSE);
//         uuqq->Draw("hist same");

//         uuh_zh->SetTitle(titles[i]);
//         uuh_zh->GetXaxis()->SetTitle(xtitles[i]);
//         uuh_zh->GetYaxis()->SetTitle(ytitles[i]);
//         uuh_zh->SetMinimum(0);
//         uuh_zh->SetMaximum(largest);
//         uuh_zh->SetLineColor(kCyan);
//         uuh_zh->SetStats(kFALSE);
//         uuh_zh->Draw("hist same");

//         uuh_ww->SetTitle(titles[i]);
//         uuh_ww->GetXaxis()->SetTitle(xtitles[i]);
//         uuh_ww->GetYaxis()->SetTitle(ytitles[i]);
//         uuh_ww->SetMinimum(0);
//         uuh_ww->SetMaximum(largest);
//         uuh_ww->SetLineColor(kMagenta);
//         uuh_ww->SetFillStyle(3004);
//         uuh_ww->SetFillColor(kMagenta);
//         uuh_ww->SetStats(kFALSE);
//         uuh_ww->Draw("hist same");

//         TLegend* leg = new TLegend(0.13,0.69,0.25,0.89);
//         leg->AddEntry(data_obs,"data_obs","l");
//         leg->AddEntry(uuh_ww,"WW Fusion and Interference","f");
//         leg->AddEntry(uuh_zh,"ZH","l");
//         leg->AddEntry(uuqq,"uuqq","l");
//         leg->AddEntry(uubb,"uubb","l");
//         leg->AddEntry(qqqq,"qqqq","l");
//         leg->AddEntry(qqlv,"qqlv","l");
//         leg->AddEntry(qqll,"qqll","l");
//         leg->AddEntry(qq,"qq","l");


//         leg->Draw();

//         TPaveText* pt = new TPaveText(0.833,0.8,0.883,0.88, "NDC"); 
//         pt->SetFillColor(0); // text is black on white
//         pt->SetTextSize(0.03); 
//         pt->SetTextAlign(22);
//         pt->SetTextFont(42);
//         pt->AddText(sfraction + " yield");
//         pt->SetShadowColor(0);
//         pt->Draw();

//         TFile *g = new TFile(filename, "update"); // Specifies the root files which contain the generated histograms
//         uuh_ww->Write();
//         data_obs->Write();
//         g->Close();

//     }

//     // Saves as PDF
//     can->Print("",".pdf");
// }

