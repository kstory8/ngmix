#ifndef _PYGMIX_HEADER_GUARD
#define _PYGMIX_HEADER_GUARD

//struct PyGMix_Gauss2D {
struct __attribute__((__packed__)) PyGMix_Gauss2D {
    double p;
    double row;
    double col;

    double irr;
    double irc;
    double icc;

    double det;

    double drr;
    double drc;
    double dcc;

    double norm;
    double pnorm;
};

//struct PyGMix_Jacobian {
struct __attribute__((__packed__)) PyGMix_Jacobian {
    double row0;
    double col0;

    double dudrow;
    double dudcol;
    double dvdrow;
    double dvdcol;

    double det;
    double sdet;
};

/*
 *
 * fast exponential function
 *
 */

union pygmix_fmath_di {
    double d;
    uint64_t i;
};

static inline double expd(double x)
{

// holds definition of the table and C1,C2,C3, a, ra
#include "fmath-dtbl.c"

    union pygmix_fmath_di di;

    di.d = x * a + b;
    uint64_t iax = dtbl[di.i & sbit_masked];

    double t = (di.d - b) * ra - x;
    uint64_t u = ((di.i + adj) >> sbit) << 52;
    double y = (C3 - t) * (t * t) * C2 - t + C1;

    di.i = u | iax;
    return y * di.d;
}


// will check > -26 and < 0.0 so these are not actually necessary
//static int _exp3_ivals[] = {-26, -25, -24, -23, -22, -21, 
//                            -20, -19, -18, -17, -16, -15, -14,
//                            -13, -12, -11, -10,  -9,  -8,  -7,
//                            -6,  -5,  -4,  -3,  -2,  -1,   0};
/*
static int _exp3_i0=-26;
static double _exp3_lookup[] = {  5.10908903e-12,   1.38879439e-11,   3.77513454e-11,
                                  1.02618796e-10,   2.78946809e-10,   7.58256043e-10,
                                  2.06115362e-09,   5.60279644e-09,   1.52299797e-08,
                                  4.13993772e-08,   1.12535175e-07,   3.05902321e-07,
                                  8.31528719e-07,   2.26032941e-06,   6.14421235e-06,
                                  1.67017008e-05,   4.53999298e-05,   1.23409804e-04,
                                  3.35462628e-04,   9.11881966e-04,   2.47875218e-03,
                                  6.73794700e-03,   1.83156389e-02,   4.97870684e-02,
                                  1.35335283e-01,   3.67879441e-01,   1.00000000e+00};
*/

#define PYGMIX_MAX_CHI2 25.0

#define PYGMIX_GAUSS_EVAL(gauss, rowval, colval) ({            \
    double _u = (rowval)-(gauss)->row;                         \
    double _v = (colval)-(gauss)->col;                         \
    double _g_val=0.0;                                         \
                                                               \
    double _chi2 =                                             \
          (gauss)->dcc*_u*_u                                   \
        + (gauss)->drr*_v*_v                                   \
        - 2.0*(gauss)->drc*_u*_v;                              \
                                                               \
    if (_chi2 < PYGMIX_MAX_CHI2) {                             \
        _g_val = (gauss)->pnorm*expd( -0.5*_chi2 );            \
    }                                                          \
                                                               \
    _g_val;                                                    \
})

#define PYGMIX_GMIX_EVAL(gmix, n_gauss, rowval, colval) ({     \
    int _i=0;                                                  \
    double _gm_val=0.0;                                        \
    struct PyGMix_Gauss2D* _gauss=gmix;                        \
    for (_i=0; _i< (n_gauss); _i++) {                          \
        _gm_val += PYGMIX_GAUSS_EVAL(_gauss, (rowval), (colval)); \
        _gauss++;                                              \
    }                                                          \
    _gm_val;                                                   \
})


#endif
