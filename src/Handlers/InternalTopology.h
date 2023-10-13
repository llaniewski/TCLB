#ifndef INTERNALTOPOLOGY_H
#define INTERNALTOPOLOGY_H

#include "../CommonHandler.h"

#include "vHandler.h"
#include "Callback.h"
#include "Design.h"

class  InternalTopology  : public  Design  {
	size_t Pars;
public:
	static std::string xmlname;
int Init ();
size_t NumberOfParameters ();
int Parameters (int type, double * tab);
};

#endif // INTERNALTOPOLOGY_H
