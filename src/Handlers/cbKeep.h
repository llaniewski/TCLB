#ifndef CBKEEP_H
#define CBKEEP_H

#include "../CommonHandler.h"
#include "Callback.h"
#include "vHandler.h"

class cbKeep : public Callback {
    int old_iter_type;
    int my_type;
    int what, whatInObj;
    double thr, force;

public:
    static std::string xmlname;
    int Init();
    int DoIt();
    int Finish();
};

#endif  // CBKEEP_H
