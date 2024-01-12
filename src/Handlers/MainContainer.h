#ifndef MAINCONTAINER_H
#define MAINCONTAINER_H

#include "../CommonHandler.h"
#include "Action.h"
#include "GenericAction.h"
#include "vHandler.h"

class MainContainer : public GenericAction {
public:
    static std::string xmlname;
    int Init();
    int Finish();
};

#endif  // MAINCONTAINER_H
