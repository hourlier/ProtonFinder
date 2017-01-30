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
#include "TH1D.h"
#include "TCanvas.h"
#include "TBox.h"


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
    int Nrow, Ncol;
    TH2D *hImage[3];
    TH1D *hADC = new TH1D("hADC","hADC",1000,0,1000);
    TBox *ROIBox;
    char planeName[4] = "UVY";
    TCanvas *cROI = new TCanvas("cROI","cROI",1500,300);
    cROI->Divide(3,1);
    int DisplayLevel = 2;

    // loop over the events
    for(int ientry = firstEntry;ientry<firstEntry+nEntries;ientry++){
        std::cout << ientry << "/" << nEntries << std::endl;
        dataco.goto_entry(ientry,"larcv");

        // access 2D images
        larcv::EventImage2D* event_imgs    = (larcv::EventImage2D*)dataco.get_larcv_data( larcv::kProductImage2D, larcv_image_producer );
        const std::vector<larcv::Image2D>& img_v = event_imgs->Image2DArray();
        larcv::EventROI* event_rois = (larcv::EventROI*)dataco.get_larcv_data( larcv::kProductROI, larcv_roi_producer );
        const std::vector<larcv::ROI>& roi_v = event_rois->ROIArray();

        eventID=Form("%04d_%03d_%05d",dataco.run(),dataco.subrun(),dataco.event());

        for(int iPlane = 0;iPlane < 3;iPlane++){
            Nrow = img_v.at(iPlane).meta().rows();
            Ncol = img_v.at(iPlane).meta().cols();

            hImage[iPlane] = new TH2D(Form("hImage_%c_%s",planeName[iPlane],eventID.c_str()),Form("hImage_%c_%s",planeName[iPlane],eventID.c_str()),Ncol,0,Ncol,Nrow,0,Nrow);
            for(int iRow = 0;iRow<Nrow;iRow++){
                for(int iCol = 0;iCol<Ncol;iCol++){
                    hADC->Fill(img_v.at(iPlane).pixel(iRow,iCol));
                    if(img_v.at(iPlane).pixel(iRow,iCol) < 100 && DisplayLevel == 2)continue;
                    hImage[iPlane]->SetBinContent(iCol,Nrow-iRow,img_v.at(iPlane).pixel(iRow,iCol));

                }
            }
            cROI->cd(iPlane+1);

            for(size_t iROI = 0;iROI < roi_v.size();iROI++){//loop over possible multiple ROIs

                Xmin = img_v.at(iPlane).meta().col( roi_v.at(iROI).BB(iPlane).min_x());
                Xmax = img_v.at(iPlane).meta().col( roi_v.at(iROI).BB(iPlane).max_x());
                Ymin = Nrow-img_v.at(iPlane).meta().row( roi_v.at(iROI).BB(iPlane).min_y() );
                Ymax = Nrow-img_v.at(iPlane).meta().row( roi_v.at(iROI).BB(iPlane).max_y() );

                ROIBox = new TBox(Xmin,Ymin,Xmax,Ymax);

                ROIBox->SetFillStyle(0);
                ROIBox->SetLineWidth(1);
                ROIBox->SetLineColor(iPlane+2);
                if(DisplayLevel != 0){
                    hImage[iPlane]->GetXaxis()->SetRangeUser(Xmin-4,Xmax+4);
                    hImage[iPlane]->GetYaxis()->SetRangeUser(Ymin-4,Ymax+4);
                    if(DisplayLevel == 2)hImage[iPlane]->GetZaxis()->SetRangeUser(0,1000);
                }
                hImage[iPlane]->SetContour(10);
                hImage[iPlane]->Draw("colz");
                ROIBox->Draw();
            }

        }
        cROI->Modified();
        cROI->Update();
        if(DisplayLevel == 0)cROI->SaveAs(Form("plots/FullImage_%s.png",eventID.c_str()));
        if(DisplayLevel == 1)cROI->SaveAs(Form("plots/ROI_%s.png",eventID.c_str()));
        if(DisplayLevel == 2)cROI->SaveAs(Form("plots/Track_%s.png",eventID.c_str()));

    }

    // finalize
    TCanvas *cADC = new TCanvas();
    cADC->SetLogy();
    hADC->Draw();
    cADC->SaveAs("plots/cADC.png");
    dataco.finalize();

    return 0;
}
