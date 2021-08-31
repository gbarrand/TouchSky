//------------------------------------------------------------------------------------
// SOPHYA class library - (C)  UPS+LAL IN2P3/CNRS , (C) CEA-Irfu-SPP
// Angles/3D geometry - class LongitudeLatitude - a position on the sphere 
// B. Revenu 2000  , R. Ansari 2006 , Updated 2015 
//------------------------------------------------------------------------------------

#ifndef LONGLAT_H_SEEN
#define LONGLAT_H_SEEN

#include "machdefs.h"
#include <iostream>

#include "angle.h"

namespace SOPHYA {  


//--------------------------------------------------------------------------------   
//-------------------------  LongitudeLatitude class ----------------------------
//--------------------------------------------------------------------------------   

// forward declaration of Vector3d and UnitVector classes used by LongitudeLatitude class
class Vector3d; 
class UnitVector;

class LongitudeLatitude  {
public:
  //! Default constructor: longitude=Phi=0., Theta=0 -> latitude=Pi/2 
  inline LongitudeLatitude() : theta_(0.), phi_(0.) { }
  //! Constructor with specification of longitude and latitude as Angle objects 
  LongitudeLatitude(Angle longitude, Angle latitude, bool fg /*true*/);
  //! Constructor with specification of theta and phi as float numbers in radians  
  LongitudeLatitude(double theta, double phi);
  //! Constructor, with specification of a point in space (cartesian coordinates) 
  LongitudeLatitude(double x, double y, double z);
  //! Constructor, with specification of a point in space as Vector3d  
  LongitudeLatitude(Vector3d const & v);
  //! Copy constructor 
  inline LongitudeLatitude(LongitudeLatitude const & a) : theta_(a.theta_) , phi_(a.phi_) { }
  //! Copy operator 
  inline LongitudeLatitude operator = (LongitudeLatitude const & a)
  { theta_=a.theta_; phi_=a.phi_; return *this; }

  //! Set the Theta/Phi angles (in radians)  
  void SetThetaPhi(double theta, double phi);
  //! Set the Theta/Phi angles, with arguments specified as Angle objects  
  inline void SetThetaPhi(Angle theta, Angle phi)  { SetThetaPhi(theta.ToRadian(), phi.ToRadian()); }
  //! Set the Longitude and Latitude 
  void SetLongLat(Angle longitude, Angle latitude, bool fg=true);

  //! Return the longitude as an Angle
  inline Angle Longitude() const 
  { return Angle(phi_,Angle::Radian); }
  //! Return the latitude as an Angle
  inline Angle Latitude() const 
  { return Angle(Angle::PioTwoCst()-theta_,Angle::Radian); }
  //! Return the co-latitude (=theta) as an Angle
  inline Angle Colatitude() const 
  { return Angle(theta_,Angle::Radian); }
  //! alias for Longitude()
  inline Angle RightAscension() const 
  { return Angle(phi_,Angle::Radian); }
  //! alias for Latitude()
  inline Angle Declination() const 
  { return Angle(Angle::PioTwoCst()-theta_,Angle::Radian); }

  //! Return the theta value (in radians) 
  double Theta() const {return theta_;}
  //! Return the phi value (in radians) 
  inline double Phi() const {return phi_;}

  //! return a UnitVector pointing toward the direction defined by (*this)
  UnitVector getUnitVector() const;
  //! return the separation angle between the two directions (*this) and (sd) 
  double  SepAngle(LongitudeLatitude const& sd) const;

  //! print the object (ascii representation) on stream os - no newline appended.
  std::ostream& Print(std::ostream& os) const;
  //! return true if the two objects are equal - the angles are equal with the tolerance defined by Angle::getZeroTolerance()
  inline bool isEqual(LongitudeLatitude const& a)   const
  { 
    double dt=theta_-a.theta_; double dp=phi_-a.phi_; 
    double zerotol=Angle::getZeroTolerance();
    if ((dt>zerotol)||(-dt>zerotol)||(dp>zerotol)||(-dp>zerotol))  return false;
    return true;
  }

private:  
  double theta_;   //!< (Pi/2 - latitude) 
  double phi_;     //!<  same as longitude 
  static double zerotol_;    //!<< tolerance when checking angles equality 
};

//! operator << overloading - Prints the LongitudeLatitude object \b ll on the stream \b os
inline std::ostream& operator<<(std::ostream& s, const LongitudeLatitude& ll) 
{  return ll.Print(s);   }

//! Equality operator (==) definition for two LongitudeLatitude objects   
inline bool operator == (LongitudeLatitude const & a, LongitudeLatitude const & b)
{ return a.isEqual(b); }

//! Non-Equality operator (!=) definition for two LongitudeLatitude objects   
inline bool operator != (LongitudeLatitude const & a, LongitudeLatitude const & b)
{ return !(a.isEqual(b)); }

//! shorthand for the class name (and for backward compatibility) 
typedef  LongitudeLatitude  LongLat ; 

} // namespace SOPHYA

#endif
