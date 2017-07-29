#include <fitfunctions.h>


//-----------------------------------------------------------------------------
double MathFit::G1b(const double &x, const G1bPar &p) {
    // Normal distribution: b + a * exp(- (x - m)^2 / (2 * s^2))
    // p(0) = b
    // p(1) = a
    // p(2) = m
    // p(3) = s
    const double ret = p(0) + (p(1) * std::exp((-1.0 *
                                                std::pow((x - p(2)), 2))
                                               / (2 * std::pow(p(3), 2))));
    return ret;
}

//-----------------------------------------------------------------------------
double MathFit::mean(std::vector<double> vec) {
    double sum = 0;
    for (size_t i = 0; i < vec.size(); i++)
        sum += vec.at(i);
    return sum / static_cast<double>(vec.size());
}

//-----------------------------------------------------------------------------
double MathFit::median(std::vector<double> vec) {
  double median = 0;

  std::sort(vec.begin(), vec.end());
  if (vec.size() % 2 == 0)
      median = (vec[vec.size() / 2 - 1] + vec[vec.size() / 2]) / 2;
  else
      median = vec[vec.size() / 2];
  return median;
}

//-----------------------------------------------------------------------------
std::vector<double> MathFit::abs(std::vector<double> vec) {
    std::vector<double> absVec(vec.size());
    for (size_t i = 0; i < vec.size(); i++)
        absVec.at(i) = std::abs(vec.at(i));
    return absVec;
}

//-----------------------------------------------------------------------------
std::vector<double> MathFit::sub_offset(std::vector<double> vec,
                                        const double offset) {
    std::vector<double> vecNoOffset(vec.size());
    for (size_t i = 0; i < vec.size(); i++)
        vecNoOffset.at(i) = vec.at(i) - offset;
    return vecNoOffset;
}

//-----------------------------------------------------------------------------
int MathFit::idxOf(std::vector<double> vec, const double val) {
    int pos = std::find(vec.begin(), vec.end(), val) - vec.begin();
    return pos < vec.size() ? pos : 0;
}

//-----------------------------------------------------------------------------
double MathFit::residualG1b(const std::pair<double, double> &data,
                            const G1bPar &p) {
    return G1b(data.first, p) - data.second;
}

//-----------------------------------------------------------------------------
G1bPar MathFit::res_dG1b (const std::pair<double, double> &data,
                          const G1bPar &p) {
    G1bPar d;

    // Derivative function of normal distribution with respect to parameters:
    // Normal distribution: b + a * exp(- (x - m)^2 / (2 * s^2))
    // p(0) = b
    // p(1) = a
    // p(2) = m
    // p(3) = s

    const double x = data.first;
    const double exp = std::exp((-1.0 * std::pow((x - p(2)), 2))
                                / (2 * std::pow(p(3), 2)));
    // Derivative with respect to bias - b:
    d(0) = 1.0;

    // Derivative with respect to amplitude - a:
    d(1) = exp;

    // Derivative with respect to mean - m:
    d(2) = p(1) * ((x - p(2)) / std::pow(p(3), 2)) * exp;

    // Derivative with respect to standart deviation - s:
    d(3) = p(1) * (std::pow((x - p(2)), 2) / std::pow(p(3), 3)) * exp;

    return d;
}

//-----------------------------------------------------------------------------
std::vector<double> MathFit::fitG1b(const std::vector<double> dataX,
                                    const std::vector<double> dataY) {
    try {
        // randomly pick a set of parameters to use in this example
        // const parameter_vector params = 10*randm(3,1);


        // Crreate data according to model:
        // p(0) = b
        // p(1) = a
        // p(2) = m
        // p(3) = s

        auto b = median(dataY);  // bias
        auto absY = abs(dataY);

        G1bPar p;
        p(0) = b;
        p(1) = *std::max_element(absY.begin(), absY.end());

        auto idxAMax = idxOf(absY, p(1));
        p(1) = std::copysign(p(1), dataY.at(idxAMax));
        p(2) = dataX.at(idxAMax);
        p(3) = 1000;

        // Create data samples:
        std::vector<std::pair<double, double> > samples;
        for (size_t i = 0; i < dataX.size(); ++i)
            samples.push_back(std::make_pair(dataX.at(i), dataY.at(i)));

        // Use the Levenberg-Marquardt method to determine the parameters which
        // minimize the sum of all squared residuals.
        solve_least_squares_lm(objective_delta_stop_strategy(1e-3),
                               residualG1b,
                               res_dG1b,
                               samples,
                               p);

        // Return fitted data points:
        std::vector<double> out;
        for (unsigned int i = 0; i < dataX.size(); i++)
            out.push_back(G1b(dataX.at(i), p));
        return out;

        // x = 1;
        // cout << "Use Levenberg-Marquardt, approximate derivatives" << endl;
        // If we didn't create the residual_derivative function then we could
        // have used this method which numerically approximates the derivatives for you.
        // solve_least_squares_lm(objective_delta_stop_strategy(1e-7),
        //                       residual,
        //                       derivative(residual),
        //                       data_samples,
        //                       x);

        // Now x contains the solution.  If everything worked it will be equal to params.
        // cout << "inferred parameters: "<< trans(x) << endl;
        // cout << "solution error:      "<< length(x - params) << endl;
        // cout << endl;




        // x = 1;
        // cout << "Use Levenberg-Marquardt/quasi-newton hybrid" << endl;
        // This version of the solver uses a method which is appropriate for problems
        // where the residuals don't go to zero at the solution.  So in these cases
        // it may provide a better answer.
        // solve_least_squares(objective_delta_stop_strategy(1e-7),
        //                    residual,
        //                    residual_derivative,
        //                    data_samples,
        //                    x);

        // Now x contains the solution.  If everything worked it will be equal to params.
        // cout << "inferred parameters: "<< trans(x) << endl;
        // cout << "solution error:      "<< length(x - params) << endl;

    }
    catch (std::exception& e) {
        // cout << e.what() << endl;
    }
}
