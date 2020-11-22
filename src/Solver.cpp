#include "Consts.h"
//#include <cuda.h>
#include "pugixml.hpp"
#include "Global.h"
#include <mpi.h>
#ifdef GRAPHICS
	#include "gpu_anim.h"
#endif
#include "cross.h"
#include "Region.h"
class LatticeContainer;
#include "vtkLattice.h"
#include "Geometry.h"
#include "Connectivity.h"
#include "def.h"
#include "utils.h"
#include "unit.h"

#include <fstream>
#include <iostream>
#include <vector>
#include <iomanip>
#include <assert.h>

#include "Solver.h"

using namespace std;

void MouseMove( Solver * data, int x, int y, int nx, int ny );
int MainLoop( uchar4* outputBitmap, Solver *d, int ticks );
void MainFree( Solver *d);

/// Solver initializer
/**
	This stuff should be probably in the constructor
*/
	void Solver::Init() {
		iter = 0;
		iter_type = ITER_NORM;
		opt_iter = 0;
		mpi_rank = 0;
		mpi_size = 1;
		steps = 1;
		saveN = 0;
		saveI = 0;
		saveFile = NULL;
		info.outpath[0] ='\0';
		LogScales = NULL;
	}

/// Solver destructor. Deletes most of the stuff
	Solver::~Solver() {
		if (lattice) delete lattice;
#ifdef GRAPHICS
		if (bitmap) delete bitmap;
#endif
		if (geometry) delete geometry;
		if (LogScales) delete LogScales;
	}


/// Initializes the Save mechanism
	void Solver::saveInit(int n){
		n++;
		if (saveN != 0) {
			free(saveFile[0]);
			free(saveFile);
		}
		saveN=n;
		n++;
		saveFile = (char**) malloc(sizeof(char*)*n);
		char * fn = (char*) malloc(sizeof(char )*n*STRING_LEN);
		for (int i=0;i<n;i++) {
			saveFile[i] = &(fn[i*STRING_LEN]);
			saveFile[i][0] = '\0';
		}
		outGlobalFile("SAVE", ".txt", saveFile[saveN]);
	}
	
/// Add a unit gauge
/**
	Add an equation for the units like "1m/s=10"
	\param nm Name of the setting/equation (only for keeping score)
	\param val Value of the setting (eg. 1m/s)
	\param gauge How much it should be equal (eg. 10)
*/
	void Solver::setUnit(std::string nm, std::string val, std::string gauge) {
		units.setUnit(nm, units(val)/units(gauge), 1);
	}

/// Calculate the scales
/**
	Runs the Gauge procedure on the Units object connected to the solver
	This causes the calculate all the unit scales
*/
	void Solver::Gauge() {
		units.makeGauge();
		if (mpi_rank == 0) {
			units.printGauge();
		}
	}

	void Solver::setOutput(const char * out) {
		char _confname[STRING_LEN];
		strcpy(_confname,info.conffile);
		stripbare(_confname);
		sprintf(info.outpath, "%s%s", out, _confname);
		if (lattice != NULL) sprintf(lattice->snapFileName, "%s%s_Snap", out, _confname);
		NOTICE("Setting output path to: %s\n", info.outpath);
	}

/// Inits the csv Log file
/*
	Inits the csv Log file with the header
	/param filename Path to the Log file
*/
	int Solver::initLog(const char * filename)
	{ 
		if (mpi.rank == 0) {
                    FILE * f = NULL;
                    debug2("Initializing %s\n",filename);
                    f = fopen(filename, "wt");
                    assert( f != NULL );
			
                    fprintf(f,"\"Iteration\",\"Time_si\",\"Walltime\",\"Optimization\"");
		for (ModelBase::Settings::const_iterator it=lattice->model->settings.begin(); it !=lattice->model->settings.end(); it++) {
			const char * nm = it->name.c_str();
			fprintf(f,",\"%s\",\"%s_si\"", nm, nm);
			LogScales[it->id] = 1/units.alt(it->unit);
		}
		for (ModelBase::ZoneSettings::const_iterator it=lattice->model->zonesettings.begin(); it != lattice->model->zonesettings.end(); it++) {
			const char * nm = it->name.c_str();
		    	for (std::map<std::string,int>::iterator it2 = geometry->SettingZones.begin(); it2 != geometry->SettingZones.end(); it2++) {
	                	fprintf(f,",\"%s-%s\",\"%s-%s_si\"", nm, it2->first.c_str(), nm, it2->first.c_str());
	                }
			LogScales[lattice->model->settings.size() + it->id] = 1/units.alt(it->unit);
		}
		for (ModelBase::Globals::const_iterator it=lattice->model->globals.begin(); it != lattice->model->globals.end(); it++) {
			const char * nm = it->name.c_str();
                        fprintf(f,",\"%s\",\"%s_si\"", nm, nm);
			LogScales[lattice->model->settings.size() + lattice->model->zonesettings.size() + it->id] = 1/units.alt(it->unit);
		}
		for (ModelBase::Scales::const_iterator it=lattice->model->scales.begin(); it != lattice->model->scales.end(); it++) {
			const char * nm = it->name.c_str();
                        fprintf(f,",\"%s_si\"", nm);
                        LogScales[lattice->model->settings.size() + lattice->model->zonesettings.size() + lattice->model->globals.size() + it->id] = 1/units.alt(it->unit);
		}
		fprintf(f,"\n");
		fclose(f);
		}
                return 0;
	}

/// Writes to the csv Log file.
/** Oh, yes it does
	\param filename Path to the Log file
*/
	int Solver::writeLog(const char * filename)
	{ 
	        FILE * f = NULL;
		double v;
		double * glob = lattice->globals;
	        if (mpi.rank == 0) {
			int j=0;
	                f = fopen(filename, "at");  
	                assert( f != NULL );
			fprintf(f,"%d, %.13le, %.13le, %d",iter, 1.0/units.alt("1s") * iter, get_walltime(), opt_iter);
			for (size_t i=0; i< lattice->model->settings.size(); i++) {
				v = lattice->GetSetting(i);
				fprintf(f,", %.13le, %.13le",v,v*LogScales[j]);
				j++;
			}
			for (size_t i=0; i< lattice->model->zonesettings.size(); i++) {
			    	for (std::map<std::string,int>::iterator it = geometry->SettingZones.begin(); it != geometry->SettingZones.end(); it++) {
			    		int ind = lattice->ZoneIter;
			    		int zone = it->second;
			    		v = lattice->zSet.get(i, zone, ind);
//					v = lattice->settings_val[i];
					fprintf(f,", %.13le, %.13le",v,v*LogScales[j]);
		                }
				j++;
			}
			for (size_t i=0; i< lattice->model->globals.size(); i++) {
				v = glob[i];
				fprintf(f,", %.13le, %.13le",v,v*LogScales[j]);
				j++;
			}
			for (size_t i=0; i< lattice->model->scales.size(); i++) {
				fprintf(f,", %.13le",LogScales[j]);
				j++;
			}
			fprintf(f,"\n");
	                fclose(f);  
        	}
		return 0;
	}

	void Solver::print(const char * str) {
		output("%8d it %s\n", iter, str);
	}


///	Writes state of lattive to VTK.
/**
	Writes all Quantities and Geometry features to a VTI file with vtkWriteLattice
	\param nm Appendix added to the name of the vti file written
	\param s Set of fields/quantities/geometry features to write
*/
	int Solver::writeVTK(const char * nm, name_set * s) {
		print("writing vtk");
		char filename[2*STRING_LEN];
		int ret;
		if(latticeType == 0) { // cartesian lattice output function
			outIterFile(nm, ".vti", filename);
			ret = vtkWriteLattice(filename, lattice, units, s);
		} else if(latticeType == 1 && !connectivity->cellDataOutput) {
			outIterFile(nm, ".vtp", filename);
			
			ret = vtkWriteLatticeArbitrary(filename, latticeSize, lattice, units, s);
		} else if(latticeType == 1 && connectivity->cellDataOutput) {
			outIterFile(nm, ".vtu", filename);
			ret = vtkWriteLatticeArbitraryUG(filename, latticeSize, lattice, connectivity, units, s);
		}
		return ret;
	}

///	Writes state of lattive to txt files.
/**
	Writes all Quantities  to a set of txt files
	\param nm Appendix added to the name of the txt file written
	\param s Set of fields/quantities/geometry features to write
	\param type type of file: 0-normal, 1-gzip
*/
	int Solver::writeTXT(const char * nm, name_set * s, int type) {
		print("writing txt");
		char filename[2*STRING_LEN];
		outIterFile(nm, "", filename);
		int ret = txtWriteLattice(filename, lattice, units, s, type);
		return ret;
	}



///	Writes state of lattive to a binary file.
/**
	Writes all the data of the lattice with vtkWriteLattice
	\param nm Appendix added to the name of the bin file written
*/
	int Solver::writeBIN(const char * nm) {
		print("writing bin");
		char filename[2*STRING_LEN];
		outIterFile(nm, "", filename);
		int ret = binWriteLattice(filename, lattice, units);
		return ret;
	}

///	Sets the size of the Lattice
/**
	Sets the size and allocates all the needed buffers etc.
	\param nx X size of the lattice
	\param ny Y size of the lattice
	\param nz Z size of the lattice (1 for 3D)
	\param ns Number of Snapshots allocated
	\param latticeSize total size of the lattice (will be different for arbitraryLattices)
	\param latticeType (0 = Cartesian, 1 = Arbitrary)
*/
	int Solver::setSize(int nx, int ny, int nz, int ns, size_t latticeSize_, int latticeType_) {
		info.region.nx = nx;
		info.region.ny = ny;
		info.region.nz = nz;
		latticeSize = latticeSize_;
		output("Global lattice size: %dx%dx%d\n", info.region.nx, info.region.ny, info.region.nz);
//		if (info.region.nx < info.xsdim) {
//			info.xsdim = info.region.nx - 1 + 32 - ((info.region.nx - 1) % 32);
//			NOTICE("small mesh: resetting number of threads to: %dx%d\n", info.xsdim, info.ysdim);
//		}
		info.region.nx += info.xsdim - 1 - ((info.region.nx - 1) % info.xsdim);
		MPIDivision();
		InitAll(ns, latticeType_, latticeSize_);
		// Setting settings to default
		for (ModelBase::Settings::const_iterator it=lattice->model->settings.begin(); it !=lattice->model->settings.end(); it++) {
			if (! it->isDerived) {
				lattice->SetSetting(it->id, units.alt(it->defaultValue));
			}
		}
		return 0;
	}

///	Decompose the lattice for parallel processing
/**
	Divides the lattice into simmilar-size parts for MPI parallel processing
*/
	int Solver::MPIDivision() {
		if (mpi_rank == 0) {
			int divz,divy;
			float com, mincom, optcom;
			mincom = (1+mpi_size) * (info.region.ny+info.region.nz);
			optcom = 2 * sqrt((float) info.region.ny*info.region.nz*mpi_size);
			for (divz = 1; divz <= mpi_size; divz ++)
				if (mpi_size % divz == 0) {
	                                divy = mpi_size / divz;
	                                com = divz * info.region.ny + divy * info.region.nz;
	                                char buf[8000];
	                                char * str = buf;
	                                str += sprintf(str, "MPI division %d x %d. Communication: %f (%3.0f%%) ", divz, divy, com, 100*(com/optcom - 1));
	                                if (com < mincom) {
	                                        mincom = com;
	                                        int * zlens = new int[divz];
	                                        int * ylens = new int[divy];
	                                        int mz,my;
	                                        mz = info.region.nz;
	                                        my = info.region.ny;
	                                        if (mz >= divz && my >= divy) {
	                                                str += sprintf(str, "Division:");
	                                                for (int i=0; i<divy; i++)
	                                                {	ylens[i] = my/(divy-i);
	                                                        my -= ylens[i];
	                                                        str += sprintf(str, " %d",ylens[i]);
	                                                }
	                                                str += sprintf(str, " x");
	                                                for (int i=0; i<divz; i++)
	                                                {	zlens[i] = mz/(divz-i);
	                                                        mz -= zlens[i];
	                                                        str += sprintf(str, " %d",zlens[i]);
	                                                }
	                                                str += sprintf(str, "\n");
	                                                int dz=0,dy=0,k=0;;
	                                                for (int i=0; i<divz; i++) {
								dy=0;
		                                                for (int j=0; j<divy; j++) {
	                                                                mpi.node[k].region.dz = dz;
	                                                                mpi.node[k].region.dy = dy;
	                                                                mpi.node[k].region.nz = zlens[i];
	                                                                mpi.node[k].region.ny = ylens[j];
	                                                                mpi.node[k].region.dx = info.region.dx;
	                                                                mpi.node[k].region.nx = info.region.nx;
	                                                                dy += ylens[j];
	                                                                k++;
	                                                        }
	                                                        dz += zlens[i];
	                                                }
						        mpi.divx = 1;
						        mpi.divy = divy;
						        mpi.divz = divz;
	                                                fillSides(mpi, 1, divy, divz);
	                                        } else {
	                                                str += sprintf(str, "Mesh too small to divide\n");
	                                        }
	                                        delete[] zlens;
	                                        delete[] ylens;
	                                } else {
						str += sprintf(str, "\n");
					}
					debug2(buf);
	                	}
	                int k = 0;
	                for (int i=0; i < mpi_size; i++) {
	                        debug2("Processor %d will get: %dx%dx%d\n", i, mpi.node[i].region.nx, mpi.node[i].region.ny,mpi.node[i].region.nz);
	                        if (k < mpi.node[i].region.size()) k = mpi.node[i].region.size();
	                }
	                float overhead = ((double)(  k*mpi_size - info.region.size()  )) / info.region.size();
	                notice("Max region size: %d. Mesh size %d. Overhead: %2.f%%\n", k, info.region.size(), overhead * 100);
		}
	
	        MPI_Bcast(mpi.node, mpi_size * sizeof(NodeInfo), MPI_BYTE, 0, MPMD.local);
	        MPI_Bcast(&mpi.divx, 1, MPI_INT, 0, MPMD.local);
	        MPI_Bcast(&mpi.divy, 1, MPI_INT, 0, MPMD.local);
	        MPI_Bcast(&mpi.divz, 1, MPI_INT, 0, MPMD.local);
	        region = mpi.node[mpi_rank].region;
	        mpi.totalregion = info.region;
	        output("Local lattice size: %dx%dx%d\n", region.nx, region.ny,region.nz);
		return 0;
	}


/// Initializes all the internals of the Solver
/**
	Initializes Lattice, settings, etc.
	\param ns Number of Snapshots to allocate
*/
	int Solver::InitAll(int ns, int latticeType_, size_t latticeSize_) {
	        // Making a window
	        #ifdef GRAPHICS
	        	NOTICE("Running graphics at %dx%d\n", region.nx, region.ny);
			bitmap = new GPUAnimBitmap( region.nx, region.ny, this );
			RunMainLoop();
			debug0("Graphics done");
	        #endif
	
		// Creating Lattice (GPU allocation is here)
		debug0("Creating Lattice object ...");
		LatticePromise prom;
		prom.region = region;
		prom.mpi = mpi;
		prom.ns = ns;
		prom.latticeType = latticeType_;
		prom.latticeSize = latticeSize_;
//		lattice = new Lattice(region, mpi, ns);
		lattice = LatticeFactory::Produce(prom);
		if (lattice == 0) {
			ERROR("Could not find Lattice to produce\n");
			return -1;
		}
		output("Lattice created (model: %s)\n", lattice->model->name.c_str());
	   	debug0("Lattice done");

	   	LogScales = new double[
	   		lattice->model->globals.size() +
	   		lattice->model->settings.size() +
	   		lattice->model->zonesettings.size() +
	   		lattice->model->scales.size()
		];

		for (ModelBase::ZoneSettings::const_iterator it=lattice->model->zonesettings.begin(); it != lattice->model->zonesettings.end(); it++) {
			lattice->zSet.set(it->id, -1, units.alt(it->defaultValue));
		}
		latticeType = latticeType_;
		geometry = new Geometry(region, mpi.totalregion, units, lattice->model);
		connectivity = new Connectivity(region, mpi.totalregion, units, lattice->model);
		return 0;
	}

/// Runs the main loop (GUI)
/**
	Runs the main loop in the case of the GUI version
*/
	int Solver::RunMainLoop() {
		#ifdef GRAPHICS
        		bitmap->mouse_move( (void (*)(void*,int,int,int,int)) MouseMove);
		        bitmap->anim_and_exit( (int (*)(uchar4*,void*,int)) MainLoop, (void (*)(void*))MainFree );
			glutMainLoopEvent();
		#endif
		return 0;
	}

/// Idle function of main loop (GUI)
/**
	Part of the loop in the case of GUI version
*/
	int Solver::EventLoop(){
		#ifdef GRAPHICS
			bitmap->idle_func();
			glutMainLoopEvent();
		#endif
		return 0;
	}

/// Mouse Move callback (GUI)
/**
	Function called when mouse is moved, wile button pressed in GUI version
*/
void MouseMove( Solver * data, int x, int y, int nx, int ny )
{
	lbRegion r(
		x,
		data->region.ny - y - 1,
		0,
	1,1,1);
	ModelBase::NodeTypeFlags::const_iterator it = data->lattice->model->nodetypeflags.ByName("Wall");
	if (it != data->lattice->model->nodetypeflags.end()) {
		big_flag_t NodeType = it->flag;
		data->lattice->FlagOverwrite(&NodeType,r); // Overwrite mesh flags with flags from 'mask' table
	}
}

/// Refresh callback (GUI)
/**
	Function called when refresh of the window is needed.
	Renders the graphics inside of the window in GUI version
*/
int MainLoop( uchar4* outputBitmap, Solver *d, int ticks )
{
	d->lattice->Color(outputBitmap); // Updating graphics
	return 0;
}

/// Clean-up
/**
	Empty now. TODO
*/
void MainFree( Solver *d ) {

}