/* ------------------------------------------------------------------------------------
   SOPHYA class library - (C)  UPS+LAL IN2P3/CNRS  , (C) CEA-Irfu-SPP 
   Class Angle - conversion between different angle units 
   R. Ansari, LAL/IN2P3-CNRS, Univ. Paris Sud - 2006 
   Updated: 2015 
   ------------------------------------------------------------------------------------  */

#include <math.h>

#include "machdefs.h"
#include "angle.h"

using namespace std;

namespace SOPHYA {

// Class de conversion d'angles R. Ansari , Juin 2006 - Updated March 2015 


/*!
   \class Angle
   \ingroup SkyMap
   \brief  Class to ease angle manipulation and conversions (radian <> degree <> arcmin <> arcsec).
   The angle value is kept in radians internally.
   \code
   // Example to convert 0.035 radians to arcsec
   double vr = 0.035;
   cout << "vr: " << vr << " ->arcsec= " << Angle(vr).ToArcSec() << endl;
   // Example to convert 2.3 arcmin to radian - we use the conversion operator
   double vam = 2.3;
   cout << "Angle arcmin= " << vam << " ->rad= " 
        << (double)Angle(vam, Angle::ArcMin) << endl;
   \endcode

*/

// Les constantes ont ete calcule avec Mathematica, avec une precision de 30 digits
// N[Pi,30]  N[Pi/180,30] ...
double Angle::_deg2rad = 0.0174532925199432957692369076849;     // Pi/180.;
double Angle::_min2rad = 0.000290888208665721596153948461415;   // Pi/(180.*60.);
double Angle::_sec2rad = 4.84813681109535993589914102358e-6;    // Pi/(180.*3600.);
double Angle::_rad2deg = 57.2957795130823208767981548141;       // 180./Pi;
double Angle::_rad2min = 3437.74677078493925260788928885;       // 180*60/Pi;
double Angle::_rad2sec = 206264.806247096355156473357331;       // 180./M_PI*3600.;
double Angle::_api = 3.14159265358979323846264338328;           // Pi
double Angle::_a2pi = 6.283185307179586476925286766568;         // 2xPi
double Angle::_apio2 = 1.57079632679489661923132169164;         // Pi/2

// Tolerance ==0 lors de la comparaison des angles 
double Angle::zerotol_ = 1.e-9;

/*!
  \param zthr : two angles considered equal if their difference is < zthr. if (zthr < 0.) use std::numeric_limits<double>::min()
  \param prt : if > 0 , print the new zero threshold value
*/
void Angle::setZeroTolerance(double zthr, int prt)
{
  if (zthr<0.)  {
    if (prt>0)  cout<<"Angle::setZeroTolerance() zthr<0. -> using std::numeric_limits<double>::min()"<<endl;
    zthr=std::numeric_limits<double>::min();
  }
  zerotol_=zthr;
  if (prt>0)  cout<<"Angle::setZeroTolerance() setting difftol_ to "<<zthr<<endl;  
  return;
}

/*--Methode*/
Angle::Angle(double val, Angle::AngleUnit un)
{
  switch (un) {
  case Angle::Radian :
    _angrad = val;
    break;
  case Angle::Degree :
    _angrad = val*_deg2rad;
    break;
  case Angle::ArcMin :
    _angrad = val*_min2rad;
    break;
  case Angle::ArcSec :
    _angrad = val*_sec2rad;
    break;
  default:
    _angrad = val;
    break;
  }
}

}  // End of namespace SOPHYA
