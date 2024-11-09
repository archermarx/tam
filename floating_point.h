#ifndef TAM_FLOATING_POINT_H
#define TAM_FLOATING_POINT_H

extern const double Inf;
extern const double Nan;
extern const float epsf;
extern const double eps;

#ifndef TAM_FLOATING_POINT_IMPLEMENTED
#define TAM_FLOATING_POINT_IMPLEMENTED

const double Inf = 1.0/0.0;
const double NaN = 0.0/0.0;

const float epsf = 0x1p-23; 
const double eps = 0x1p-52;

#endif // TAM_FLOATING_POINT_IMPLEMENTED
#endif // TAM_FLOATING_POINT_H
