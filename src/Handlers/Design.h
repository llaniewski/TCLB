#ifndef DESIGN_H
#define DESIGN_H

#include "../CommonHandler.h"
#include "Callback.h"
#include "vHandler.h"

class Design : public Callback {
public:
    int DoIt();
    int Init();
    int Finish();
    int Type();
};

#endif  // DESIGN_H
