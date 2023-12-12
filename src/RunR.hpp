#ifndef RUNR_HPP
#define RUNR_HPP
    #include "../config.h"
    #include "RunR.h"
    #ifdef WITH_R
        #include <RcppCommon.h>
        #include <Rcpp.h>
        #include <RInside.h>                            // for the embedded R via RInside
        #undef Free					// Conflict of names
        #undef WARNING
        #undef ERROR

        #include "Solver.h"
        #include "Handlers/vHandler.h"
        template <class T>
        SEXP rWrap(Solver* solver, vHandler * hand, const T& ptr);
        SEXP rWrap(Solver* solver, vHandler * hand);
    #else
        class RInside { };
    #endif
#endif // RUNR_HPP
