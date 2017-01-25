//
// Author: Adrien Hourlier
//
// date of creation..........: 01/24/2017
// date of last modification.: 01/25/2017
//
// This code accesses the protons ROIs from the LArLite and LarCV files in input
// The goal is to isolate the pixels that correspond to the proton tracks and set everything else to 0
//

#include <iostream>
#include <string>
#include <vector>

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

// ROOT includes
#include "TH2D.h"
#include "TCanvas.h"


int main( int nargs, char** argv ) {
    std::cout << "[Proton Finder]" << std::endl;

    // configuration
    larcv::PSet cfg = larcv::CreatePSetFromFile("config.cfg");
    larcv::PSet PFconfig = cfg.get<larcv::PSet>("ProtonFinderConfigurationFile");
    std::string larcv_image_producer = PFconfig.get<std::string>("InputLArCVImages");
    std::string larcv_roi_producer   = PFconfig.get<std::string>("InputLArCVROI");

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
    if(nargs >= 2)nEntries = atoi(argv[1]);   // if need to shorten run time
    if(nargs >= 3)firstEntry = atoi(argv[2]); // if not starting from 0
    if(nargs > 3){std::cout << "ERROR, syntax is ./run_ProtonFinder <nEntries> <firstEntry>" << std::endl; return 1;}

    // define useful variables
    std::string eventID = "";
    int Xmin, Xmax, Ymin,Ymax;
    TH2D *hROI[3];
    char planeName[4] = "UVY";
    TCanvas *cROI = new TCanvas("cROI","cROI",900,300);
    cROI->Divide(3,1);

    // loop over the events
    for(int ientry = firstEntry;ientry<firstEntry+nEntries;ientry++){
        std::cout << ientry << "/" << nEntries << std::endl;
        dataco.goto_entry(ientry,"larcv");

        // access 2D images
        larcv::EventImage2D* event_imgs    = (larcv::EventImage2D*)dataco.get_larcv_data( larcv::kProductImage2D, larcv_image_producer );
        const std::vector<larcv::Image2D>& img_v = event_imgs->Image2DArray();


        // access ROIs
        larcv::EventROI* event_rois = (larcv::EventROI*)dataco.get_larcv_data( larcv::kProductROI, larcv_roi_producer );
        eventID=Form("%04d_%03d_%04d",dataco.run(),dataco.subrun(),dataco.event());
        std::cout << eventID << std::endl;
        const std::vector<larcv::ROI>& roi_v = event_rois->ROIArray();

        for(size_t iROI = 0;iROI < roi_v.size();iROI++){//loop over possible multiple ROIs
            for(int iPlane = 0;iPlane < 3;iPlane++){// loop over 3 planes

                Xmin = roi_v.at(iROI).BB(iPlane).min_x();
                Xmax = roi_v.at(iROI).BB(iPlane).max_x();
                Ymin = roi_v.at(iROI).BB(iPlane).min_y();
                Ymax = roi_v.at(iROI).BB(iPlane).max_y();

                std::cout << "plane " << planeName[iPlane] << " : Xmin = " << Xmin << ",\t Xmax = " << Xmax <<  std::endl;
                std::cout <<                           "        : Ymin = " << Ymin << ",\t Ymax = " << Ymax <<  std::endl;
                std::cout << std::endl;

                hROI[iPlane] = new TH2D(Form("hROI_%c_%s",planeName[iPlane],eventID.c_str()),Form("hROI_%c_%s",planeName[iPlane],eventID.c_str()),(Xmax-Xmin),Xmin,Xmax,(Ymax-Ymin),Ymin,Ymax);

                for(int iX = Xmin;iX<=Xmax;iX++){// loop over wires
                    for(int iY = Ymin;iY <=Ymax;iY++){//loop over times
                        hROI[iPlane]->SetBinContent((iX-Xmin)+1,(iY-Ymin)+1,img_v.at(iPlane).pixel(img_v.at(iPlane).meta().col(iX),img_v.at(iPlane).meta().row(iY)));
                        //hROI[iPlane]->SetBinContent((iX-Xmin)+1,(iY-Ymin)+1,img_v.at(iPlane).pixel(iX/4,iY/8));
                    }// loop over times
                }// loop over wires

                cROI->cd(iPlane+1);
                hROI[iPlane]->Draw("colz");
            }// loop over 3 planes
            cROI->Modified();
            cROI->Update();
            cROI->SaveAs(Form("ROI_%zu_%s.png",iROI,eventID.c_str()));
        }// loop over ROIs
    }

    // finalize
    dataco.finalize();

    return 0;
}
