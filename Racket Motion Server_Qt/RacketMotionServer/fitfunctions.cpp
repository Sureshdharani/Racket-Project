#include <fitfunctions.h>


//-----------------------------------------------------------------------------
double MathFit::model (const input_vector& input,
                       const parameter_vector& params)
{
    const double p0 = params(0);
    const double p1 = params(1);
    const double p2 = params(2);

    const double i0 = input(0);
    const double i1 = input(1);

    const double temp = p0*i0 + p1*i1 + p2;

    return temp*temp;
}

//-----------------------------------------------------------------------------
double MathFit::residual (const std::pair<input_vector, double> &data,
                          const parameter_vector &params)
{
    return model(data.first, params) - data.second;
}

//-----------------------------------------------------------------------------
parameter_vector MathFit::residual_derivative (
        const std::pair<input_vector, double> &data,
        const parameter_vector &params)
{
    parameter_vector der;

    const double p0 = params(0);
    const double p1 = params(1);
    const double p2 = params(2);

    const double i0 = data.first(0);
    const double i1 = data.first(1);

    const double temp = p0*i0 + p1*i1 + p2;

    der(0) = i0*2*temp;
    der(1) = i1*2*temp;
    der(2) = 2*temp;

    return der;
}

//-----------------------------------------------------------------------------
void MathFit::fitexp(const std::vector<double> data)
{
    try {
        // randomly pick a set of parameters to use in this example
        const parameter_vector params = 10*randm(3,1);
        // cout << "params: " << trans(params) << endl;


        // Now let's generate a bunch of input/output pairs according to our model.
        std::vector<std::pair<input_vector, double> > data_samples;
        input_vector input;
        for (int i = 0; i < 1000; ++i)
        {
            input = 10*randm(2,1);
            const double output = model(input, params);

            // save the pair
            data_samples.push_back(std::make_pair(input, output));
        }

        // Before we do anything, let's make sure that our derivative function defined above matches
        // the approximate derivative computed using central differences (via derivative()).
        // If this value is big then it means we probably typed the derivative function incorrectly.
        // cout << "derivative error: " << length(residual_derivative(data_samples[0], params) -
        //                                        derivative(residual)(data_samples[0], params) ) << endl;





        // Now let's use the solve_least_squares_lm() routine to figure out what the
        // parameters are based on just the data_samples.
        parameter_vector x;
        x = 1;

        // cout << "Use Levenberg-Marquardt" << endl;
        // Use the Levenberg-Marquardt method to determine the parameters which
        // minimize the sum of all squared residuals.
        solve_least_squares_lm(objective_delta_stop_strategy(1e-7).be_verbose(),
                               residual,
                               residual_derivative,
                               data_samples,
                               x);

        // Now x contains the solution.  If everything worked it will be equal to params.
        // cout << "inferred parameters: "<< trans(x) << endl;
        // cout << "solution error:      "<< length(x - params) << endl;
        // cout << endl;




        x = 1;
        // cout << "Use Levenberg-Marquardt, approximate derivatives" << endl;
        // If we didn't create the residual_derivative function then we could
        // have used this method which numerically approximates the derivatives for you.
        solve_least_squares_lm(objective_delta_stop_strategy(1e-7).be_verbose(),
                               residual,
                               derivative(residual),
                               data_samples,
                               x);

        // Now x contains the solution.  If everything worked it will be equal to params.
        // cout << "inferred parameters: "<< trans(x) << endl;
        // cout << "solution error:      "<< length(x - params) << endl;
        // cout << endl;




        x = 1;
        // cout << "Use Levenberg-Marquardt/quasi-newton hybrid" << endl;
        // This version of the solver uses a method which is appropriate for problems
        // where the residuals don't go to zero at the solution.  So in these cases
        // it may provide a better answer.
        solve_least_squares(objective_delta_stop_strategy(1e-7).be_verbose(),
                            residual,
                            residual_derivative,
                            data_samples,
                            x);

        // Now x contains the solution.  If everything worked it will be equal to params.
        // cout << "inferred parameters: "<< trans(x) << endl;
        // cout << "solution error:      "<< length(x - params) << endl;

    }
    catch (std::exception& e) {
        // cout << e.what() << endl;
    }
}
