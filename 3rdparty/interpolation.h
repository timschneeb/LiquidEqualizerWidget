namespace Interpolation {
    typedef struct
    {
        double* x_;
        double* y_;
        double* dydx_;
        int n;
    } cubic_hermite;
    typedef struct
    {
        cubic_hermite cb;
        double *s;
    } ierper;
    void initIerper(ierper *intp, int n);
    void freeIerper(ierper *intp);
    double getValueAt(cubic_hermite *cuher, double x);
    double getDerivativeAt(cubic_hermite *cuher, double x);
    void pchip(ierper *intp, double * x, double * y, int n, int left_endpoint_derivative, int right_endpoint_derivative);
    void makima(ierper *intp, double * x, double * y, int n, int left_endpoint_derivative, int right_endpoint_derivative);
}
