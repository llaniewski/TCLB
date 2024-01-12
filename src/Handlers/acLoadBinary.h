#ifndef ACLOADBINARY_H
#define ACLOADBINARY_H

#include "../CommonHandler.h"
#include "Action.h"
#include "vHandler.h"

class acLoadBinary : public Action {
public:
    static std::string xmlname;
    int Init();
};

#endif  // ACLOADBINARY_H
