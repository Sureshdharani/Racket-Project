#include <fitfunctions.h>


//-----------------------------------------------------------------------------
void MathFit::expfunc(double *p, double *x, int m, int n, void *data)
{
    for(int i = 0; i < n; ++i)  {
        x[i] = p[0] * exp(-p[1] * i) + p[2];
    }
}

//-----------------------------------------------------------------------------
void MathFit::jacexpfunc(double *p, double *jac, int m, int n, void *data)
{

    // fill Jacobian row by row
    int j = 0;
    for(int i = 0; i < n; ++i) {
        jac[j++] = exp(-p[1] * i);
        jac[j++] = -p[0] * i * exp(-p[1] * i);
        jac[j++] = 1.0;
    }
}

//-----------------------------------------------------------------------------
void MathFit::fitexp(const std::vector<double> data)
{
    const int n = data.size();  // number of measurements
    const int m = 3;         // number of parameters
    double p[m], x[n], opts[LM_OPTS_SZ], info[LM_INFO_SZ];
    int ret;  // fit function return status

    // initial parameters estimate: (1.0, 0.0, 0.0)
    p[0] = 1.0;
    p[1] = 0.0;
    p[2] = 0.0;

    // optimization control parameters:
    // passing to levmar NULL instead of opts reverts to defaults
    opts[0]=LM_INIT_MU;
    opts[1]=1E-15;
    opts[2]=1E-15;
    opts[3]=1E-20;

    // relevant only if the finite difference Jacobian version is used:
    opts[4]=LM_DIFF_DELTA;

    // create copy of data to fit:
    for(unsigned int i = 0; i < data.size(); i++)
        x[i] = data.at(i);

    // invoke the optimization function:
    ret = dlevmar_der(expfunc, jacexpfunc,
                      p, x, m, n, 1000, opts, info,
                      NULL, NULL, NULL); // with analytic Jacobian
    // ret = dlevmar_dif(expfunc,
    //                   p, x, m, n, 1000, opts, info,
    //                   NULL, NULL, NULL); // without Jacobian
}
