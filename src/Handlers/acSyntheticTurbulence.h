#ifndef ACSYNTHETICTURBULENCE_H
#define ACSYNTHETICTURBULENCE_H

#include "../CommonHandler.h"
#include "Action.h"
#include "vHandler.h"

class acSyntheticTurbulence : public Action {
public:
    static std::string xmlname;
    int ReadWaveNumer(std::string name, double* var);
    int Init();
};

#endif  // ACSYNTHETICTURBULENCE_H
