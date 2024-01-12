#ifndef IFCONTAINER_H
#define IFCONTAINER_H

#include "../CommonHandler.h"
#include "Action.h"
#include "GenericAction.h"
#include "vHandler.h"

class IfContainer : public GenericAction {
public:
    static std::string xmlname;
    int Init();
    int Finish();
};

#endif  // IFCONTAINER_H
