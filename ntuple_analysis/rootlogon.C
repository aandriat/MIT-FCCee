// Defines paths to delphes libraries
{
        gSystem->Load("/afs/cern.ch/user/j/jcurti/CMSSW_7_6_3_patch2/src/delphes/libDelphes.so");
        gROOT->ProcessLine(".include /afs/cern.ch/user/j/jcurti/CMSSW_7_6_3_patch2/src/delphes");
        gROOT->ProcessLine(".include /afs/cern.ch/user/j/jcurti/CMSSW_7_6_3_patch2/src/delphes/external");
	gROOT->Macro("HttStyles.cc++");
}
