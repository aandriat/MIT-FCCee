//Helper functions used in recoil mass macros
//Implements many event selection cuts currently used in my Higgs to Invisible Study
//Joseph Curti
//17 June 2016


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
#include <utility>

#include "modules/Delphes.h"                   //   |
#include "ExRootAnalysis/ExRootTreeReader.h"   //   -  Include needed delphes headers
#include "classes/DelphesClasses.h"            //   |

#endif

using namespace TMath;
using namespace std;

Double_t MASS_Z = 91.1876;
Double_t MASS_H = 125.09;

Double_t distance(Double_t x1, Double_t y1, Double_t x2, Double_t y2){
    //returns sqrt( (x1-x2)^2 + (y1-y2)^2 )
    //Using x2 = y2 = 0 returns magnitude of (x1,y1)
    return TMath::Sqrt(TMath::Power((x1 - x2),2) + TMath::Power((y1 - y2),2));
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
    distance1 = distance(eta1, phi1, ph_eta, ph_phi);
    distance2 = distance(eta2, phi2, ph_eta, ph_phi);
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
string hist_fill(TH1D* massHIST, TH1D* recoilmassHIST, Double_t norm, TClonesArray *branchTrack, TClonesArray *branchTower, TClonesArray *branchJet, TClonesArray *branchPhoton, TClonesArray *branch, T (*examplePart), std::map<TString, Int_t> precut, std::map<TString, Int_t> cut, TLorentzVector CME, TString parttype, Int_t iEntry){
    //If any selection cut is not met "skip" is returned
    //If the event meets the requirements and the hist was filled successfully
    //  "filled" is returned
    //If no pair of this flavor of particles were selected returns none
    //      so that another flavor may be checked
    //Branches to be read from the collision file
    T *particle = 0, *particleSel1 = 0, *particleSel2 = 0, *particle_i = 0;
    Photon *photon = 0, *photonSel = 0;
    Tower *tower = 0;
    Track *track = 0;
    Bool_t skip = kFALSE;
    Int_t FSR = 0;
    Double_t min_diff, diparticlePT, diparticlePL, acop_angle, lab_angle;
    TLorentzVector sumallP4, photonP4, particle1P4, particle2P4, sumparticleP4, diffparticleP4, trackP4, towerP4;
    std::pair <Int_t, Int_t> chosen, photon_results; 

    chosen = std::make_pair (-1,-1);
    min_diff = CME.M();

    // Pre Cuts
    if(precut["numjets"]==1){ 
        // PreCut 1: Removes events with less than 2 jets
        if (branch->GetEntries() < 2) return "skip";
    }

    if(precut["isoparticles"]==1){
        // PreCut 2: Removes events with isolated particles
        for (Int_t iTrack = 0; iTrack < branchTrack->GetEntries(); iTrack++){
            track = (Track*) branchTrack->At(iTrack);
            if (track->PID != 11 or track->PID != -11 or track->PID != 13 or track->PID != -13) continue;
            skip = kTRUE;
            for (Int_t iTower = 0; iTower < branchTower->GetEntries(); iTower++){
                tower = (Tower*) branchTower->At(iTower);
                trackP4 = track->P4();
                towerP4 = tower->P4();
                if (trackP4.Angle(towerP4.Vect()) < 25){ //If particle has a particle within 25 degrees  from itself, it is not isolated 
                    skip = kFALSE;
                }
                if (skip == kFALSE) return "skip";
            }
        }
    }

    if (parttype == "electron" || parttype == "muon") {chosen = select_2_leading(branch, particle);}

    if (parttype == "jet") {chosen = select_2_higgs(branch, particle, CME);}

    if (chosen.first == -1 || chosen.second == -1) {//-1 if one was not chosen
        return "none";
    } //If either particle was not chosen that does not select
    else{
        particleSel1 = (T*) branch->At(chosen.first);
        particleSel2 = (T*) branch->At(chosen.second);
        particle1P4 = particleSel1->P4();
        particle2P4 = particleSel2->P4();
    }

    sumparticleP4 = particleSel1->P4() + particleSel2->P4();
    diffparticleP4 = CME-sumparticleP4;

    for (Int_t iTower = 0; iTower < branchTower->GetEntries(); iTower++){ // Sum of visible mass - used for cuts
        tower = (Tower*) branchTower->At(iTower);
        sumallP4 += tower->P4();
    }

    Jet *jetSel1 = dynamic_cast<Jet*> (particleSel1);
    Jet *jetSel2 = dynamic_cast<Jet*> (particleSel2);


    //cuts

    if (cut["photon"]==1){
        //Gets number of photons with at least 10 GeV PT. First int is number and Second is index which is relevant
        //only if number = 1
        photon_results = num_photons_greaterPTthan(branchPhoton, 10);
        if (photon_results.first > 1) return "skip";
    }

    if (cut["charge"]==1){
        if (particleSel1->Charge != -1*particleSel2->Charge) return "skip"; //particles must be opposite in charge
    }

    if (cut["photon"]==1){
        if (photon_results.first == 1){ //If there is a candidate photon for FSR
            //Checks if FSR. Also saves photonP4
            photonSel = (Photon*) branchPhoton->At(photon_results.second);
            FSR = is_FSR(photonSel->Eta, photonSel->Phi, particleSel1->Eta, particleSel1->Phi,
                         particleSel2->Eta, particleSel2->Phi);
            photonP4 = photonSel->P4();
        }
        //FSR Momentum Correction
        if (FSR == 1) 
            particle1P4 += photonP4;
        else if (FSR == 2) 
            particle2P4 += photonP4;   
    }


    if (cut["zmass"]==1){
        //candidate z boson must be within 4 GeV of actual Z boson mass
        if ((sumparticleP4.M() > (MASS_Z + 4)) || (sumparticleP4.M() < (MASS_Z - 4)))
            return "skip";
    }

    if (cut["labangle"]==1){
        //Lab Frame Angle cut
        lab_angle = lab_frame_angle(particleSel1->Eta, particleSel1->Phi, particleSel2->Eta, particleSel2->Phi);
        if (lab_angle <= (100*TMath::Pi()/180)) return "skip";
    }

    if (cut["acoangle"]==1){
        //Acoplanarity Cut
        acop_angle = acoplanarity_angle(particle1P4, particle2P4);
        if (acop_angle <= (10*TMath::Pi()/180)) return "skip"; //Acoplanarity angle must be greater than 10 degrees
    }

    if (cut["diparticlept"]==1){
        //Tranverse and Longitudinal Momentum Cuts
        //PT^2 = Px^2 + Py^2, PL = Pz
        diparticlePT = distance(sumparticleP4.Px(), sumparticleP4.Py(), 0, 0);
        if (diparticlePT <= 10) return "skip"; //particle pair PT must be > 10 GeV 
    }

    if (cut["diparticlepl"]==1){
        diparticlePL = sumparticleP4.Pz();
        if (diparticlePL >= 50) return "skip"; //particle pair PL must be < 50 GeV
    }

    if(cut["vismass"]==1){
        // Cut 1: Visible mass consistent with mh-20 GeV < mvis < mh+10 GeV
        if (sumparticleP4.M() < 125-20 || sumparticleP4.M() > 125+10) return "skip";
    }
    if(cut["visenergy"]==1){
        // Cut 2: Visible energy required to be 105 < Evis < 160
        if (sumparticleP4.E() > 160 || sumparticleP4.E() < 105) return "skip";
    }
    if(cut["vispt"]==1){
        // Cut 3: Total transverse momentum 20 < sum PT < 80
        if (sumparticleP4.Pt() < 20 || sumparticleP4.Pt() > 80) return "skip";
    }

    if(cut["visz"]==1){
        // Cut 5: Total z momentum must be < 60GeV
        if (sumparticleP4.Pz() > 60) return "skip";
    }
    if(cut["anglecut"] == 1){
        // Cut 6: Azimuthal angle must be small (costheta<0.95)
        if (particle1P4.CosTheta() > 0.95 || particle2P4.CosTheta() > 0.95) return "skip";
    }
    
    if (cut["numtracks"] == 1){
        // Cut 7: Limit on number of charged tracks 10 < Ncharged < 40
        if (jetSel1->NCharged > 40 || jetSel1->NCharged < 10 || jetSel2->NCharged > 40 || jetSel2->NCharged < 10) return "skip";
    }
    if (cut["numevis"] == 1){
        // Cut 8: 100 < Evis < 300
        if (sumallP4.E() > 300 || sumallP4.E() < 100) return "skip";
    }
    if (cut["missingmass"] == 1){
        // Cut 9: missing mass larger than 172
        if (diffparticleP4.M()<172) return "skip";
    }
    if (cut["reconstructedmass"] == 1){
        // Cut 10: reconstructed higgs mass larger than 100, smaller than 143
        if (sumparticleP4.M() < 100 || sumparticleP4.M() > 143) return "skip";
    }

    if (jetSel1 != NULL && jetSel2 != NULL){
        if(cut["btag"]==1){
            // Cut 4: Both jets must be btagged
            if (jetSel1->BTag != 1 || jetSel2->BTag != 1) return "skip";
        }
        if (cut["numtracks"] == 1){
            // Cut 7: Limit on number of charged tracks 10 < Ncharged < 40
            if (jetSel1->NCharged > 40 || jetSel1->NCharged < 10 || jetSel2->NCharged > 40 || jetSel2->NCharged < 10) return "skip";
        }
    }
    
    massHIST->Fill(sumparticleP4.M(), norm); // Fills histogram with mass of di-jet system
    recoilmassHIST->Fill(diffparticleP4.M(), norm); // Fills histogram with missing mass

    return "filled";
}