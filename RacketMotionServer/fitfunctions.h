#ifndef FITFUNCTIONS_H
#define FITFUNCTIONS_H

#include <vector>
#include <dlib-19.4/dlib/optimization.h>


using namespace dlib;

/*
 *  Parameters vector of function p(0) + p(1) * exp(- (x - p(2))**2 / (2 * p(3)**2)):
 */
typedef matrix<double,4,1> normPar;  // normal destribution parmeters

// ----------------------------------------------------------------------------------------
namespace MathFit {

    // Calculates mean value of the vector:
    double mean(std::vector<double> vec);

    // We will use this function to generate data.  It represents a function of 2 variables
    // and 4 parameters.   The least squares procedure will be used to infer the values of
    // the 4 parameters based on a set of input/output pairs.
    double modelNormal (const double &x,
                        const normPar &p);

    // This function is the "residual" for a least squares problem. It takes an input/output
    // pair and compares it to the output of our model and returns the amount of error.  The idea
    // is to find the set of parameters which makes the residual small on all the data pairs.
    double residual (const std::pair<double, double> &data,  // (x,y)
                     const normPar &p);
    // const std::pair<input_vector, double>& data,
    // const parameter_vector& params

    // This function is the derivative of the residual() function with respect to the parameters.
    normPar residual_derivative (const std::pair<double, double> &data,
                                     const normPar &p);

    // fits normal destribution
    std::vector<double> fitNormal(const std::vector<double> dataX,
                                  const std::vector<double> dataY);
}

#endif // FITFUNCTIONS_H
