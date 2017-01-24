
#ifndef __larlitecv_ProtonFinder_h__
#define __larlitecv_ProtonFinder_h__

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

namespace larlitecv {

    class ProtonFinder {
    public:
        ProtonFinder() {};
        virtual ~ProtonFinder() {};
        void InitializeME();
        void RunME(larlitecv::DataCoordinator dataco);
        void FinalizeME();
    protected:
        int pdgCode;
    };
    
}
#endif

