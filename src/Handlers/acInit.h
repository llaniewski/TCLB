#ifndef ACINIT_H
#define ACINIT_H

#include "../CommonHandler.h"
#include "Action.h"
#include "vHandler.h"

class acInit : public Action {
public:
    static std::string xmlname;
    int Init();
};

#endif  // ACINIT_H
