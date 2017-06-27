#ifndef FITFUNCTIONS_H
#define FITFUNCTIONS_H

#include <vector>


namespace MathFit {
    // calculates exponential function
    void expfunc();

    // jacobian of exponential function
    void jacexpfunc();

    // fits exponential function
    void fitexp(const std::vector<double> data);
}

#endif // FITFUNCTIONS_H
