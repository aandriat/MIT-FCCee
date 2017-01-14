//Helper functions to hist_process.C
//Implements mass calculation and selection cuts
//Joseph Curti and Alexander Andriatis
//20 June 2016


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
#include <utility>
#include <TRandom1.h>


#include "modules/Delphes.h"                   //   |
#include "ExRootAnalysis/ExRootTreeReader.h"   //   -  Include needed delphes headers
#include "classes/DelphesClasses.h"            //   |

#endif

using namespace TMath;
using namespace std;

// Defines fixed particle mases
Double_t MASS_Z = 91.1876;
Double_t MASS_H = 125.09;

Double_t lab_frame_angle(Double_t eta1, Double_t phi1, Double_t eta2, Double_t phi2){
        //Takes in (eta1, phi1) and (eta2, phi2)
        //Returns the angle between the two directions in radians

        //converts Eta (pseudorapidity) to Theta (polar angle)
        Double_t theta1 = 2*(TMath::ATan(TMath::Power(TMath::E(), (-1*eta1))));
        Double_t theta2 = 2*(TMath::ATan(TMath::Power(TMath::E(), (-1*eta2))));

        TVector3 unit_vector1(TMath::Sin(theta1)*TMath::Cos(phi1), TMath::Sin(theta1)*TMath::Sin(phi1), TMath::Cos(theta1));
        TVector3 unit_vector2(TMath::Sin(theta2)*TMath::Cos(phi2), TMath::Sin(theta2)*TMath::Sin(phi2), TMath::Cos(theta2));
        Double_t angle = TMath::ACos((unit_vector1.Dot(unit_vector2)));
        return angle;
}

Double_t acoplanarity_angle(TLorentzVector p1, TLorentzVector p2){
    //Takes in to Four Vectors and outputs the acoplanarity angle
    //That is the angle between the beam axis (z axis) and the plane formed by
    //the two particles 3-momenta
    //Returns acoplanarity angle in radians
    TVector3 p1P3(p1.Px(), p1.Py(), p1.Pz()); //Makes 3-dimensional vector of the spatial momentum
    TVector3 p2P3(p2.Px(), p2.Py(), p2.Pz());
    TVector3 normal_vector = p1P3.Cross(p2P3);
    Double_t aco_angle = TMath::ASin(TMath::Abs(normal_vector.Z())/normal_vector.Mag());
    return aco_angle;
}

Int_t isoleptons(TClonesArray *branchElectron, TClonesArray *branchMuon){
    // Checks if event contains an isolated lepton
    Int_t numleptons = 0;
    numleptons += branchElectron->GetEntries();
    numleptons += branchMuon->GetEntries();
    return numleptons;
}



TLorentzVector momentum_adjustment(TLorentzVector particle1P4, TLorentzVector particle2P4, TLorentzVector CME){
    Double_t olddifference = 0, new1P4 = 0, new2P4 = 0, newdifference = 0;
    Bool_t skip = kFALSE;
    TLorentzVector sumparticleP4 = {0,0,0,0};

    olddifference = CME.M();
    skip=kFALSE;
    while (skip==kFALSE){
        new1P4 = particle1P4.Px();
        new2P4 = particle2P4.Px();
        particle1P4.SetPx(new1P4 - 0.1);
        particle2P4.SetPx(new2P4 + 0.1);
        sumparticleP4 = particle1P4+particle2P4;
        newdifference = TMath::Abs(MASS_H - sumparticleP4.M());
        if (newdifference < olddifference){
            olddifference = newdifference;
        }
        else{
            skip=kTRUE;
        }
    }
    olddifference = CME.M();
    skip=kFALSE;
    while (skip==kFALSE){
        new1P4 = particle1P4.Px();
        new2P4 = particle2P4.Px();
        particle1P4.SetPx(new1P4 + 0.1);
        particle2P4.SetPx(new2P4 - 0.1);
        sumparticleP4 = particle1P4+particle2P4;
        newdifference = TMath::Abs(MASS_H - sumparticleP4.M());
        if (newdifference < olddifference){
            olddifference = newdifference;
        }
        else{
            skip=kTRUE;
        }
    }
    olddifference = CME.M();
    skip=kFALSE;
    while (skip==kFALSE){
        new1P4 = particle1P4.Py();
        new2P4 = particle2P4.Py();
        particle1P4.SetPx(new1P4 - 0.1);
        particle2P4.SetPx(new2P4 + 0.1);
        sumparticleP4 = particle1P4+particle2P4;
        newdifference = TMath::Abs(MASS_H - sumparticleP4.M());
        if (newdifference < olddifference){
            olddifference = newdifference;
        }
        else{
            skip=kTRUE;
        }
    }
    olddifference = CME.M();
    skip=kFALSE;
    while (skip==kFALSE){
        new1P4 = particle1P4.Px();
        new2P4 = particle2P4.Px();
        particle1P4.SetPy(new1P4 + 0.1);
        particle2P4.SetPy(new2P4 - 0.1);
        sumparticleP4 = particle1P4+particle2P4;
        newdifference = TMath::Abs(MASS_H - sumparticleP4.M());
        if (newdifference < olddifference){
            olddifference = newdifference;
        }
        else{
            skip=kTRUE;
        }
    }
    olddifference = CME.M();
    skip=kFALSE;
    while (skip==kFALSE){
        new1P4 = particle1P4.Px();
        new2P4 = particle2P4.Px();
        particle1P4.SetPz(new1P4 - 0.1);
        particle2P4.SetPz(new2P4 + 0.1);
        sumparticleP4 = particle1P4+particle2P4;
        newdifference = TMath::Abs(MASS_H - sumparticleP4.M());
        if (newdifference < olddifference){
            olddifference = newdifference;
        }
        else{
            skip=kTRUE;
        }
    }
    olddifference = CME.M();
    skip=kFALSE;
    while (skip==kFALSE){
        new1P4 = particle1P4.Px();
        new2P4 = particle2P4.Px();
        particle1P4.SetPz(new1P4 + 0.1);
        particle2P4.SetPz(new2P4 - 0.1);
        sumparticleP4 = particle1P4+particle2P4;
        newdifference = TMath::Abs(MASS_H - sumparticleP4.M());
        if (newdifference < olddifference){
            olddifference = newdifference;
        }
        else{
            skip=kTRUE;
        }
    }
    return sumparticleP4;
}
        

template<typename T>
std::pair<Int_t, Int_t> select_2_leading(TClonesArray *branch, T (*examplePart), Int_t PT_cut = 1){
    //Loops through the event branch of particle type 'part_type' (typically electrons or muons)
    //PT Cut of 10 Gev on by default
    //Selects the two highest in PT
    //examplePart is not changed or used in any way. Its only purpose is to specify the class (i.e. Electron or Muon)
    //T is a variable that stores the Class of particle being selected
    //Returns a pair object of the indexes of the two selected particle *objects*
    //Note returns indexes in event not pointers/object itself

    Int_t index1 = -1;
    Int_t index2 = -1;
    T *part = 0, *partSel1 = 0, *partSel2 = 0; //electron or muon depending on parameters
    for (Int_t iPart = 0; iPart < branch->GetEntries(); iPart++){
        part = (T*) branch->At(iPart); 

        if(PT_cut == 1 && part->PT < 10) continue; // Keeps only those particles with transverse momentum > 20 GeV

        // Select the two with the highest pT.
        if(!partSel1){  
            partSel1 = part;
            index1 = iPart;
        }else if(!partSel2){
            partSel2 = part;
            index2 = iPart;
        }else if(part->PT > partSel1->PT){
            partSel2 = partSel1;
            index2 = index1;
            partSel1 = part;
            index1 = iPart;
        }else if(part->PT > partSel2->PT){
            partSel2 = part;
            index2 = iPart;
        }
    }
    std::pair <Int_t, Int_t> selected;
    selected = std::make_pair (index1, index2);
    return selected;
}

template<typename T>
std::pair<Int_t, Int_t> select_2_higgs(TClonesArray *branch, T (*examplePart), TLorentzVector CME){
    // Selects the pair of particles in an event whos combined four-vector results in a mass closest to the Higgs.
    Int_t index1 = -1;
    Int_t index2 = -1;
    T *part = 0, *part2 = 0, *partSel1 = 0, *partSel2 = 0;
    TLorentzVector part1P4, part2P4, sumP4;
    Double_t sum_mass, mass_diff, min_diff;
    min_diff = CME.M();

    for (Int_t iPart = 0; iPart < branch->GetEntries(); iPart++){
        part = (T*) branch->At(iPart); 
        for (Int_t iPart2 = iPart+1; iPart2 < branch->GetEntries(); iPart2++){
            part2 = (T*) branch->At(iPart2);
            part1P4 = part->P4();
            part2P4 = part2->P4();
            sumP4 = part1P4 + part2P4;
            sum_mass = sumP4.M();
            mass_diff = TMath::Abs(MASS_H - sum_mass); // Selects the jet pair with mass closest to Higgs
            if(mass_diff < min_diff){
                partSel1=part;
                partSel2=part2;
                index1=iPart;
                index2=iPart2;
                min_diff = mass_diff;
            }
        }
    }
    std::pair <Int_t, Int_t> selected;
    selected = std::make_pair (index1, index2);
    return selected;
}

template<typename T>
TString parameter_functions(TClonesArray *branch, T (*examplePart), TString parttype, std::map<TString, TClonesArray*> branches, std::map<TString, Int_t> cut, TLorentzVector CME, TTree *ttree, std::map<TString, Float_t> *parameters){
    //If any selection cut is not met "skip" is returned
    //If the event meets the requirements and the hist was filled successfully
    //  "filled" is returned
    //If no pair of this flavor of particles were selected returns none
    //      so that another flavor may be checked
    //Branches to be read from the collision file


    T *particle = 0, *particleSel1 = 0, *particleSel2 = 0;
    Tower *tower = 0, *eflowphoton = 0, *eflowenutralhadron = 0;
    Track *track = 0, *eflowtrack = 0;
    Int_t FSR = 0;
    TLorentzVector sumallP4, diffallP4, particle1P4, particle2P4, sumparticleP4, diffparticleP4, trackP4, towerP4;
    std::pair <Int_t, Int_t> chosen; 
    chosen = std::make_pair (-1,-1);

    // Defines Tree Branch Variables
    Float_t recoilmass = 0, vism = 0, vise = 0, vispt = 0, vispz = 0, ivise = 0, ivispt = 0, ivispz = 0, divism = 0, divise = 0, divispt = 0, divispz = 0, diie = 0, diipt = 0, diipz = 0, acoplanarity = 0, labangle = 0, btag = 0, numtracks = 0, numjets = 0, costheta1=0, costheta2=0, numleptons=0;
 
    // Leading Particle Selection
    if (parttype == "electron" || parttype == "muon") {chosen = select_2_leading(branch, particle);}

    if (parttype == "jet") {chosen = select_2_higgs(branch, particle, CME);}

    if (chosen.first == -1 || chosen.second == -1) {//-1 if one was not chosen
        return "skip"; //If either particle was not chosen that does not select
    }

    particleSel1 = (T*) branch->At(chosen.first);
    particleSel2 = (T*) branch->At(chosen.second);
    particle1P4 = particleSel1->P4();
    particle2P4 = particleSel2->P4();

    sumparticleP4 = particle1P4 + particle2P4;
    diffparticleP4 = CME-sumparticleP4;

    for (Int_t iObject = 0; iObject < branches["EFlowTrack"]->GetEntries(); iObject++){ // Sum of visible mass - used for cuts
        eflowtrack = (Track*) branches["EFlowTrack"]->At(iObject);
        sumallP4 += eflowtrack->P4();
    }
    for (Int_t iObject = 0; iObject < branches["EFlowPhoton"]->GetEntries(); iObject++){ // Sum of visible mass - used for cuts
        eflowphoton = (Tower*) branches["EFlowPhoton"]->At(iObject);
        sumallP4 += eflowphoton->P4();
    }
    for (Int_t iObject = 0; iObject < branches["EFlowNeutralHadron"]->GetEntries(); iObject++){ // Sum of visible mass - used for cuts
        eflowenutralhadron = (Tower*) branches["EFlowNeutralHadron"]->At(iObject);
        sumallP4 += eflowenutralhadron->P4();
    }

    diffallP4 = CME-sumallP4;

    Jet *jetSel1 = dynamic_cast<Jet*> (particleSel1);
    Jet *jetSel2 = dynamic_cast<Jet*> (particleSel2);

    recoilmass = diffparticleP4.M();
    (*parameters)["recoilmass"] = recoilmass;

    vism = sumallP4.M();
    (*parameters)["vism"] = vism;

    vise = sumallP4.E();
    (*parameters)["vise"] = vise;
    
    vispt = sumallP4.Pt();
    (*parameters)["vispt"] = vispt;

    vispz = TMath::Abs(sumallP4.Pz());
    (*parameters)["vispz"] = vispz;
    
    ivise = diffallP4.E();
    (*parameters)["ivise"] = ivise;

    ivispt = diffallP4.Pt();
    (*parameters)["ivispt"] = ivispt;

    ivispz = TMath::Abs(diffallP4.Pz());
    (*parameters)["ivispz"] = ivispz;

    divism = sumparticleP4.M();
    (*parameters)["divism"] = divism;

    divise = sumparticleP4.E();
    (*parameters)["divise"] = divise;

    divispt = sumparticleP4.Pt();
    (*parameters)["divispt"] = divispt;

    divispz = TMath::Abs(sumparticleP4.Pz());
    (*parameters)["divispz"] = divispz;
    
    diie = diffparticleP4.E();
    (*parameters)["diie"] = diie;

    diipt = diffparticleP4.Pt();
    (*parameters)["diipt"] = diipt;

    diipz = TMath::Abs(diffparticleP4.Pz());
    (*parameters)["diipz"] = diipz;

    acoplanarity = acoplanarity_angle(particle1P4, particle2P4);
    (*parameters)["acoplanarity"] = acoplanarity;

    labangle = lab_frame_angle(particle1P4.Eta(), particle1P4.Phi(), particle2P4.Eta(), particle2P4.Phi());
    (*parameters)["labangle"] = labangle;
    
    if (jetSel1 != NULL && jetSel2 != NULL){

        btag = jetSel1->BTag + jetSel2->BTag;
        (*parameters)["btag"] = btag;
        
        numtracks = jetSel1->NCharged + jetSel2->NCharged;
        (*parameters)["numtracks"] = numtracks;
    }

    numjets = branch->GetEntries();
    (*parameters)["numjets"] = numjets;

    costheta1 = TMath::Abs(particle1P4.CosTheta());
    (*parameters)["costheta1"] = costheta1;
    
    costheta2 = TMath::Abs(particle2P4.CosTheta()); 
    (*parameters)["costheta2"] = costheta2;
    
    numleptons = isoleptons(branches["Electron"], branches["Muon"]);
    (*parameters)["numleptons"] = numleptons;
    
    ttree->Fill();
    return "fill";
}