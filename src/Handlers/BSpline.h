#ifndef BSPLINE_H
#define BSPLINE_H

#include "../CommonHandler.h"

#include "vHandler.h"
#include "Callback.h"
#include "Design.h"

class  BSpline  : public  Design  {
	size_t Pars;
	size_t Pars2;
	std::vector<double> tab2;
	double lower, upper;
	FILE * f;
	Handler * hand;
	bool per;
	int order;
public:
	static std::string xmlname;
int Init ();
int Finish ();
size_t NumberOfParameters ();
double Pos (size_t j);
int Parameters (int type, double * tab);
};

#endif // BSPLINE_H
