#include "../HandlerFactory.h"
#include "acParam.h"
std::string acParam::xmlname = "Param";

int acParam::Init () {
	        std::string par, zone, value, gauge;
		bool permissive = false;
	        int zone_number = -1;
		Action::Init();
		pugi::xml_attribute attr;
		attr = node.attribute("name");
		if (attr) par = attr.value();
		attr = node.attribute("zone");
		if (attr) zone = attr.value();
		attr = node.attribute("value");
		if (attr) value = attr.value();
		attr = node.attribute("gauge");
		if (attr) gauge = attr.value();
		attr = context_attribute("permissive");
		if (attr) permissive = attr.as_bool();
		if (zone != "") {
			if (solver->geometry->SettingZones.count(zone) > 0) { 
				zone_number = solver->geometry->SettingZones[zone];
			} else {
				ERROR("Unknown zone %s (found while setting parameter %s)\n", zone.c_str(), par.c_str());
				return -1;
			}
		}
		double val = solver->units.alt(value);
		if (par == "") {
			if (gauge != "") {
				output("Gauge without setting\n");
			} else {
				ERROR("Setting name not specified in Param element\n");
				return -2;
			}
		} else {
			int idx = solver->lattice->model->settings.ByName(par);
			int zoneidx = solver->lattice->model->zonesettings.ByName(par);
			if (idx != LIST_INVALID) {
	                        output("Setting %s to %s (%lf)\n", par.c_str(), value.c_str(), val);
				solver->lattice->SetSetting(idx, val);
			} else if (zoneidx != LIST_INVALID) {
				output("Setting %s in zone %s (%d) to %s (%lf)\n", par.c_str(), zone.c_str(), zone_number, value.c_str(), val);
				solver->lattice->zSet.set(zoneidx, zone_number, val);
			} else {
				if (permissive) {
					if (gauge != "") {
						notice("Unknown setting %s with gauge\n", par.c_str());
					} else {
						WARNING("Unknown setting %s\n", par.c_str());
					}
				} else {
					ERROR("Unknown setting %s\n", par.c_str());
					return -3;
				}
			}
		}
		return 0;
	}


// Register the handler (basing on xmlname) in the Handler Factory
template class HandlerFactory::Register< GenericAsk< acParam > >;
