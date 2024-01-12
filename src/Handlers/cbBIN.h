#ifndef CBBIN_H
#define CBBIN_H

#include "../CommonHandler.h"
#include "Callback.h"
#include "vHandler.h"

class cbBIN : public Callback {
    std::string nm;

public:
    static std::string xmlname;
    int Init();
    int DoIt();
};

#endif  // CBBIN_H
