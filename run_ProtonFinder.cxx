
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
#include "DataFormat/EventROI.h"
#include "DataFormat/EventPixel2D.h"

#include "ProtonFinder.h"

int main( int nargs, char** argv ) {
    std::cout << "[Proton Finder]" << std::endl;

    // configuration
    larcv::PSet cfg = larcv::CreatePSetFromFile("config.cfg");
    larcv::PSet PFconfig = cfg.get<larcv::PSet>("ProtonFinderConfigurationFile");
    std::string larcv_image_producer = PFconfig.get<std::string>("InputLArCVImages");
    std::string larcv_roi_producer = PFconfig.get<std::string>("InputLArCVROI");

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

    //larlitecv::ProtonFinder pf;
    //pf.InitializeME();

    // loop over the events
    for(int ientry = firstEntry;ientry<firstEntry+nEntries;ientry++){
        std::cout << ientry << "/" << nEntries << std::endl;
        dataco.goto_entry(ientry,"larcv");
        larcv::EventImage2D* event_imgs    = (larcv::EventImage2D*)dataco.get_larcv_data( larcv::kProductImage2D, larcv_image_producer );
        std::cout << dataco.run() << "\t" << dataco.subrun() << "\t" << dataco.event() << std::endl;
        std::cout << "get data: number of images=" << event_imgs->Image2DArray().size() << std::endl;
        const std::vector<larcv::Image2D>& img_v = event_imgs->Image2DArray();
        std::cout << "size of first image: "<< " rows=" << img_v.at(0).meta().rows() << " cols=" << img_v.at(0).meta().cols() << std::endl;

        //trying to access ROIs
        larcv::EventROI* event_rois = (larcv::EventROI*)dataco.get_larcv_data( larcv::kProductROI, larcv_roi_producer );
        std::cout << "get data: number of ROIs=" << event_rois->ROIArray().size() << std::endl;



        //pf.RunME(dataco,ientry);
    }

    // finalize
    //pf.FinalizeME();
    dataco.finalize();

    return 0;
}
