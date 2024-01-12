#ifndef ACRUNACTION_H
#define ACRUNACTION_H

#include "../CommonHandler.h"
#include "Action.h"
#include "GenericAction.h"
#include "vHandler.h"

class acRunAction : public GenericAction {
public:
    static std::string xmlname;
    int Init();
};

#endif  // ACRUNACTION_H
