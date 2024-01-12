#ifndef ACPARAMS_H
#define ACPARAMS_H

#include "../CommonHandler.h"
#include "Action.h"
#include "vHandler.h"

class acObjective : public Action {
public:
    static std::string xmlname;
    int Init();
};

#endif  // ACPARAMS_H
