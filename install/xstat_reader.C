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
#endif

void xstat_reader(){

   TFile *f = new  TFile("higgsCombineTest.MaxLikelihoodFit.mH120.root");

   TTree *t = (TTree*)f->Get("limit");
   Double_t limitErr;
   t->SetBranchAddress("limitErr",&limitErr); 

   ofstream myfile;
   myfile.open ("uncertainty.txt");

   for (Int_t i=0; i< (Int_t) t->GetEntries(); i++){
      t->GetEntry(i);
      if (limitErr == 0) continue;
      cout << limitErr << endl;
      myfile << limitErr << "\n"; //write to file
   }

  myfile.close();
}
