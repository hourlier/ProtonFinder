
#include <iostream>
#include <string>

// config/storage: from LArCV
#include "Base/PSet.h"
#include "Base/LArCVBaseUtilFunc.h"
#include "Base/DataCoordinator.h"

// larlite data
#include "Base/DataFormatConstants.h"
#include "DataFormat/opflash.h" // example of data product

// larcv data
#include "DataFormat/EventImage2D.h"
#include "DataFormat/EventPixel2D.h"

#include "ProtonFinder.h"

int main( int nargs, char** argv ) {
    std::cout << "[Proton Finder]" << std::endl;

    // configuration
    larcv::PSet cfg = larcv::CreatePSetFromFile("config.cfg");
    larcv::PSet exconfig = cfg.get<larcv::PSet>("ProtonFinderConfigurationFile");
    std::string larcv_image_producer = exconfig.get<std::string>("InputLArCVImages");

    // configure Data coordinator
    larlitecv::DataCoordinator dataco;

    // add files to the coordinator
    std::string PATH = "/Users/hourlier/Documents/PostDocMIT/Research/MicroBooNE/DeepLearning/DLwork/DataFiles";
    dataco.add_inputfile(Form("%s/larlite_extBNB9131runs_cosmic_trained_only_on_mc_pscore_0.99_1598evts_23aug2016.root",PATH.c_str()), "larlite");
    dataco.add_inputfile(Form("%s/larcv_extBNB9131runs_cosmic_trained_only_on_mc_pscore_0.99_1598evts_23aug2016.root",PATH.c_str()),"larcv");
    dataco.add_inputfile(Form("%s/roi_extBNB9131runs_cosmic_trained_only_on_mc_pscore_0.99_1598evts_23aug2016.root",PATH.c_str()),"larcv");

    dataco.configure( "config.cfg", "StorageManager", "IOManager", "ProtonFinderConfigurationFile" );

    // initialize
    dataco.initialize();

    int nEntries = dataco.get_nentries("larcv");
    int firstEntry = 0;
    if(nargs >= 2)nEntries = atoi(argv[1]);   // if need to shorten
    if(nargs >= 3)firstEntry = atoi(argv[2]); // if not starting from 0
    if(nargs > 3){std::cout << "ERROR, syntax is .run_ProtonFinder <nEntries> <firstEntry>" << std::endl; return 1;}

    larlitecv::ProtonFinder pf;
    pf.InitializeME();

    // loop over the events
    for(int ientry = 0;ientry<nEntries;ientry++){
        std::cout << ientry << "/" << nEntries << std::endl;
        dataco.goto_entry(ientry+firstEntry,"larcv");
        pf.RunME(dataco);
    }

    // finalize
    pf.FinalizeME();
    dataco.finalize();

    return 0;
}
