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
#include <THStack.h>  
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
#include <TCut.h>
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

void draw_hist(TString input_name, Double_t lumi = 10.0, std::array<int, 5> binning = {0,200,0,30000,200}, TString var = "recoilmass", int analysis_type = 0){
    TFile *f = new TFile(input_name); 
    TString outputName;
    TString SignalString, ZZstring, WWstring;
    TH1D* ZHSignalHist, *ZZ_Hist, *WW_Hist, *ZinvHbb_Hist, *ZHinvSM_Hist;
    std::vector<TH1D*> hists_v;
    std::vector<Int_t> colors_v;
    TLegend* leg = new TLegend(0.75, 0.65, 0.95, 0.90);
    SetLegendStyle(leg);
    THStack *stack = new THStack("stack", "");
    TString xtitle = var;
    assert (sizeof(binning) / sizeof(int) == 5);


    if (analysis_type == 1){ //H inv leptonic channel
        outputName = "hists/H_inv_Lep_1/plots/" + var + "Plot";
        if (var == "recoilmass") { xtitle = "Missing Mass [GeV]"; }


        SignalString = "eeZHinv_" + var + "HIST";
        WWstring = "eeWW_" + var + "HIST";
        ZZstring = "eeZZ_" + var + "HIST";

        ZHSignalHist = (TH1D*) f->Get(SignalString);
        WW_Hist = (TH1D*)f->Get(WWstring);
        ZZ_Hist = (TH1D*)f->Get(ZZstring);
        ZHinvSM_Hist = (TH1D*) f->Get("ZHinvSM_Hist");

        leg->AddEntry(ZHSignalHist,"ZH Signal","l");
        leg->AddEntry(ZHinvSM_Hist,"ZH Hinv SM","f");
        leg->AddEntry(ZZ_Hist,"ZZ","f");
        leg->AddEntry(WW_Hist,"WW","f");

        //Add each histogram to stack from bottom to top
        stack->Add(WW_Hist, "WW_Hist");
        stack->Add(ZZ_Hist, "ZZ_Hist");
        stack->Add(ZHinvSM_Hist, "ZHinvSM_Hist");
        stack->Add(ZHSignalHist, "ZHSignalHist");

        Double_t ZHyield = ZHSignalHist->Integral();
        Double_t ZZyield = ZZ_Hist->Integral();
        Double_t WWyield = WW_Hist->Integral();
        Double_t ZHSMyield = ZHinvSM_Hist->Integral();

        cout << endl << "ZH Signal Yield " << ZHyield << endl;
        cout << "ZZ Background Yield " << ZZyield << endl;
        cout << "ZH SM Background Yield " << ZHSMyield << endl;
        cout << "WW Background Yield " << WWyield << endl << endl;
        // cout << endl << "ZH Signal Yield     ZZ Background Yield     WW Background Yield    Combined Yield" << endl;
        // cout <<  ZHyield << " " << ZZyield << " "<< WWyield << " " << CombinedYield << endl << endl;

        //put Signal first!!
        hists_v.push_back(ZHSignalHist);        colors_v.push_back(kRed);
        hists_v.push_back(WW_Hist);             colors_v.push_back(kOrange - 7);
        hists_v.push_back(ZZ_Hist);             colors_v.push_back(kOrange - 3);
        hists_v.push_back(ZHinvSM_Hist);        colors_v.push_back(kYellow);
    }
    else if (analysis_type == 2){ //Zh xsec Z to leptons channel
        outputName = "hists/ZHxsec_Lep_2/plots/" + var + "Plot";
        if (var == "recoilmass") { xtitle = "Recoil Mass [GeV]"; }

        SignalString = "eeZH_SM_" + var + "HIST";
        WWstring = "eeWW_" + var + "HIST";
        ZZstring = "eeZZ_" + var + "HIST";

        ZHSignalHist = (TH1D*)f->Get(SignalString);
        WW_Hist = (TH1D*)f->Get(WWstring);
        ZZ_Hist = (TH1D*)f->Get(ZZstring);

        leg->AddEntry(ZHSignalHist,"ZH Signal","l");
        leg->AddEntry(ZZ_Hist,"ZZ","f");
        leg->AddEntry(WW_Hist,"WW","f");

        stack->Add(WW_Hist, "WW_Hist");
        stack->Add(ZZ_Hist, "ZZ_Hist");
        stack->Add(ZHSignalHist, "ZHSignalHist");

        Double_t ZHyield = ZHSignalHist->Integral();
        Double_t ZZyield = ZZ_Hist->Integral();
        Double_t WWyield = WW_Hist->Integral();

        cout << endl << "ZH Signal Yield " << ZHyield << endl;
        cout << "ZZ Background Yield " << ZZyield << endl;
        cout << "WW Background Yield " << WWyield << endl << endl;

        //put Signal first!!
        hists_v.push_back(ZHSignalHist);        colors_v.push_back(kRed);
        hists_v.push_back(WW_Hist);             colors_v.push_back(kOrange - 7);
        hists_v.push_back(ZZ_Hist);             colors_v.push_back(kOrange - 3);
    }
    else if (analysis_type == 3){ //H inv hadronic channel
        outputName = "hists/H_inv_Hadr_3/plots/CutFlow/" + var + "Plot";
        if (var == "recoilmass") { xtitle = "Missing Mass [GeV]"; }

        SignalString = "eeZH_ZjetsHinv_" + var + "HIST";
        WWstring = "WW2jet_Large_" + var + "HIST";
        ZZstring = "eeZZ_ZinvZjets_" + var + "HIST";

        ZHSignalHist = (TH1D*)f->Get(SignalString);
        WW_Hist = (TH1D*)f->Get(WWstring);
        ZZ_Hist = (TH1D*)f->Get(ZZstring);
        ZHinvSM_Hist = (TH1D*) f->Get("ZHinvSM_Hist");
        // ZinvHbb_Hist = (TH1D*)f->Get("ZinvHbb_recoilmassHIST");

        leg->AddEntry(ZHSignalHist,"ZH Signal","l");
        // leg->AddEntry(ZHinvSM_Hist,"ZH Hinv SM","f");
        leg->AddEntry(ZZ_Hist,"ZZ","f");
        leg->AddEntry(WW_Hist,"WW","f");
        // leg->AddEntry(ZinvHbb_Hist,"ZinvHbb","f");

        stack->Add(WW_Hist, "WW_Hist");
        // stack->Add(ZinvHbb_Hist, "ZinvHbb_Hist");
        stack->Add(ZZ_Hist, "ZZ_Hist");
        // stack->Add(ZHinvSM_Hist, "ZHinvSM_Hist");
        stack->Add(ZHSignalHist, "ZHSignalHist");

        Double_t ZHyield = ZHSignalHist->Integral();
        Double_t ZZyield = ZZ_Hist->Integral();
        Double_t WWyield = WW_Hist->Integral();
        // Double_t ZHSMyield = ZHinvSM_Hist->Integral();
        // Double_t ZinvHbbyield = ZinvHbb_Hist->Integral();

        cout << endl << "ZH Signal Yield " << ZHyield << endl;
        cout << "ZZ Background Yield " << ZZyield << endl;
        // cout << "ZH SM Background Yield " << ZHSMyield << endl;
        cout << "WW Background Yield " << WWyield << endl << endl;
        // cout << "ZH_ZtoInv Background Yield " << ZinvHbbyield << endl << endl;
        cout << "ZH Signal Yield     ZZ Background Yield     WW Background Yield  "<< endl;
        cout <<  ZHyield << " " << ZZyield << " "<< WWyield  << endl;



        //put Signal first!!
        hists_v.push_back(ZHSignalHist);        colors_v.push_back(kRed);
        hists_v.push_back(WW_Hist);             colors_v.push_back(kOrange - 7);
        hists_v.push_back(ZZ_Hist);             colors_v.push_back(kOrange - 3);
        // hists_v.push_back(ZinvHbb_Hist);        colors_v.push_back(kBlue - 3);

        // hists_v.push_back(ZHinvSM_Hist);        colors_v.push_back(kYellow);
    }


    // // TString SignalString = "ZHbbinv2jet_" + var + "HIST";
    // // TString WWstring = "WW2jet_" + var + "HIST";

    // //TString SignalString = "eeZH_ZjetsHinv_" + var + "HIST";
    // //TString WWstring = "WW2jet_Large_" + var + "HIST";
    // //TString ZZstring = "eeZZ_ZinvZjets_" + var + "HIST";

    // // TH1D* ZHSignalHist = (TH1D*)f->Get("eeZHbbinvExcILD_recoilmassHIST");
    // // TH1D* ZZ_Hist = (TH1D*)f->Get("eeZZbbinvExcILD_recoilmassHIST");
    // // TH1D* ZinvHbb_Hist = (TH1D*)f->Get("ZinvHbb_recoilmassHIST");

    // // TH1D* WW_Hist = (TH1D*)f->Get("WW2jet_Large_recoilmassHIST");

    Int_t rebin = 1;
    for (unsigned i=0; i < hists_v.size(); i++) {
        hists_v[i]->Rebin(rebin);
        // hists_v[i]->Scale(lumi/10.0);

        if (var != "recoilmass") { 
        cout << "Normalizing to 1!!!!!" << endl;
        hists_v[i]->Scale( 1.0/ (hists_v[i]->Integral()) );
        }
    }

    //Titling Purposes
    Int_t numBins = hists_v[0]->GetSize() - 2;
    Double_t xmin = hists_v[0]->GetXaxis()->GetXmin();
    Double_t xmax = hists_v[0]->GetXaxis()->GetXmax();
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

    for (unsigned i=0; i < hists_v.size(); i++) {
        if (i==0)
        {
            InitSignal(hists_v[0]);
            hists_v[0]->SetLineColor(kRed);
            if (var != "recoilmass") { 
            hists_v[0]->Scale( 1.0/ (hists_v[0]->Integral()) );
            }
        }

        else InitHist(hists_v[i], xtitle, ytitle, colors_v[i], 1001);
    }


    //Add each histogram to stack from bottom to top

    // TCanvas *can = new TCanvas("ILD ZH & ZZ Whizard", "ILD ZH & ZZ Whizard", 800, 600);
    TCanvas *can = MakeCanvas("canv", "histograms", 800, 600);
    // can->Divide(1,1); //Can be used to split canvas into multiple pads
    // can->cd(1);

    TH1D* dummy = new TH1D("", "", numBins, xmin, xmax);//Dummy hist to set frame since THstack acts weird    
    //dummy->SetMaximum(10000); 
       // dummy->SetMaximum(60000); 

    dummy->SetMaximum(binning[3]); 
    if (var != "recoilmass" ) { dummy->SetMaximum(1); }
    dummy->SetMinimum(binning[2]); 
    dummy->GetXaxis()->SetTitle(xtitle);
    dummy->GetYaxis()->SetTitle(ytitle);
    dummy->GetYaxis()->SetTitleOffset(1.35);
    dummy->Draw();
    stack->Draw("same");
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
    pt->AddText("FCC-ee, " + str_lumi + " ab^{-1} at #sqrt{s} = 240 GeV");
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
    if (analysis_type == 0) { outputName = "hists/Misc/plots/" + var + "Plot"; }
    can->Modified();
    can->Print(outputName + ".pdf"); //Automatically saves plot to a .pdf file with the same name as the histogram


    // TCanvas *summary = MakeCanvas("Summary", "summary", 800, 600);
    // can->Divide(1,1); //Can be used to split canvas into multiple pads
    // can->cd(1);




    //makes datacards
    if (analysis_type == 3)
    {
        TString cardname = "hists/H_inv_Hadr_3/cards/H_inv_Hadr_3_" + var + "_hists.txt";
        TString fullpath = "/afs/cern.ch/work/j/jcurti/public/H_inv/macros/" + input_name; 
        char outputLimitsShape[200];
        sprintf(outputLimitsShape, cardname.Data() );
        ofstream newcardShape;
        newcardShape.open(outputLimitsShape);
        newcardShape << Form("imax 1 number of channels\n");
        newcardShape << Form("jmax * number of background\n");
        newcardShape << Form("kmax * number of nuisance parameters\n");
        newcardShape << Form("shapes * * %s $PROCESS $SYSTEMATIC\n", fullpath.Data() );
        newcardShape << Form("Observation %d\n", -1);
        // newcardShape << Form("bin 1 1 1 1\n");
        newcardShape << Form("bin 1 1 1 \n");
        // newcardShape << Form("process %s %s %s ZHinvSM_Hist \n", SignalString.Data(), WWstring.Data(), ZZstring.Data());
        newcardShape << Form("process %s %s %s \n", SignalString.Data(), WWstring.Data(), ZZstring.Data());
        // newcardShape << Form("process 0 1 2 3 \n");
        // newcardShape << Form("rate          -1 -1 -1 -1\n") ;
        // newcardShape << Form("lumi       lnN  1.00001  1.00001 1.00001  1.00001\n");
        newcardShape << Form("process 0 1 2 \n");
        newcardShape << Form("rate         -1 -1 -1\n") ;
        newcardShape << Form("lumi       lnN  1.00001  1.00001 1.00001  \n");

        newcardShape.close();
    }


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
// void processtree(TString sample, Double_t weight, TLorentzVector CME, TString filenameHeader, TString directory, TString cut, Int_t nbins, Double_t xmin, Double_t xmax, TString var, int type_analysis=0){
void processtree(TString sample, Double_t weight, TLorentzVector CME, TString filenameHeader, TString directory, TString cut, std::array<int, 5> binning, TString var, int type_analysis=0){
    assert (sizeof(binning) / sizeof(int) == 5);

    //Open Input Ntuple File
    TFile *f = new TFile(directory);
    TTree *ttree = (TTree*)f->Get(sample);

    TString filename = filenameHeader + var + "_hists.root";
    // TString recohistname = "_recoilmassHIST";
    // TString histname = sample + recohistname;
    // TH1D *recoilmassHIST = new TH1D(histname,histname, nbins, xmin, xmax);
    // ttree->Project(histname, "scaledrecoilmass", cut);

    TString recohistname = "_" + var + "HIST";
    // TString recohistname = "_" + var + "HIST";
    TString histname = sample + recohistname;
    // TH1D *recoilmassHIST = new TH1D(histname,histname, nbins, xmin, xmax);
    TH1D *recoilmassHIST = new TH1D(histname,histname, binning[4], binning[0], binning[1]);
    // ttree->Project(histname, var, cut);

    TCut cuts = cut.Data();
    // TCut cuts = (cut1&&"")*Form("%f", 0.6)||(cut1)*Form("%f",);

    ttree->Project(histname, var, cuts);
    // ttree->Project(histname, var, "eventWeight*"+cut);

    //Normalize Hist
    recoilmassHIST->Scale(weight*0.5);


    TFile g(filename, "update");
    recoilmassHIST->Write();


    if ( (type_analysis == 1 && sample =="eeZHinv") || (type_analysis == 3 && sample =="eeZH_ZjetsHinv") )     // if (sample =="eeZHbbinvExcILD")
    {
        TH1D* ZHinvSM_Hist = (TH1D*) recoilmassHIST->Clone();
        ZHinvSM_Hist->SetName("ZHinvSM_Hist");
        ZHinvSM_Hist->Scale(.001048);
        ZHinvSM_Hist->Write();
    }

    TH1D* data_obs = new TH1D("data_obs", "data_obs", binning[4], binning[0], binning[1]); 
    data_obs->Write();

    // draw_hist(filename, 5, "", var, type_analysis); //draws the histogram if you want to see it
    g.Close();
}

// For a simulated collision with multiple physics processes as individual delphes files, calculates relevant histograms into one root file. 
void tree_Hist(){
    int analysisType = 3; //1 for H inv leptonic channel, 2 for ZH xsec Z to lepton channel, 3 for H inv hadronic channel (0 for arbitrary)

    // std::array<TString, 1> variables = {"recoilmass"};
    std::vector<TString> variables; //variables to be plotted
    std::vector<std::array<int, 5> > binnings;

                                                                    //  xmin,  xmax, ymin,     ymax, num bins
    variables.push_back("recoilmass");              binnings.push_back({  120,   140,   0,     600000,     20}); //xmin, xmax, ymin, ymax, num bins  
    variables.push_back("recoilmass");              binnings.push_back({   0,   200,   0,     70000,     200}); //xmin, xmax, ymin, ymax, num bins  
    variables.push_back("recoilmassME");            binnings.push_back({   0,   200,   0,     100000,    200});   
    variables.push_back("recoilmassMP");            binnings.push_back({   0,   200,   0,     100000,    200});  
    variables.push_back("scaledrecoilmass");        binnings.push_back({   0,   200,   0,      15000,    200});       
    variables.push_back("divism");                  binnings.push_back({   0,   200,   0,      30000,    200});  
    variables.push_back("divispt");                 binnings.push_back({   0,   100,   0,      30000,    200});   
    variables.push_back("divispz");                 binnings.push_back({   0,   100,   0,      30000,    200}); 
    variables.push_back("INCdivispt");                 binnings.push_back({   0,   100,   0,      30000,    200});   
    variables.push_back("INCdivispz");                 binnings.push_back({   0,   100,   0,      30000,    200}); 
    // variables.push_back("INCbtag");                 binnings.push_back({   0,     6,   0,    2000000,      6});   //was 100,000
    // variables.push_back("INCtautag");               binnings.push_back({   0,     6,   0,     100000,      6});   
    variables.push_back("numIncJets");              binnings.push_back({   0,    10,   0,     100000,     10});   
    // variables.push_back("numElectrons");            binnings.push_back({   0,    10,   0,     100000,     10});  
    // variables.push_back("numMuons");                binnings.push_back({   0,    10,   0,     100000,     10}); 
    // variables.push_back("numPhotons");              binnings.push_back({   0,    10,   0,     100000,     10});   
    variables.push_back("INCdivism");               binnings.push_back({   0,   200,   0,      30000,    200}); 
    // variables.push_back("jetveto");                 binnings.push_back({   0,     6,   0,     100000,      6});   
    variables.push_back("labangle");                binnings.push_back({   0,     5,   0,      30000,    100});   
    variables.push_back("acoplanarity");            binnings.push_back({   0,     5,   0,      30000,    100}); 
    variables.push_back("INClabangle");             binnings.push_back({   0,     5,   0,      30000,    100});   
    // variables.push_back("INCacoplanarity");         binnings.push_back({   0,     5,   0,      30000,    100}); 





    assert(variables.size() == binnings.size());

    TLorentzVector CME = {0,0,0,240}; // Defines center of mass energy
    // TString filename = "hists/invHists14.root"; // Filename for created histogram collection
    // TString filename = "hists/wwTest.root"; 
    
    TString filenameHeader, directory;
    if (analysisType == 1) { 
        // filename = "hists/testingInvLeptonic.root";  
        filenameHeader = "hists/H_inv_Lep_1/H_inv_Lep_1_";
        directory = "ntuples/zhINVzzwwFSRv2.root";
    } 
    if (analysisType == 2) { 
        // filename = "hists/testingIncXSecLeptonic.root";  
        filenameHeader = "hists/ZHxsec_Lep_2/ZHxsec_Lep_2_";
        directory = "ntuples/zhSMzzwwNtuple.root";
    }
    if (analysisType == 3) { 
        // filename = "hists/testingInvHadronic.root";  
        filenameHeader = "hists/H_inv_Hadr_3/H_inv_Hadr_3_";
        directory = "ntuples/ZHZZWW_alljets.root";
    }

    // TString filename = "hists/testingInvLeptonic.root"; 
    // //TString filename = "hists/testingIncXSecLeptonic.root";
    // //TString filename = "hists/testingInvHadronic.root"; 

    // TString directory = "ntuples/zhINVzzwwFSRv2.root"; //Input Tree: Inv leptonic
    // //TString directory = "ntuples/zhSMzzwwNtuple.root"; //Input Tree: xsec leptonic                                                                     
    // //TString directory = "ntuples/zhINVzzwwNoISR.root"; //No ISR 

    // TString directory = "ntuples/ZHZZWW_alljets.root"; //Input tree Hadronic

    // TString directory = "ntuples/zhwwjet2Test3_newvars.root"; 
    // TString directory = "ntuples/bbHinvILDWW.root"; //Input tree

    // TString directory = "ntuples/zhSMzzwwTree41.root"; //Input tree

    Double_t lum = 5;
    setTDRStyle();
    // Double_t xmin = 80; 
    // Double_t xmax = 160;
    Double_t xmin = 0; 
    Double_t xmax = 200;
    Int_t nbins = 200;
    
    // TString xtitle = "Missing Mass [GeV]";
    // if (analysisType==2) { xtitle = "Recoil Mass [GeV]"; }
    // TString ytitle = "Events / " + str_gev + " GeV";

    //data_obs needed for combine tool
    // TH1D* data_obs = new TH1D("data_obs", "data_obs", nbins, xmin, xmax); 
    // TFile f(filename, "recreate");
    // data_obs->Write();
    // f.Close();

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

    TString diparticlept2 = "(divispt < 55)";
    TString zmass3 = "( (divism <= (91.1876 + 5)) )";
    TString btag = "( btag > 0 )";

    TString zmass4 = "( (divism <= (91.1876 + 20)) && (divism >= (91.1876 - 20)) )";
    TString zmass5 = "( (divism <= (91.1876 + 10)) && (divism >= (91.1876 - 10)) )";
    TString INCzmass1 = "(INCdivism > 80.385 + 1)";
    TString INCzmass2 = "(INCdivism > 80.385 - 10)";
    TString INCtautag = "(INCtautag == 0)";
    TString diparticlept30 = "(divispt > 30)";
    TString diparticlepz55 = "(divispz < 55)";





    TString num_elect = "(numElectrons == 0)";
    TString num_muon = "(numMuons == 0)";
    TString num_photon = "(numPhotons == 0)";
    TString num_incJets = "(numIncJets == 2)";
    TString num_incJets3 = "(numIncJets <= 3)";
    TString num_OincJets = "(numOIncJets == 2)";
    TString INCzmass = "(INCdivism < 91.1876)";
    TString INCpt = "(INCdivispt < 55)";
    TString INCpz = "(INCdivispz < 45)";

    TString INC1or2btag = "(INCbtag > 0)";
    TString INC1btag = "(INCbtag == 1 )";
    TString INC2btag = "(INCbtag == 2 )";
    TString INC0btag = "(INCbtag == 0 )";

    TString WeightedINC1or2btag = "(INCbtag > 0)";
    TString WeightedINC1btag = "2*.6*.4*(INCbtag == 1 )";
    TString WeightedINC2btag = "(INCbtag == 2 ))";
    TString WeightedINC0btag = ".4*.4*( INCbtag == 0 )";

    TString recoilmassCUT = "( (recoilmass < 140 ) && (recoilmass > 120) )";





    //List the cuts here and turn them on and off with 0 or 1
    std::map<Int_t, std::pair<TString, Int_t> > cuts ={ 
    { 1, std::pair<TString, Int_t> (jetveto,          0)},
    { 2, std::pair<TString, Int_t> (num_FSRcand,      0)},

    { 3, std::pair<TString, Int_t> (charge,           0)},
    { 4, std::pair<TString, Int_t> (labangle,         1)}, //*
    { 5, std::pair<TString, Int_t> (acoangle,         0)}, //*
    { 6, std::pair<TString, Int_t> (diparticlept,     1)}, //*
    { 7, std::pair<TString, Int_t> (diparticlepl,     1)}, //*

    { 8, std::pair<TString, Int_t> (zmass,            0)},
    { 9, std::pair<TString, Int_t> (lepton,           0)},

    { 10, std::pair<TString, Int_t> (zmass2,          0)},

    { 11, std::pair<TString, Int_t> (diparticlept2,   1)}, //*
    { 12, std::pair<TString, Int_t> (zmass3,          0)}, //*
    { 13, std::pair<TString, Int_t> (btag,            0)},

    { 14, std::pair<TString, Int_t> (num_elect,       1)},
    { 15, std::pair<TString, Int_t> (num_muon,        1)},
    { 16, std::pair<TString, Int_t> (num_photon,      1)},
    { 17, std::pair<TString, Int_t> (num_incJets3,    0)},
    { 18, std::pair<TString, Int_t> (num_OincJets,    0)},
    { 19, std::pair<TString, Int_t> (INCzmass1,       0)},
    { 20, std::pair<TString, Int_t> (INCpt,           0)},
    { 21, std::pair<TString, Int_t> (INCpz,           0)},

    { 22, std::pair<TString, Int_t> (INC1or2btag,      0)},
    { 23, std::pair<TString, Int_t> (INC1btag,         0)},
    { 24, std::pair<TString, Int_t> (INC2btag,         0)},
    { 25, std::pair<TString, Int_t> (INC0btag,         0)},

    { 26, std::pair<TString, Int_t> (WeightedINC1or2btag,      0)},
    { 27, std::pair<TString, Int_t> (WeightedINC1btag,         0)},
    { 28, std::pair<TString, Int_t> (WeightedINC2btag,         0)},
    { 29, std::pair<TString, Int_t> (WeightedINC0btag,         0)},

    { 30, std::pair<TString, Int_t> (zmass4,          1)}, //*
    { 31, std::pair<TString, Int_t> (zmass5,          1)}, //*
    { 32, std::pair<TString, Int_t> (INCzmass2,       0)},
    { 33, std::pair<TString, Int_t> (INCtautag,        1)},
    { 34, std::pair<TString, Int_t> (diparticlept30,        0)},
    { 35, std::pair<TString, Int_t> (diparticlepz55,        1)},
    { 36, std::pair<TString, Int_t> (recoilmassCUT,        1)},











    //---Don't use
    // { 11, std::pair<TString, Int_t> (photonjetveto,   0)},
    // { 12, std::pair<TString, Int_t> (jetveto2,        0)},
    // { 13, std::pair<TString, Int_t> (PTcut,           0)}
    
    };

    //This combines the above defined cuts that are on (have value 1) into a single string to be passed
    // as a parameter
    //flavor = 0  corresponds to electrons, 1 for muons
    // TString total_cut = ".36*" + get_total_cuts(cuts); // + " && (flavor == 1)"; //This can be added to look at e or mu only
    TString total_cut = get_total_cuts(cuts); // + " && (flavor == 1)"; //This can be added to look at e or mu only

    cout << "Total cuts: " + total_cut << endl;

    //Define your samples name here as they are in your sample directory
    //Note the number is the normalization constant for that sample, not the cross section!
    //weight = xsec*lumi/total_unweighted_events

    std::map<TString, Double_t> samples ={ // Defines which samples to process and their corresponding cross sections {"sample name", weight}
     // { "eeZHinv",  0.0529862}, //3.7 mil events
    // { "eeZH_SM", 0.257656}, //0.8 mil events
    // { "eeZH_SM_AramDelphes", 0.255364} //0.8 mil events

     // { "eeZZ", 0.523514},// 5.1 mil events, *(1300/2648.7)
    // { "eeZZnoISR", 2.96024}, //*(1300/2648.7)

    // { "eeZHinv11", 0.279487}, //1 mil events
    // { "eeZZ11", 1.09722},// 3 mil events
    // { "eeWW11", 9.35053}, //5 mil events
    // { "eeZH_SM11", 0.244865} //0.8 mil events

    // { "eeZHinv21", 0.257702}, //1 mil events
    // { "eeZH_SM21", 0.244707}, //0.8 mil events
    // { "eeZZ21", 1.10995}, // 3 mil events
    // { "eeWW21", 9.85769}  //5 mil events

    // { "eeZH_SM41", 0.249691},
    // { "eeZHinv41", 0.257702}, //1 mil events
    // { "eeZZ41", 1.36096},// 3 mil events
    // { "eeWW41", 9.85769} //5 mil events

    // { "eeZHinv12", 0.244754}, //1 mil events
    // { "eeZH_SM12", 0.122363}, //0.8 mil events
    // { "eeZZ12", 1.28281}, // 3 mil events
    // { "eeWW12", 9.87214}  //5 mil events

    // { "eeZHinv14", 0.169978}, //1 mil events
    // { "eeZH_SM14", 0.126223}, //0.8 mil events
    // { "eeZZ14", 1.55722}, // 3 mil events
    // { "eeWW14", 8.63675}  //5 mil events

    // { "eeZHinv0", 0.122445}, //1 mil events
    // { "eeZH_SM0", 0.130175}, //0.8 mil events
    // { "eeZZ0", 1.55722}, // 3 mil events
    // { "eeWW0", 7.67484}  //5 mil events

    // { "eeWW", 5.38153} //7.1 mil events
    // { "eeWWnoISR", 20.7019}

    // { "eeZHbbinv", 3.51429}, //.1 mil events
    // { "eeZZbbinv", 13.6866} //.1 mil events

    // { "eeZHbbinvExc", 0.733056}, //.5 mil events
    // { "eeZZbbinvExc", 2.64859} //.5 mil events
    
    // { "eeZHbbinvExcILD", 0.781029}, //.4 mil events
    // { "eeZZbbinvExcILD", 1.62655}, //.4 mil events
    // { "ZinvHbb", 0.28249}, //.8 mil events
    // { "WWexc", 139.501}, //1.2 mil events 139.501 .00000001

    // { "jetTest3", 19888.9}, //20k events WW
    // { "ZHbbinv2jet", 3.51953}, //100k events
    // { "WW2jet", 495.547} //336k events

    { "eeZZ_ZinvZjets", 4.486}, //100k events
    { "WW2jet_Large", 139.6}, //336k events ZHZZWW_alljets.root
    { "eeZH_ZjetsHinv", 1.52}, //100k events

    };


    for (unsigned j = 0; j < variables.size() ; ++j)
    {
        cout << "j: " << j << " " << variables[j] << endl;
        // Runs over all selected samples and creates histograms of variables
        // for (Int_t i=0; i<1; i++){
            std::map<TString, Double_t>::iterator process;
            for ( process = samples.begin(); process != samples.end(); process++){
                cout << "Found process " << process->first << endl;
                if (process->second != 0.0){
                    cout << variables[j] << endl;
                    // processtree(process->first, process->second, CME, filenameHeader, directory, total_cut, nbins, xmin, xmax, variables[j], analysisType);
                    processtree(process->first, process->second, CME, filenameHeader, directory, total_cut, binnings[j], variables[j], analysisType);                    
                }
            }
        // }
        draw_hist(filenameHeader + variables[j] + "_hists.root", lum, binnings[j], variables[j], analysisType); //draws the histogram if you want to see it

    }


    //Otherwise histograms can be drawn later from the created histogram collection
}

