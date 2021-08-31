/* ------------------------------------------------------------------------------------
  SOPHYA class library - (C)  UPS+LAL IN2P3/CNRS , (C) CEA-Irfu-SPP
  Angles/3D geometry - class LongitudeLatitude - a position on the sphere 
  B. Revenu 2000  , R. Ansari 2006,2015 
  ------------------------------------------------------------------------------------ */

#include <math.h>
#include "longlat.h"
#include "pexceptions.h"
#include "vector3d.h"
#include "unitvector.h"

using namespace std;

namespace SOPHYA {  

/*!
   \class LongitudeLatitude
   \ingroup SkyMap

   \brief Class representing a point (position) on a sphere through two angles. 
   
   Each class instance keeps internally the two angles (Theta and Phi) in radians.
   We consider a sphere with centre O , 
   Oz is the axes going through the poles, xOy is the equatorial plane 
   Given a point M on the sphere, P being the M projection on the xOy plane:
   
   Theta = angle(Oz, OM) , 0 <= Theta <= Pi , Theta=0/Pi -> North/South pole 
   Phi = angle(Ox, OP) , 0<= Phi < 2 Pi
   
   Longitude stands for Phi , named also Lambda(), or RA (RightAscension())
   Latitude is the angle with respect to the equatorial plane =  Pi/2 - Theta.
   Declination() or Beta()  are other names that refer to Latitude 
   Colatitude corresponds to Theta  = Pi/2 - Latitude  
   
   \sa SOPHYA::Angle 
*/

/*! 
  \param longitude : longitude or right ascension angle  0 <= longitude < 2 Pi.
  However, the extended range in longitude -2 Pi < longitude < 4 Pi is accepted, converted to [0,2Pi[ 
  \param latitude : latitude or declination angle -Pi/2 <= latitude <= Pi/2 (or colatitude, see below)
  \param fg : if true, the second angle represent the latitude (zero at equator),
  if not (false), the second angle represent the co-latitude = Pi/2-latitude.
  There is no default value for fg,  in order to avoid confusion with the constructor with the two 
  angles (theta,phi) specification.

  \warning throws a ParmError exception if longitude or latitude is out of range 
*/
/* --Methode-- */
  LongitudeLatitude::LongitudeLatitude(Angle longitude, Angle latitude, bool fg)
{
  SetLongLat(longitude, latitude, fg);
}

/*! 
  \param theta : spherical theta angle  0 <= theta <= Pi.
  \param phi : spherical phi angle 0 <= phi < 2 Pi 
     however, phi in the extended range -2 Pi < phi < 4 Pi is accepted. if phi < 0., phi -> phi+2 Pi , if phi > 2 Pi , phi -> phi - 2 Pi

  \warning throws a ParmError exception if theta or phi is out of range 
*/

LongitudeLatitude::LongitudeLatitude(double theta, double phi)
{
  SetThetaPhi(theta, phi);
}

/* --Methode-- */
LongitudeLatitude::LongitudeLatitude(double x, double y, double z)
{
  Vector3d v(x,y,z);
  SphVector3D sv(v);
  SetThetaPhi(sv.Theta(), sv.Phi());
}

/* --Methode-- */
LongitudeLatitude::LongitudeLatitude(Vector3d const& v)
{
  SetThetaPhi(SphVector3D(v).Theta(), SphVector3D(v).Phi());
}

/*! 
  \param theta : the angle(Oz,OM)   0 <= theta <= Pi 
  \param phi :  the angle(Ox,OP)    0 <= phi < 2 Pi , however, the method accepts phi 
  in the extended range -2 Pi < phi < 4 Pi , if phi < 0., phi -> phi+2 Pi , if phi > 2 Pi , phi -> phi - 2 Pi

  \warning throws a ParmError exception if theta of phi is out of range 
*/
/* --Methode-- */
void LongitudeLatitude::SetThetaPhi(double theta, double phi)
{
  if ((theta<0.)||(theta>Angle::OnePiCst()))  
    throw ParmError("LongitudeLatitude::SetThetaPhi(theta,phi) - Out of bounds (0,Pi) theta");
  if (phi<0.) phi += Angle::TwoPiCst();
  if (phi>Angle::TwoPiCst())  phi -= Angle::TwoPiCst(); 
  if ((phi<0.)||(phi>Angle::TwoPiCst()))  
    throw ParmError("LongitudeLatitude::SetThetaPhi(theta,phi) - Out of bounds (0,2Pi) phi");
  theta_=theta;
  phi_=phi;
  if (phi_<0.)  phi_=0.;
  if (phi>=Angle::TwoPiCst())  phi_=0.; 
  if (theta_<0.)  theta_=0.;
  if (theta_>Angle::OnePiCst())  theta_=Angle::OnePiCst();
  return;
}

/*! 
  \param longitude : longitude or right ascension angle  0 <= longitude < 2 Pi.
  However, the extended range in longitude -2 Pi < longitude < 4 Pi is accepted, converted to [0,2Pi[ 
  \param latitude : latitude or declination angle -Pi/2 <= latitude <= Pi/2 (or colatitude, see below)
  \param fg : if true, the second angle represent the latitude (zero at equator),
  if not (false), the second angle represent the co-latitude = Pi/2-latitude.

  \warning throws a ParmError exception if longitude or latitude is out of range 
*/
/* --Methode-- */
void LongitudeLatitude::SetLongLat(Angle longitude, Angle latitude, bool fg)
{
  double lng = longitude.ToRadian();
  double lat = (fg?latitude.ToRadian():Angle::PioTwoCst()-latitude.ToRadian());
  if (lng<0.) lng+=Angle::TwoPiCst();
  if (lng>Angle::TwoPiCst()) lng-=Angle::TwoPiCst();
  if ((lng<0.)||(lng > Angle::TwoPiCst()))  
    throw ParmError("LongitudeLatitude::SetLongLat(long,lat) - Out of bounds (0,2Pi) longitude");
  if ((lat<-Angle::PioTwoCst())||(lat>Angle::PioTwoCst()))  
    throw ParmError("LongitudeLatitude::SetLongLat(long,lat) - Out of bounds (-Pi/2, Pi/2) latitude");
  phi_ = lng; 
  if (phi_<0.)  phi_=0.;
  if (phi_>=Angle::TwoPiCst())  phi_=0.; 
  theta_ = Angle::PioTwoCst()-lat;
  if (theta_<0.)  theta_=0.;
  if (theta_>Angle::OnePiCst())  theta_=Angle::OnePiCst();
}

/* --Methode-- */
UnitVector LongitudeLatitude::getUnitVector() const
{
  return UnitVector(*this);
}

/* --Methode-- */
double LongitudeLatitude::SepAngle(LongitudeLatitude const& sd) const
{
  return Vector3d(*this).SepAngle(Vector3d(sd)) ;
}

/* --Methode-- */
std::ostream& LongitudeLatitude::Print(std::ostream& os) const
{
  os << "LongitudeLatitude(long="<<Longitude().ToDegree()<<" ,lat="<<Latitude().ToDegree()<<" deg ->Theta="
     <<Theta()<<" ,Phi="<<Phi()<<")";
  return os;
}

} // End of namespace SOPHYA
