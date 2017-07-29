#ifndef FITFUNCTIONS_H
#define FITFUNCTIONS_H

#include <vector>
#include <numeric>
#include <algorithm>
#include <dlib-19.4/dlib/optimization.h>

using namespace dlib;

/*
 *  Parameters vector of function p(0) + p(1) * exp(- (x - p(2))**2 / (2 * p(3)**2)):
 */
typedef matrix<double,4,1> G1bPar;  // gauss1 with bias parmeters
typedef matrix<double,7,1> G2bPar;  // gauss2 with bias parmeters

// ----------------------------------------------------------------------------------------
namespace MathFit {

    // Calculates mean value of the vector:
    double mean(std::vector<double> vec);

    // Computes median value of vector:
    double median(std::vector<double> scores);

    // Calculates absolut values of each element in te vector:
    std::vector<double> abs(std::vector<double> vec);

    // Returns the vector without offset:
    std::vector<double> sub_offset(std::vector<double> vec, const double offset);

    // Returns index of val in vec else 0:
    int idxOf(std::vector<double> vec, const double val);

    // Gaussian 1 with bias:
    double G1b(const double &x, const G1bPar &p);

    // Gaussian 2 with bias:
    double G2b(const double &x, const G2bPar &p);

    // Residual function of G1b
    // std::pair<double, double> &data ---> (x, y)
    double residualG1b(const std::pair<double, double> &data, const G1bPar &p);

    // Residual function of G2b
    // std::pair<double, double> &data ---> (x, y)
    double residualG2b(const std::pair<double, double> &data, const G2bPar &p);

    // Residual derviative of G1b:
    G1bPar res_dG1b(const std::pair<double, double> &data, const G1bPar &p);

    // Residual derviative of G2b:
    G2bPar res_dG2b(const std::pair<double, double> &data, const G2bPar &p);

    // Fit G1b:
    std::vector<double> fitG1b(const std::vector<double> dataX,
                               const std::vector<double> dataY);

    // Fit G2b:
    std::vector<double> fitG2b(const std::vector<double> dataX,
                               const std::vector<double> dataY);
}

#endif // FITFUNCTIONS_H
