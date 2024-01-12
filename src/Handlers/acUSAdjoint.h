#ifndef ACUSADJOINT_H
#define ACUSADJOINT_H

#include "../CommonHandler.h"
#include "Action.h"
#include "GenericAction.h"
#include "vHandler.h"

class acUSAdjoint : public GenericAction {
    int old_iter_type;

public:
    int Init();
};

#endif  // ACUSADJOINT_H
