#ifndef RUNR_H
#define RUNR_H
    #include <string>

    class RInside;
    class Solver;

    namespace RunR {
        RInside& GetR();
        int replInit();
        int replDo();
        void parseEval(const std::string& source);
    };
#endif // RUNR_H
