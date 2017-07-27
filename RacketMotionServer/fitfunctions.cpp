#include <fitfunctions.h>


//-----------------------------------------------------------------------------
double MathFit::gauss1b(const double &x, const normPar &p) {
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
    for (unsigned int i = 0; i < vec.size(); i++)
        sum += vec.at(i);

    return sum / static_cast<double>(vec.size());
}

//-----------------------------------------------------------------------------
double MathFit::residual (const std::pair<double, double> &data,
                          const normPar &p)
{
    // return modelNormal(data.first, params) - data.second;
    return gauss1b(data.first, p) - data.second;
}

//-----------------------------------------------------------------------------
normPar MathFit::residual_derivative (const std::pair<double, double> &data,
                                          const normPar &p)
{
    normPar d;

    /*
    const double p0 = params(0);
    const double p1 = params(1);
    const double p2 = params(2);

    const double i0 = data.first(0);
    const double i1 = data.first(1);

    const double temp = p0*i0 + p1*i1 + p2;

    der(0) = i0*2*temp;
    der(1) = i1*2*temp;
    der(2) = 2*temp;
    */

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
std::vector<double> MathFit::fitGauss1b(const std::vector<double> dataX,
                                       const std::vector<double> dataY) {
    try {
        // randomly pick a set of parameters to use in this example
        // const parameter_vector params = 10*randm(3,1);


        // Crreate data according to model:
        normPar p;
        p(0) = *std::min_element(dataY.begin(), dataY.end());
        p(1) = *std::max_element(dataY.begin(), dataY.end()) - p(0);
        p(2) = dataX.at(dataX.size()/2);
        p(3) = 0.1;


        // Now let's generate a bunch of input/output pairs according to our model.
        /*
        std::vector<std::pair<input_vector, double> > data_samples;
        input_vector input;
        for (int i = 0; i < dataX.size(); ++i)
        {
            input = 10 * randm(2,1);
            const double output = modelNormal(input, params);

            // save the pair
            data_samples.push_back(std::make_pair(input, output));
        }
        */

        // create data samples:
        std::vector<std::pair<double, double> > data_samples;
        for (int i = 0; i < dataX.size(); ++i)
            data_samples.push_back(std::make_pair(dataX.at(i), dataY.at(i)));

        // Use the Levenberg-Marquardt method to determine the parameters which
        // minimize the sum of all squared residuals.
        solve_least_squares_lm(objective_delta_stop_strategy(1e-7),
                               residual,
                               residual_derivative,
                               data_samples,
                               p);


        // return fitted data points:
        std::vector<double> out;
        for (unsigned int i = 0; i < dataX.size(); i++)
            out.push_back(gauss1b(dataX.at(i), p));
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
