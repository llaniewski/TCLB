#ifndef REPEATCONTROL_H
#define REPEATCONTROL_H

#include "../CommonHandler.h"

#include "vHandler.h"
#include "Callback.h"
#include "Design.h"

class  RepeatControl  : public  Design  {
	size_t Pars;
	size_t Pars2;
	double * tab2;
	double lower, upper;
	FILE * f;
	Handler * hand;
	bool flip;
	double flip_level;
public:
	static std::string xmlname;
int Init ();
int Finish ();
size_t NumberOfParameters ();
double Flip (double v, double l, size_t j);
int Parameters (int type, double * tab);
};

#endif // REPEATCONTROL_H
