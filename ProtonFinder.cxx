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

namespace larlitecv {
    void ProtonFinder::InitializeME(){
        pdgCode = 2212;
    }

    void ProtonFinder::RunME(larlitecv::DataCoordinator &dataco, int ientry){
        std::cout << dataco.run() << "\t" << dataco.subrun() << "\t" << dataco.event() << std::endl;
    }

    void ProtonFinder::FinalizeME(){
        
    }
}

