// E+E- Delphes Jet Resolution for ee->bbvv final state
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
#include <map>
#include <TFitResult.h>
#include <TGraphErrors.h>

#include "modules/Delphes.h"                   //   |
#include "ExRootAnalysis/ExRootTreeReader.h"   //   -  Include needed delphes headers
#include "classes/DelphesClasses.h"            //   |

#endif

using namespace TMath;
using namespace std;

TString root = ".root";

Double_t resfit(Double_t *v, Double_t *par){
      Double_t fitval = par[0]/v[0] + par[1]/TMath::Sqrt(v[0]) + par[2];
      return fitval;
}

void process_resolution(ExRootTreeReader *treeReader, TH1D* resHIST, Int_t emin){
    TClonesArray *branchJet = treeReader->UseBranch("Jet");
    TClonesArray *branchGenJet = treeReader->UseBranch("GenJet");

    Int_t numberOfEntries = treeReader->GetEntries();

    for (Int_t iEntry = 0; iEntry < numberOfEntries; iEntry++){
        treeReader->ReadEntry(iEntry);

        Jet  *gen1, *gen2, *reco1, *reco2, *part1, *part2;
        if (branchJet->GetEntries() != 2) continue;
        reco1 = (Jet*) branchJet->At(0);
        reco2 = (Jet*) branchJet->At(1);

        Double_t newdeltar, olddeltar;
        olddeltar = 8;
        for (Int_t i = 0; i < branchGenJet->GetEntries(); i++){
            part1 = (Jet*) branchGenJet->At(i); 
            for (Int_t j = i+1; j < branchGenJet->GetEntries(); j++){
                part2 = (Jet*) branchGenJet->At(j);
                newdeltar = (reco1->P4()).DeltaR(part1->P4()) + (reco2->P4()).DeltaR(part2->P4());
                if(newdeltar < olddeltar){
                    gen1 = part1;
                    gen2 = part2;
                    olddeltar = newdeltar;
                }
            }
        }
        Double_t measure1, measure2;
        measure1 = ((reco1->P4()).E())/((gen1->P4()).E());
        measure2 = ((reco2->P4()).E())/((gen2->P4()).E());
        if (gen1->P4().E() > emin && gen1->P4().E() < emin+10){
            resHIST->Fill(measure1);
        }
        if (gen2->P4().E() > emin && gen2->P4().E() < emin+10){
            resHIST->Fill(measure2);
        }
    }
}

// For a simulated collision with multiple physics processes as individual delphes files, calculates relevant histograms into one root file. 
void jet_resolution(){
    TLorentzVector CME = {0,0,0,350}; // Defines center of mass energy
    TString filename = "ee_350.root"; // Filename for created histogram collection
    TString directory = "SAMPLEPATH"; // Directory of stored delphes samples

    TString samplename = directory + "ee_uuh_zh" + root;
    TChain chain("Delphes");
    chain.Add(samplename); 
    ExRootTreeReader *treeReader = new ExRootTreeReader(&chain); // Reads sample from delphes root fiile

    Int_t rangemin = 15;
    Int_t rangemax = 165;
    Int_t binsize = 10;
    Int_t numpoints = (rangemax-rangemin)/binsize;

    std::map<Int_t, TH1D*> histograms;
    for (Int_t i = rangemin; i < rangemax; i+=binsize){
        TString histname = TString::Format("%i",i);
        histograms.insert ( std::pair<Int_t, TH1D*> (i, new TH1D(histname,histname,180,-1,3)));
    }

    for (Int_t i = rangemin; i < rangemax; i+=binsize){
        if (histograms[i]==NULL) continue;
        process_resolution(treeReader, histograms[i], i);
    }

    Float_t energy[numpoints], resolution[numpoints], reserr[numpoints];
    for (Int_t i = rangemin; i < rangemax; i+=binsize){
        if (histograms[i]==NULL) continue;

        TFitResultPtr r = histograms[i]->Fit("gaus","S 0","",0.9,1.1);
        //r->Print("V");

        TString histname = TString::Format("%i",i);
        TCanvas *can = new TCanvas(histname, histname, 1280, 720);
        can->Divide(1,1);
        can->cd(1);
        //gPad->SetLogy();
        histograms[i]->GetXaxis()->SetTitle("E_{reco}/E_{gen}");
        histograms[i]->GetYaxis()->SetTitle("Counts");
        histograms[i]->SetLineColor(kRed);
        histograms[i]->Draw();

        TF1 *fit1 = (TF1*)histograms[i]->GetFunction("gaus");
        fit1->SetLineColor(kBlue);
        fit1->SetName("fit");
        fit1->Draw("same");

        Double_t std = fit1->GetParameter(2);
        Double_t stderror = fit1->GetParError(2);
        energy[(i-rangemin)/binsize] = i+binsize/2;
        resolution[(i-rangemin)/binsize] = std;
        reserr[(i-rangemin)/binsize] = stderror;


        can->Print("",".pdf");
        delete can;
    }

    TCanvas *can1 = new TCanvas("Jet Resolution","Jet Resolution", 800, 600);
        can1->cd();
        TGraphErrors *jetres_plot = new TGraphErrors(numpoints, energy, resolution, 0, reserr);
            jetres_plot->SetTitle("");
            jetres_plot->GetXaxis()->SetTitle("E_{gen} (GeV)");
            jetres_plot->GetYaxis()->SetTitle("#sigma(E_{reco}/E_{gen})");
            jetres_plot->GetYaxis()->SetTitleOffset(1.2);
            jetres_plot->GetXaxis()->SetNdivisions(7);
            jetres_plot->GetYaxis()->SetNdivisions(7);
            jetres_plot->GetXaxis()->SetLimits(rangemin - binsize, rangemax + binsize);
            jetres_plot->SetMinimum(0);
            jetres_plot->SetMaximum(0.1);
            jetres_plot->SetLineColor(kBlue+1);
            jetres_plot->SetFillStyle(1001);
            jetres_plot->SetFillColor(kCyan-9);
            jetres_plot->SetMarkerColor(4);
            jetres_plot->SetMarkerStyle(21);
            jetres_plot->Draw("AP");

        TF1 *resfit_plot = new TF1("resfit",resfit,rangemin,rangemax,3);
            resfit_plot->SetParameters(1,0.3,0);
            resfit_plot->SetParNames("N","S","C");
            resfit_plot->SetLineColor(kRed);
            // resfit_plot->FixParameter(0,0.0);
            // resfit_plot->FixParameter(1,0.0);
            // resfit_plot->FixParameter(2,0.0);
            jetres_plot->Fit("resfit", "R","same");

        Double_t calcN = resfit_plot->GetParameter(0);
            TString resresultintroN = "N = ";
            TString resultstringN = TString::Format("%.2f",calcN);
            TString resresultN = resresultintroN + resultstringN;
        Double_t calcS = resfit_plot->GetParameter(1);
            TString resresultintroS = "S = ";
            TString resultstringS = TString::Format("%.2f",calcS);
            TString resresultS = resresultintroS + resultstringS;
        Double_t calcC = resfit_plot->GetParameter(2);
            TString resresultintroC = "C = ";
            TString resultstringC = TString::Format("%.2f",calcC);
            TString resresultC = resresultintroC + resultstringC;


        TPaveText* pt = new TPaveText(0.45, 0.6, 0.80, 0.85, "tlbrNDC");
            pt->SetFillStyle(0);
            pt->SetTextSize(0.04);
            pt->SetBorderSize(0);
            pt->SetTextColor(1);
            pt->AddText("#frac{#sigma(E_{reco})}{E_{gen}} = #frac{N}{E} + #frac{S}{#sqrt{E}} + C");
            pt->AddText(resresultN);
            pt->AddText(resresultS);
            pt->AddText(resresultC); 
            pt->Draw("same");

    can1->Print("",".pdf");
    delete can1;        


}
