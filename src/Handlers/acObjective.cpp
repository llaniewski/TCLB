#include "acObjective.h"
std::string acObjective::xmlname = "Objective";

int acObjective::Init () {
	Model * model = solver->lattice->model;
	int zone_number = 0;
	size_t n =  model->globals.size();
	std::vector<double> glob(n);
	std::vector<double> grad(n);
	std::vector<double> inObj(n);
	double obj = 0;
	for (size_t i = 0; i < n; i++) {
		glob[i] = solver->lattice->globals[i];
		inObj[i] = 0;
	}
	MPI_Bcast(glob.data(), (int) n, MPI_DOUBLE, 0, solver->mpi_comm);
	pugi::xml_attribute attr;
	for (const Model::Objective& it : model->objectives) {
		attr = node.attribute(it.name.c_str());
		if (attr) {
			double weight = attr.as_double();
			double ret;
			it.fun(glob.data(), &ret, grad.data());
			obj = obj + weight * ret;
			for (size_t i = 0; i < n; i++) {
				inObj[i] = inObj[i] + weight * grad[i];
			}
		}
	}
	for (const Model::Global& it : model->globals) {
		if (it.inObjId >= 0) solver->lattice->zSet.set(it.inObjId, zone_number, inObj[it.id]);
	}
	const Model::Global& obj_glob = model->globals.by_name("Objective");
	if (obj_glob) solver->lattice->globals[ obj_glob.id ] = obj;
	return 0;
}

// Register the handler (basing on xmlname) in the Handler Factory
template class HandlerFactory::Register< GenericAsk< acObjective > >;
