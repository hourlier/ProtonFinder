
#include "ProtonFinder.h"

namespace larlitecv {
    void ProtonFinder::InitializeME(){
        pdgCode = 2212;
    }

    void ProtonFinder::RunME(larlitecv::DataCoordinator dataco){
        std::cout << pdgCode << std::endl;
    }

    void ProtonFinder::FinalizeME(){
        
    }
}

