#ifndef FOURIER_H
#define FOURIER_H

#include "../CommonHandler.h"

#include "vHandler.h"
#include "Callback.h"
#include "Design.h"

class  Fourier  : public  Design  {
	size_t Pars;
	size_t Pars2;
	double * tab2;
	double lower, upper;
	FILE * f;
	Handler * hand;
	double pi;
	double base_fun(size_t i, size_t j);
public:
	static std::string xmlname;
int Init ();
int Finish ();
size_t NumberOfParameters ();
int Parameters (int type, double * tab);
};

#endif // FOURIER_H
