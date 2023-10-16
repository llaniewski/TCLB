#ifndef ACFDTEST_H
#define ACFDTEST_H

#include "../CommonHandler.h"

#include "vHandler.h"
#include "Action.h"
#include "GenericAction.h"
#include "GenericOptimizer.h"

class  acFDTest  : public  GenericOptimizer  {
	std::vector<double> start;
	std::vector<double> dx;
	std::vector<double> x;
	std::vector<double> grad;
	std::vector<double> lower;
	std::vector<double> upper;
	int order;
	size_t par_start, par_num;
	double h_min,h_max;
	int h_n;
	public:
	static std::string xmlname;
int OptimizerInit ();
int OptimizerRun ();
int OptimizerExit ();
};

#endif // ACFDTEST_H
