#ifndef ACREMOTEFORCEINTERFACE_H
#define ACREMOTEFORCEINTERFACE_H

#include "../CommonHandler.h"
#include "Action.h"
#include "vHandler.h"

class acRemoteForceInterface : public Action {
    MPMDIntercomm inter;
    std::string integrator;

public:
    static std::string xmlname;
    int Init();
    int ConnectRemoteForceInterface(std::string);
};

#endif  // ACREMOTEFORCEINTERFACE_H
