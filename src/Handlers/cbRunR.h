#ifndef RUNR_HAND_H
#define RUNR_HAND_H

#include "../Consts.h"

#include "../CommonHandler.h"

#include "vHandler.h"
#include "Callback.h"

#ifdef WITH_R

class RunRHandler : public  Callback  {
    std::string source;
    bool interactive;
    bool echo;
public:
    int Init ();
    int DoIt ();
};

#endif // WITH_R

#endif // RUNR_HAND_H
