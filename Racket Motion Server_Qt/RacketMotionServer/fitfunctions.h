#ifndef FITFUNCTIONS_H
#define FITFUNCTIONS_H

#include <stdlib.h>
#include <math.h>
#include <vector>
#include <levmar-2.6/levmar.h>

#ifndef LM_DBL_PREC
#error Example program assumes that levmar has been compiled with double precision, see LM_DBL_PREC!
#endif

/* the following macros concern the initialization of a random number generator for adding noise */
#undef REPEATABLE_RANDOM
#define DBL_RAND_MAX (double)(RAND_MAX)

#ifdef _MSC_VER // MSVC
#include <process.h>
#define GETPID  _getpid
#elif defined(__GNUC__) // GCC
#include <sys/types.h>
#include <unistd.h>
#define GETPID  getpid
#else
#warning Do not know the name of the function returning the process id for your OS/compiler combination
#define GETPID  0
#endif /* _MSC_VER */

#ifdef REPEATABLE_RANDOM
#define INIT_RANDOM(seed) srandom(seed)
#else
#define INIT_RANDOM(seed) srandom((int)GETPID()) // seed unused
#endif


namespace MathFit {
    /*
     * model to be fitted to measurements:
     * x_i = p[0]*exp(-p[1]*i) + p[2], i=0...n-1
     */
    void expfunc(double *p, double *x, int m, int n, void *data);

    /*
     * Jacobian of expfunc()
     */
    void jacexpfunc(double *p, double *jac, int m, int n, void *data);

    /*
     * Fits exponential function
     *
     */
    void fitexp(const std::vector<double> data);
}

#endif // FITFUNCTIONS_H
