#ifndef FITFUNCTIONS_H
#define FITFUNCTIONS_H

#include <vector>
#include <dlib-19.4/dlib/optimization.h>


using namespace dlib;

// ----------------------------------------------------------------------------------------
typedef matrix<double,2,1> input_vector;
typedef matrix<double,3,1> parameter_vector;

namespace MathFit {

    // We will use this function to generate data.  It represents a function of 2 variables
    // and 3 parameters.   The least squares procedure will be used to infer the values of
    // the 3 parameters based on a set of input/output pairs.
    double model (const input_vector &input, const parameter_vector &params);

    // This function is the "residual" for a least squares problem.   It takes an input/output
    // pair and compares it to the output of our model and returns the amount of error.  The idea
    // is to find the set of parameters which makes the residual small on all the data pairs.
    double residual (const std::pair<input_vector, double> &data, const parameter_vector &params);

    // This function is the derivative of the residual() function with respect to the parameters.
    parameter_vector residual_derivative (const std::pair<input_vector, double> &data,
                                          const parameter_vector &params);

    // fits exponential function
    void fitexp(const std::vector<double> data);
}

#endif // FITFUNCTIONS_H
