#ifndef CBFAILCHECK_H
#define CBFAILCHECK_H

#include "../CommonHandler.h"
#include "Callback.h"
#include "vHandler.h"

class cbFailcheck : public Callback {
    lbRegion reg;
    bool currentlyactive;

public:
    static std::string xmlname;
    int Init();
    int DoIt();
    int Finish();
};

#endif  // CBFAILCHECK_H
