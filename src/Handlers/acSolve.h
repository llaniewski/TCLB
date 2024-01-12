#ifndef ACSOLVE_H
#define ACSOLVE_H

#include "../CommonHandler.h"
#include "Action.h"
#include "GenericAction.h"
#include "vHandler.h"

class acSolve : public GenericAction {
public:
    static std::string xmlname;
    int Init();
};

#endif  // ACSOLVE_H
