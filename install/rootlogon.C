// Defines paths to delphes libraries
{
        gSystem->Load("SRCPATH/DELPHESPATH/libDelphes.so");
        gROOT->ProcessLine(".include SRCPATH/DELPHESPATH");
        gROOT->ProcessLine(".include SRCPATH/DELPHESPATH/external");

}
