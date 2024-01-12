#ifndef ACTHRESHOLDNOW_H
#define ACTHRESHOLDNOW_H

#include "../CommonHandler.h"
#include "Action.h"
#include "GenericAction.h"
#include "vHandler.h"

class acThresholdNow : public GenericAction {
    int par;
    double level;

public:
    static std::string xmlname;
    int Init();
};

#endif  // ACTHRESHOLDNOW_H
