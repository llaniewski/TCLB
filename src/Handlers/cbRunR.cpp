
#include "../RunR.h"
#include "../RunR.hpp"
#include "cbRunR.h"

#ifdef WITH_R

int RunRHandler::Init() {
	Callback::Init();
	RInside& R = RunR::GetR();

	R["Solver"]          = rWrap(solver,this);

	interactive = false;
	echo = true;

	pugi::xml_attribute attr;
	attr = node.attribute("interactive");
	if (attr) interactive = attr.as_bool();
	attr = node.attribute("echo");
	if (attr) echo = attr.as_bool();

	source = "";
    for (pugi::xml_node par = node.first_child(); par; par = par.next_sibling()) {
		if (par.type() == pugi::node_element) {
			char nd_name[20];
			sprintf(nd_name, "xml_%0zx", par.hash_value());
			R[nd_name]          = rWrap(solver,this,par);
			
			source = source + nd_name + "()\n";
			output("element\n");
		} else if (par.type() == pugi::node_pcdata) {
			output("pcdata\n");
			source += par.value();
		} else if (par.type() == pugi::node_cdata) {
			output("cdata\n");
			source += par.value();
		} else {
			output("Unknown\n");
		}
	}
	return 0;
}


int RunRHandler::DoIt() {
	try {
		if (source != "") {
			solver->print("Running R ...");
			if (echo) {
				output("----- RunR -----\n");
				output("%s\n",source.c_str());
				output("----------------\n");
			}
			RunR::parseEval(source);
		}
		if (!interactive) {
			if (echo) NOTICE("You can run interactive R session with Ctrl+X");
			int c = kbhit();
			if (c == 24) {
				int a = getchar();
				if (a == c) {
					interactive = true;
				}
			}
		}
		if (interactive) {
			RunR::replInit();
			while( RunR::replDo() > 0 ) {}
		}
	} catch (...) {
		return -1;
	}
	return 0;
}


#endif // WITH_R

// Function created only to check to create Handler for specific conditions
vHandler * Ask_For_RunR(const pugi::xml_node& node) {
  std::string name = node.name();
  if (name == "RunR") {
#ifdef WITH_R
    return new RunRHandler;
#else
    ERROR("No R support. configure with --enable-rinside\n");
    exit(-1);  
#endif
  }
  return NULL;
}

// Register this function in the Handler Factory
template class HandlerFactory::Register< Ask_For_RunR >;


