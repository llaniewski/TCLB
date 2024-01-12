#ifndef ACGEOMETRY_H
#define ACGEOMETRY_H

#include "../CommonHandler.h"
#include "Action.h"
#include "vHandler.h"

class acGeometry : public Action {
public:
    static std::string xmlname;
    int Init();
};

#endif  // ACGEOMETRY_H
