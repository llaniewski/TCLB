#ifndef ACLOADMEMORYDUMP_H
#define ACLOADMEMORYDUMP_H

#include "../CommonHandler.h"
#include "Action.h"
#include "vHandler.h"

class acLoadMemoryDump : public Action {
public:
    static std::string xmlname;
    int Init();
};

#endif  // ACLOADMEMORYDUMP_H
