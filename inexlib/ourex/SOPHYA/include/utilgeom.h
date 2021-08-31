//------------------------------------------------------------------------------------
// SOPHYA class library - (C)  UPS+LAL IN2P3/CNRS  , (C) CEA-Irfu-SPP 
//   Utility functions for spherical trigonometry 
// R. Ansari, C. Magneville 
//------------------------------------------------------------------------------------

#ifndef UTILGEOM_H_SEEN
#define UTILGEOM_H_SEEN
#include <math.h>
#include <vector>

namespace SOPHYA {

const double pi2=2.*M_PI;
const double dtor=M_PI/180.;
const double pi_over_2=M_PI/2.;

int sign(double);
double absd(double);
double mod(double, double=0);
void swap(double&, double&);
double min(double, double);
double max(double, double);
int arrondi(double);
double scangle(double sinus, double cosinus);

} // FIN namespace SOPHYA 

#endif
