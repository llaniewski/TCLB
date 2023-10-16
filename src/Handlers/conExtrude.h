#ifndef BSPLINE_H
#define BSPLINE_H

#include "../CommonHandler.h"

#include "vHandler.h"
#include "Callback.h"
#include "Design.h"

class  conExtrude  : public  Design  {
public:
	typedef std::array<std::vector<double>,4> coords_t;
private:
	size_t Pars;
	size_t Pars2;
	int direction;
	coords_t coords;
	std::vector<double> tab2;
	std::vector<double> Par;
	double theta;
	double margin;
	std::vector<size_t> idx;
	Handler * hand;
	bool next(size_t i);
	double Fun(double,double);	
	double FunD(double,double);	
public:
	static std::string xmlname;
int Init ();
int Finish ();
size_t NumberOfParameters ();
double Pos (int j);
int Parameters (int type, double * tab);
};

#endif // BSPLINE_H
