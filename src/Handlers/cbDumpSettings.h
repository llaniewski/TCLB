#ifndef CBDUMPSETTINGS_H
#define CBDUMPSETTINGS_H

#include "../CommonHandler.h"
#include "Callback.h"
#include "vHandler.h"

class cbDumpSettings : public Callback {
    std::string filename;
    int old_iter_type;

public:
    static std::string xmlname;
    int Init();
    int DoIt();
    int Finish();
};

#endif  // CBDUMPSETTINGS_H
