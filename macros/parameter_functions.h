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

float DeltaR(float eta1,float phi1,float eta2,float phi2){
    float deltaPhi = TMath::Abs(phi1-phi2);
    float deltaEta = eta1-eta2;
    if(deltaPhi > TMath::Pi()){
        deltaPhi = TMath::TwoPi() - deltaPhi;
    }
    return TMath::Sqrt(deltaEta*deltaEta + deltaPhi*deltaPhi);
}

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
    //Takes in two Four Vectors and outputs the acoplanarity angle
    //That is the angle between the beam axis (z axis) and the plane formed by
    //the two particles 3-momenta
    //Returns acoplanarity angle in radians
    TVector3 p1P3(p1.Px(), p1.Py(), p1.Pz()); //Makes 3-dimensional vector of the spatial momentum
    TVector3 p2P3(p2.Px(), p2.Py(), p2.Pz());
    TVector3 normal_vector = p1P3.Cross(p2P3);
    Double_t aco_angle = TMath::ASin(TMath::Abs(normal_vector.Z())/normal_vector.Mag());
    return aco_angle;
}

std::pair<Int_t, Int_t>  num_photons_greaterPTthan(TClonesArray *branchP, Double_t minimum_PT){
    //Loops over photons and counts number with PT of at least minimum_PT
    //Returns a pair
    //First int is the number of photons with greater PT than minimum_PT
    //Second int is the index of the selected photon if there is only photon above minimum_PT
    //Note stops if num becomes greater than 1 because does not meet cut
    Int_t index = -1;
    Int_t num = 0;
    Photon *photon = 0;
    for (Int_t iPhoton = 0; iPhoton < branchP->GetEntries(); iPhoton++){
        photon = (Photon*) branchP->At(iPhoton);
        if(photon->PT >= minimum_PT) {
            // photonSel = photon;
            index = iPhoton;
            num++;
        }
        if (num > 1) break; //stops loop if more than 1 were found
    } 
    return std::make_pair (num, index); 
}

Int_t is_FSR(Double_t ph_eta, Double_t ph_phi, Double_t eta1, Double_t phi1, Double_t eta2, Double_t phi2){
    //Checks if candidate photon is FSR
    //Returns 0 if no
    //Returns 1 if yes and belongs to particle1
    //Returns 2 if yes and belongs to particle2
    Int_t result = 0;
    Double_t distance1 = 0; 
    Double_t distance2 = 0;
    distance1 = DeltaR(eta1, phi1, ph_eta, ph_phi);
    distance2 = DeltaR(eta2, phi2, ph_eta, ph_phi);
    if (distance1 < .4){
        result = 1;
    }
    else if (distance2 < .4)
    {
        result = 2;
    }
    if (distance1 < .4 && distance2 < .4)
    {
        cout << "Both particles" << endl;
    }
    return result;
}

string jetVeto(TClonesArray *branchElectron, TClonesArray *branchMuon, TClonesArray *branchJet){
    //Jet Veto for Inv study
    Double_t jetDistance = 0;
    Int_t is_lepton = 0; 
    Jet *jet = 0;
    Muon *muon_i = 0;
    Electron *electron_i = 0; 

    for (Int_t iJet = 0; iJet < branchJet->GetEntries(); iJet++){
        jetDistance = 0;
        is_lepton = 0; 
        jet = (Jet*) branchJet->At(iJet);
        // if (jet != 0 && jet->PT > 20) {
        if (jet != 0 && jet->PT > 20) {
            for (Int_t iMuon = 0; iMuon < branchMuon->GetEntries(); iMuon++){
                // cout << "Jet looping is working" << endl;
                jetDistance = 0;
                muon_i = (Muon*) branchMuon->At(iMuon);
                jetDistance = DeltaR(muon_i->Eta, muon_i->Phi, jet->Eta, jet->Phi);
                if (jetDistance < .4) 
                    {
                        is_lepton = 1;
                        // cout << "Jet identified as lepton" << endl;
                        break;
                    }
            }
            if (is_lepton != 1) {
                for (Int_t iElectron = 0; iElectron < branchElectron->GetEntries(); iElectron++){
                    jetDistance = 0;
                    electron_i = (Electron*) branchElectron->At(iElectron);
                    jetDistance = DeltaR(electron_i->Eta, electron_i->Phi, jet->Eta, jet->Phi);
                    if (jetDistance < .4) 
                        {
                            is_lepton = 1;
                            // cout << "Jet identified as lepton" << endl;
                            break;
                        }
                }
            }
            if (is_lepton == 1) {
                continue;
            }
            else if (jet != 0 && jet->PT > 20) {
                // cout << "Jet vetoed" << endl;
                return "skip"; //veto event
            }
        }
    }
    return "ok";
}

Int_t jetVeto2(TClonesArray *branchJet, Double_t eta1, Double_t phi1, Double_t eta2, Double_t phi2){
    //Simplified jet veto. Only for use in my invisible study
    Jet *jet = 0;
    Double_t distance1 = 0;
    Double_t distance2 = 0;
    for (Int_t iJet = 0; iJet < branchJet->GetEntries(); iJet++){
        distance1 = 0;
        distance2 = 0;
        jet = (Jet*) branchJet->At(iJet);
        if (jet != 0) {
            distance1 = DeltaR(eta1, phi1, jet->Eta, jet->Phi);
            distance2 = DeltaR(eta2, phi2, jet->Eta, jet->Phi);
            if ( (distance1 < .4) || (distance2 < .4) ){
                continue;
            }
            else if (jet->PT > 20) {
                return 1;
            }
        }
    }
    return 0;
}

Int_t photon_JetVeto(TClonesArray *branchJet, Double_t eta1, Double_t phi1, Double_t eta2, Double_t phi2){
    //Photon to Jet Veto
    //Doesn't work
    Double_t h_over_e = -9;
    Double_t percentE = -1;
    Jet *jet = 0;
    Double_t distance1 = 0;
    Double_t distance2 = 0;
    for (Int_t iJet = 0; iJet < branchJet->GetEntries(); iJet++){
        h_over_e = -9;
        percentE = -1;
        distance1 = 0;
        distance2 = 0;
        jet = (Jet*) branchJet->At(iJet);
        if (jet != 0) {
            distance1 = DeltaR(eta1, phi1, jet->Eta, jet->Phi);
            distance2 = DeltaR(eta2, phi2, jet->Eta, jet->Phi);
            if ( (distance1 < .4) || (distance2 < .4) ){
                continue;
            }
            h_over_e = jet->EhadOverEem;
            percentE = (1.0/(h_over_e + 1.0));
            if (percentE > .8) {
                return 1;
            }
        }
    }
    return 0;
}

Int_t photon_JetVeto2(TClonesArray *branchElectron, TClonesArray *branchMuon, TClonesArray *branchJet){
    //Testing another method
    Double_t jetDistance = 0;
    Int_t is_lepton = 0; 
    Jet *jet = 0;
    Muon *muon_i = 0;
    Electron *electron_i = 0; 

    Double_t h_over_e = -9;
    Double_t percentE = -1;
    for (Int_t iJet = 0; iJet < branchJet->GetEntries(); iJet++){
        jetDistance = 0;
        is_lepton = 0;
        h_over_e = -9;
        percentE = -1; 
        jet = (Jet*) branchJet->At(iJet);
        if (jet != 0) {
            for (Int_t iMuon = 0; iMuon < branchMuon->GetEntries(); iMuon++){
                // cout << "Jet looping is working" << endl;
                jetDistance = 0;
                muon_i = (Muon*) branchMuon->At(iMuon);
                jetDistance = DeltaR(muon_i->Eta, muon_i->Phi, jet->Eta, jet->Phi);
                if (jetDistance < .4) 
                    {
                        is_lepton = 1;
                        // cout << "Jet identified as lepton" << endl;
                        break;
                    }
            }
            if (is_lepton != 1) {
                for (Int_t iElectron = 0; iElectron < branchElectron->GetEntries(); iElectron++){
                    jetDistance = 0;
                    electron_i = (Electron*) branchElectron->At(iElectron);
                    jetDistance = DeltaR(electron_i->Eta, electron_i->Phi, jet->Eta, jet->Phi);
                    if (jetDistance < .4) 
                        {
                            is_lepton = 1;
                            // cout << "Jet identified as lepton" << endl;
                            break;
                        }
                }
            }
            if (is_lepton == 1) {
                continue;
            }
            else if (jet != 0) {
                h_over_e = jet->EhadOverEem;
                percentE = (1.0/(h_over_e + 1.0));
                if (percentE > .8) {
                    return 1;
                }
            }
        }
    }
    return 0;
}

Int_t FSRcorrection(TClonesArray *branchPhoton, Double_t eta1, Double_t phi1, Double_t eta2, Double_t phi2){
    //This is a test. Doesn't actually do anything
    Double_t distance1 = 0;
    Double_t distance2 = 0;
    Photon *photon = 0;
    for (Int_t iPhoton = 0; iPhoton < branchPhoton->GetEntries(); iPhoton++){
        distance1 = 0;
        distance2 = 0;
        photon = (Photon*) branchPhoton->At(iPhoton);
        if (photon != 0) {
            distance1 = DeltaR(photon->Eta, photon->Phi, eta1, phi1);
            distance2 = DeltaR(photon->Eta, photon->Phi, eta2, phi2);
            if (distance1 < 1) {
                return 1;
            }
            if (distance2 < 1) {
                return 2;
            }
        }
    }
    return 0;
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
string select_2_exactly(TClonesArray *branch, T (*examplePart), Int_t PT_cut = 1){
    //Loops through the event branch of particle type 'part_type' (typically electrons or muons)
    //PT Cut of 10 Gev on by default
    //Selects exactly two leptons
    //examplePart is not changed or used in any way. Its only purpose is to specify the class (i.e. Electron or Muon)
    //T is a variable that stores the Class of particle being selected
    //Returns a string:
    //"bad" if the event doesn't meet requirements so should be skipped
    //"selected" if there are two leptons that meet the pt cut
    //"zero" if this event has zero leptons of this flavor

    Int_t index1 = -1;
    Int_t index2 = -1;
    T *part = 0, *partSel1 = 0, *partSel2 = 0; //electron or muon depending on parameters

    //Bad event if num leptons is not 2 nor 0
    if ((branch->GetEntries() != 2) && (branch->GetEntries() != 0)){
        return "bad"; //Bad event, number of this flavor of leptons not two exactly nor zero
    }
    else if (branch->GetEntries() == 2) { //if number of this flavor of leptons is 2
        partSel1 = (T*) branch->At(0); 
        partSel2 = (T*) branch->At(1); 

        if(PT_cut == 1 && ((partSel1->PT < 10) || (partSel2->PT < 10))) {
            cout << "PT cut vetoed" << endl;
            return "bad"; //doesn't meet pt cut so skip
        }
        return "selected";
    }
    else
        return "zero";
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

Int_t FSR_count = 0;
template<typename T>
TString parameter_functions(TClonesArray *branch, T (*examplePart), TString parttype, std::map<TString, TClonesArray*> branches, std::map<TString, Int_t> cut, TLorentzVector CME, TTree *ttree, std::map<TString, Float_t> *parameters){
    //If any selection cut is not met "skip" is returned
    //If the event meets the requirements and the hist was filled successfully
    //  "filled" is returned
    //If no pair of this flavor of particles were selected returns none
    //      so that another flavor may be checked
    //Branches to be read from the collision file

    T *particle = 0, *particleSel1 = 0, *particleSel2 = 0;
    Photon *photon = 0, *photonSel = 0;
    // Tower *tower = 0, *eflowphoton = 0, *eflowenutralhadron = 0;
    // Track *track = 0, *eflowtrack = 0;
    Int_t FSR = 0;
    TLorentzVector sumallP4, diffallP4, particle1P4, particle2P4, photonP4, sumparticleP4, diffparticleP4, 
                   trackP4, towerP4;
    std::pair <Int_t, Int_t> chosen, photon_results; 
    chosen = std::make_pair (-1,-1);
    string sel_exact;

    // Defines Tree Branch Variables
    Float_t recoilmass = 0, vism = 0, vise = 0, vispt = 0, vispz = 0, ivise = 0, ivispt = 0, ivispz = 0, 
            divism = 0, divise = 0, divispt = 0, divispz = 0, diie = 0, diipt = 0, diipz = 0, acoplanarity = 0,
            labangle = 0, btag = 0, numtracks = 0, numjets = 0, costheta1 = 0, costheta2 = 0, eta1 = 0, 
            eta2 = 0, phi1 = 0, phi2 = 0, jetveto = 0, charge1 = 0, charge2 = 0, PT1 = 0, PT2 = 0, 
            genPT1 = 0, genPT2 = 0, PTresolution1 = 0, PTresolution2 = 0, flavor = -1, FSRv2 = 0,
            photon_jet_veto = 0, jetveto2 = 0, numElectrons = 0, numMuons = 0, numJets = 0, numPhotons = 0;

    // Leading Particle Selection
    if (parttype == "electron" || parttype == "muon") {chosen = select_2_leading(branch, particle);}

    // Select exactly 2 leptons of same flavor
    // if (parttype == "electron" || parttype == "muon") {
    //     sel_exact = select_2_exactly(branch, particle, 1);
    //     if (sel_exact == "bad") { 
    //         // cout << "Could not select 2 leptons" << endl;
    //         return "skip"; 
    //     }
    //     else if (sel_exact == "selected") {
    //         chosen = std::make_pair (0, 1);
    //     }
    // }

    if (parttype == "jet") {chosen = select_2_higgs(branch, particle, CME);}

    if (chosen.first == -1 || chosen.second == -1) {//-1 if one was not chosen
        return "none"; //If either particle was not chosen that does not select
    }

    particleSel1 = (T*) branch->At(chosen.first);
    particleSel2 = (T*) branch->At(chosen.second);
    particle1P4 = particleSel1->P4();
    particle2P4 = particleSel2->P4();

    if (parttype == "electron"){
        flavor = 0;
    }
    else if (parttype == "muon"){
        flavor = 1;
    }

    eta1 = particleSel1->Eta;
    phi1 = particleSel1->Phi;
    eta2 = particleSel2->Eta;
    phi2 = particleSel2->Phi;
    charge1 = particleSel1->Charge;
    charge2 = particleSel2->Charge;
    PT1 = particleSel1->PT;
    PT2 = particleSel2->PT;

    genPT1 = ((GenParticle *) particleSel1->Particle.GetObject())->PT;
    genPT2 = ((GenParticle *) particleSel2->Particle.GetObject())->PT;

    PTresolution1 = TMath::Abs(PT1 - genPT1)/genPT1;
    PTresolution2 = TMath::Abs(PT2 - genPT2)/genPT2;

    numElectrons = branches["Electron"]->GetEntries();
    numMuons = branches["Muon"]->GetEntries();
    numJets = branches["Jet"]->GetEntries();
    numPhotons = branches["Photon"]->GetEntries();

    //Gets number of photons with at least 10 GeV PT. First int is number and Second is index which is relevant
    //only if number = 1
    photon_results = num_photons_greaterPTthan(branches["Photon"], 1);
    if (photon_results.first == 1){ //If there is a candidate photon for FSR
        //Checks if FSR. Also saves photonP4
        photonSel = (Photon*) branches["Photon"]->At(photon_results.second);
        FSR = is_FSR(photonSel->Eta, photonSel->Phi, particleSel1->Eta, particleSel1->Phi,
                     particleSel2->Eta, particleSel2->Phi);
        photonP4 = photonSel->P4();
        //FSR Momentum Correction
        if (FSR == 1) {
            FSR_count++;
            particle1P4 += photonP4;
        }
        else if (FSR == 2) {
            FSR_count++;
            particle2P4 += photonP4;   
        }
    }

    sumparticleP4 = particle1P4 + particle2P4;
    diffparticleP4 = CME-sumparticleP4;

    /*
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
    */

    diffallP4 = CME-sumallP4;

    Jet *jetSel1 = dynamic_cast<Jet*> (particleSel1);
    Jet *jetSel2 = dynamic_cast<Jet*> (particleSel2);




    //=======================================================================================
    // Parameter filling
    //=======================================================================================




    recoilmass = diffparticleP4.M();
    (*parameters)["recoilmass"] = recoilmass;

    if (jetVeto(branches["Electron"], branches["Muon"], branches["Jet"]) == "skip") { jetveto = 1; }
    (*parameters)["jetveto"] = jetveto;

    jetveto2 = jetVeto2(branches["Jet"], eta1, phi1, eta2, phi2);
    (*parameters)["jetveto2"] = jetveto2;

    FSRv2 = FSRcorrection(branches["Photon"], eta1, phi1, eta2, phi2);
    (*parameters)["FSRv2"] = FSRv2;

    photon_jet_veto = photon_JetVeto2(branches["Electron"], branches["Muon"], branches["Jet"]);
    (*parameters)["photonjetveto"] = photon_jet_veto;

    // vism = sumallP4.M();
    // (*parameters)["vism"] = vism;

    // vise = sumallP4.E();
    // (*parameters)["vise"] = vise;

    // vispt = sumallP4.Pt();
    // (*parameters)["vispt"] = vispt;

    // vispz = TMath::Abs(sumallP4.Pz());
    // (*parameters)["vispz"] = vispz;

    // ivise = diffallP4.E();
    // (*parameters)["ivise"] = ivise;

    // ivispt = diffallP4.Pt();
    // (*parameters)["ivispt"] = ivispt;

    // ivispz = TMath::Abs(diffallP4.Pz());
    // (*parameters)["ivispz"] = ivispz;

    divism = sumparticleP4.M();
    (*parameters)["divism"] = divism; //zmass

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

    (*parameters)["eta1"] = eta1; 
    (*parameters)["eta2"] = eta2;
    (*parameters)["phi1"] = phi1;
    (*parameters)["phi2"] = phi2;  
    (*parameters)["charge1"] = charge1;
    (*parameters)["charge2"] = charge2;
    (*parameters)["numElectrons"] = numElectrons;
    (*parameters)["numMuons"] = numMuons;
    (*parameters)["numJets"] = numJets;
    (*parameters)["numPhotons"] = numPhotons;
   

    (*parameters)["PT1"] = PT1;
    (*parameters)["PT2"] = PT2;
    (*parameters)["genPT1"] = genPT1;
    (*parameters)["genPT2"] = genPT2;
    (*parameters)["PTresolution1"] = PTresolution1;
    (*parameters)["PTresolution2"] = PTresolution2;    

    (*parameters)["flavor"] = flavor; 
    (*parameters)["FSR"] = FSR; 
    (*parameters)["num_photons_over10GevPT"] = photon_results.first; 

    acoplanarity = acoplanarity_angle(particle1P4, particle2P4);
    (*parameters)["acoplanarity"] = acoplanarity;
 
    labangle = lab_frame_angle(eta1, phi1, eta2, phi2);
    (*parameters)["labangle"] = labangle;

    // if (jetSel1 != NULL && jetSel2 != NULL){

    //     btag = jetSel1->BTag + jetSel2->BTag;
    //     (*parameters)["btag"] = btag;

    //     numtracks = jetSel1->NCharged + jetSel2->NCharged;
    //     (*parameters)["numtracks"] = numtracks;
    // }

    // numjets = branch->GetEntries();
    // (*parameters)["numjets"] = numjets;

    costheta1 = TMath::Abs(particle1P4.CosTheta());
    (*parameters)["costheta1"] = costheta1;

    costheta2 = TMath::Abs(particle2P4.CosTheta()); 
    (*parameters)["costheta2"] = costheta2;

    ttree->Fill();
    return "fill";
}

