// main41.cc is a part of the PYTHIA event generator.
// Copyright (C) 2015 Torbjorn Sjostrand.
// PYTHIA is licenced under the GNU GPL version 2, see COPYING for details.
// Please respect the MCnet Guidelines, see GUIDELINES for details.

// Author: Mikhail Kirsanov, Mikhail.Kirsanov@cern.ch, based on main01.cc.
// This program illustrates how HepMC can be interfaced to Pythia8.
// It studies the charged multiplicity distribution at the LHC.
// HepMC events are output to the hepmcout41.dat file.

// WARNING: typically one needs 25 MB/100 events at the LHC.
// Therefore large event samples may be impractical.

// Modified for automated sample generation by Alex Andriatis
// 17 June 2016

#include "Pythia8/Pythia.h"
#include "Pythia8Plugins/HepMC2.h"

using namespace Pythia8;

int main() {

  // Interface for conversion from Pythia8::Event to HepMC event.
  HepMC::Pythia8ToHepMC ToHepMC;

  // Specify file where HepMC events will be stored.
  HepMC::IO_GenEvent ascii_io("DDD/pythia/AAA.dat", std::ios::out);

  // Generator. Process selection. LHC initialization. Histogram.
  Pythia pythia;
  pythia.readString("Beams:frameType = 4");
  pythia.readString("Beams:LHEF = DDD/whizard/AAA.lhe");
  pythia.readString("Beams:eCM = BBB.");
  pythia.readString("25:onMode = off");
  pythia.readString("25:onIfMatch = 5 5");
  //pythia.readString("HiggsH1:phiParity = 0.785398");
  //pythia.readString("HiggsH1:parity = 4");
  //pythia.readString("15:onIfMatch = 16 111 211");
  pythia.readString("Tune:ee 3");
  pythia.readString("TimeShower:pTmaxMatch = 1");
  pythia.readString("TimeShower:pTmaxFudge = 1");
  pythia.readString("TimeShower:MEcorrections = off");
  pythia.readString("TimeShower:globalRecoil = on");
  pythia.readString("TimeShower:limitPTmaxGlobal = on");
  pythia.readString("TimeShower:nMaxGlobalRecoil = 1");
  pythia.readString("TimeShower:globalRecoilMode = 2");
  pythia.readString("TimeShower:nMaxGlobalBranch = 1");
  pythia.readString("SpaceShower:pTmaxMatch = 1");
  pythia.readString("SpaceShower:pTmaxFudge = 1");
  pythia.readString("SpaceShower:MEcorrections = off");
  //pythia.readString("23:onMode = off");
  //pythia.readString("23:onIfAny = 11 13 15");
  pythia.init();						
  
  // Begin event loop. Generate event. Skip if error.
  for (int iEvent = 0; iEvent < CCC; ++iEvent) {
    if (!pythia.next()) continue;

    // Construct new empty HepMC event and fill it.
    // Units will be as chosen for HepMC build; but can be changed
    // by arguments, e.g. GenEvt( HepMC::Units::GEV, HepMC::Units::MM)
    //if(iEvent<500000) continue;

    /*int lep = 0;
    for (int i = 0; i < pythia.event.size(); ++i)
      {
	if(pythia.event[i].statusAbs()==23)
	  {
	    //if(pythia.event[i].idAbs()==11 || pythia.event[i].idAbs()==13 || pythia.event[i].idAbs()==15)
	    if(pythia.event[i].idAbs()==15)
	      lep++;
	  }
      }
      if(!(lep>0)) continue;*/
    
    HepMC::GenEvent* hepmcevt = new HepMC::GenEvent();
    ToHepMC.fill_next_event( pythia, hepmcevt );    
    
    // Write the HepMC event to file. Done with it.
    ascii_io << hepmcevt;
    delete hepmcevt;
    
    // End of event loop. Statistics. Histogram.
  }
  pythia.stat();
  
  // Done.
  return 0;
}
