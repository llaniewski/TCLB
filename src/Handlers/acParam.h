#ifndef ACPARAM_H
#define ACPARAM_H

#include "../CommonHandler.h"
#include "Action.h"
#include "vHandler.h"

class acParam : public Action {
public:
    static std::string xmlname;
    int Init();
};

#endif  // ACPARAM_H
