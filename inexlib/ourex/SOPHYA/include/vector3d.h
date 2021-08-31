//------------------------------------------------------------------------------------
//  class Vector3d :   to handle 3-D geometry
// SOPHYA class library - (C)  UPS+LAL IN2P3/CNRS  , CEA-Irfu 
//        B. Revenu, G. Le Meur   2000
//        R. Ansari 2006  - updated Jan 2015 
//------------------------------------------------------------------------------------

#ifndef VECTOR3D_H_SEEN
#define VECTOR3D_H_SEEN

#include <math.h>
#include <iostream>
#include <stdio.h>
#include <string.h>

#include "angle.h"
#include "longlat.h"


/*
  Geometrie en dimension 3. 
  B. Revenu, G. Le Meur   2000
  R. Ansari  2006, 2015 
  Tous les calculs sont faits en radians 
  et en coordonnees spheriques theta,phi
  pour les rotations (angles d'Euler) ma source est 
  "Classical Mechanics" 2nd edition, H. Goldstein, Addison Wesley
*/

namespace SOPHYA { 

// Forward  SphVector3D declaration 
class SphVector3D;

//-----------------------------------------------------------
//-------------------- class Vector3d -----------------------

class Vector3d  {
public:
//! default constructor - null vector   
  Vector3d()  { _x=_y=_z=0.; }
//! constructor with specification of the 3 cartesian coordinates  
  explicit Vector3d(double x, double y, double z)
  { _x=x; _y=y; _z=z; }
//! copy constructor 
  Vector3d(const Vector3d & v)
  { _x=v._x; _y=v._y; _z=v._z; }
//! contructor: definition from spherical coordinates 
  explicit Vector3d(const LongitudeLatitude & ll, double norm=1.);
//! constructor from a SphVector3D object 
  Vector3d(const SphVector3D & v);

//! destructor 
  virtual ~Vector3d() { } 

//! change/set the vector from the cartesian coordinates 
  inline void Setxyz(double x, double y, double z)
  { _x=x; _y=y; _z=z; }
//! change/set the vector from the spherical coordinates 
  void SetThetaPhi(double theta, double phi, double norm=1.);
//! change/set the vector from the spherical coordinates 
  inline void SetThetaPhi(const LongitudeLatitude & ll, double norm=1.)
  {  SetThetaPhi(ll.Theta(), ll.Phi()); }

//! return the X cartesian coordinate 
  inline double X() const {return _x;}
//! return the Y cartesian coordinate 
  inline double Y() const {return _y;}
//! return the Z cartesian coordinate 
  inline double Z() const {return _z;}

//! return the vector norm (length)
  inline double Norm() const
  { return sqrt(_x*_x+_y*_y+_z*_z);  }
//! return the vector norm-square (length^2) 
  inline double Norm2() const
  { return (_x*_x+_y*_y+_z*_z);  }

//! rescale the coordinates to make the vector length equal unity - can throw MathExc 
  Vector3d& NormalizeSelf();
//! alias for  NormalizeSelf() for backward compatibility - DEPRECATED 
  inline Vector3d& Normalize() { return NormalizeSelf(); }

/*! \brief return the Theta,Phi as a LongitudeLatitude object 
  \warning phi (longitude) is set to zero if theta is 0 or Pi (within tolerance)
*/
  LongitudeLatitude  GetThetaPhi() const;

//! return the dot product of the two vectors (scalar product) = (*this) . v 
  inline double dot(const Vector3d & v) const
  {   return _x*v._x+_y*v._y+_z*v._z;  }
//! alias for dot(v) (*this) . v 
  inline double Psc(const Vector3d & v) const  { return dot(v); }
//! return the vector product of two vectors = (*this) ^ v 
  inline Vector3d vecprod(const Vector3d & v) const
  { return Vector3d(_y*v._z-_z*v._y, _z*v._x-_x*v._z,_x*v._y-_y*v._x);  }

  //!   angular gap between 2 vectors in [0,Pi] 
  double SepAngle(const Vector3d&) const;

  //! return the normalized copy of the vector itself, (unit vector along the radial direction) 
  inline Vector3d ur() const { return Vector3d(*this).Normalize(); }
  // vecteur perpendiculaire de meme phi
  //! return the perpendicular vector, with equal phi and norm=1 
  Vector3d VperpSamePhi(bool fgnorm=true) const;
  //! alias for VperpSamePhi() - return a vector along u_theta, assuming the original vector is along u_r 
  inline Vector3d utheta() const { return VperpSamePhi(); }
  // vecteur perpendiculaire de meme theta
  //! return the perpendicular equal norm vector, with equal theta 
  virtual Vector3d VperpSameTheta(bool fgnorm=true) const;
  //! alias for VperpSameTheta() - return a vector along u_phi, assuming the original vector is along u_r 
  inline Vector3d uphi() const { return VperpSameTheta(); }

  //! copy operator (*this) = v
  Vector3d& operator=(const Vector3d& v)
  {  _x=v._x; _y=v._y; _z=v._z;  return *this; }
  //! return the sum of two vectors (*this) + v
  inline Vector3d Add (const Vector3d& v) const 
  { return Vector3d(_x+v._x, _y+v._y, _z+v._z); }
  //! return the difference of two vectors (*this) - v
  inline Vector3d Subtract (const Vector3d& v) const
  { return Vector3d(_x-v._x, _y-v._y, _z-v._z); }
  //! return the rescaled vector (*this) * s
  inline Vector3d Scale(double s) const
  { return Vector3d(_x*s, _y*s, _z*s); }

  //! in-place sum of two vectors (*this) += v
  inline Vector3d& operator+=(const Vector3d& v) 
  { _x+=v._x; _y+=v._y; _z+=v._z; return (*this); }
  //! in-place difference of two vectors (*this) -= v
  inline Vector3d& operator-=(const Vector3d& v) 
  { _x-=v._x; _y-=v._y; _z-=v._z; return (*this); }
  //! in-place rescaling of the vector (*this) *= s
  inline Vector3d& operator*=(double s)
  { _x*=s; _y*=s; _z*=s; return (*this); }

  //! return true if the two vectros are equal 
  bool isEqual(Vector3d const& v)   const;
  
  //! text (ascii) representation of the vector on stream \b os
  virtual std::ostream& Print(std::ostream& os, bool fgnn=true) const;

  // rotations d'Euler
  //! Perform   Euler's rotations 
  virtual Vector3d Euler(double, double, double) const;

  // rotation inverse
  //! perform   inverse Euler rotation 
  virtual Vector3d InvEuler(double, double, double) const;

  // rotation d'angle phi autour d'un axe omega (regle du tire-bouchon)
  //! perform rotation of angle phi around an axis omega (Maxwell's rule) 
  virtual Vector3d Rotate(const Vector3d& omega,double phi) const;

  //! define the tolerance when checking vectors equality or null vector 
  static void setZeroTolerance(double zthr=1.e-9, int prt=0);
  //! return the tolerance value used for checking vectors equality or null vector 
  static inline double getZeroTolerance() { return zerotol_; } 

 protected:
  double _x, _y, _z;              //!< Cartesian coordinate of the vector
  static double zerotol_;    //!<< tolerance when checking angles equality 

};

//! operator << overloading - Prints the Vector3d object \b v on the stream \b s - no endl added
inline std::ostream& operator<<(std::ostream& s, const Vector3d& v) 
{   return v.Print(s, false); }

//! left multiplication by a scalar (scaling of the vector) 
inline Vector3d operator*(double s, const Vector3d& v) 
{   return v.Scale(s); } 
//! right multiplication by a scalar (scaling of the vector) 
inline Vector3d operator*(const Vector3d& v, double s) 
{   return v.Scale(s); } 

//! addition (sum) operator  res = v1+v2  
inline Vector3d operator+(const Vector3d& v1, const Vector3d& v2) 
{  return v1.Add(v2);  }
//! difference (subtraction) operator  res = v1-v2  
inline Vector3d operator-(const Vector3d& v1, const Vector3d& v2) 
{  return v1.Subtract(v2);  }

//! dot product (scalar product) operator definition 
inline double operator*(const Vector3d& v1, const Vector3d& v2) 
{  return v1.dot(v2); }
//!  vector product operator definition
inline Vector3d operator^(const Vector3d& v1, const Vector3d& v2) 
{  return v1.vecprod(v2); }

//! Equality operator (==) definition for two Vector3d objects   
inline bool operator == (Vector3d const & a, Vector3d const & b)
{ return a.isEqual(b); }

//! Non-Equality operator (!=) definition for two Vector3d objects   
inline bool operator != (Vector3d const & a, Vector3d const & b)
{ return !(a.isEqual(b)); }


//! vector in 3D in spherical coordinate 
class SphVector3D 
{
public:
  //! constructor, with specification of theta,phi spherical angles, and optional vector length 
  explicit SphVector3D(double theta, double phi, double norm=1.);
  //! constructor from a Vector3d (in cartesian coordinates) 
  explicit SphVector3D(const Vector3d &v);
  //! return the theta angle (angle between the vector and Oz axis)
  inline double Theta() const { return _theta; }
  //! return the phi angle (angle between the vector projection on xOy plane and Ox axis)
  inline double Phi() const { return _phi; }
  //! return the vector norm (length)
  inline double Norm() const { return _norm; }
  //! text (ascii) representation of the vector on stream \b os
  virtual std::ostream& Print(std::ostream& os, bool fgnn=true) const;

protected:  
  double _norm,_theta,_phi;
};

//! operator << overloading - Prints the SphVector3D object \b v on the stream \b s - no endl added
inline std::ostream& operator<<(std::ostream& s, const SphVector3D& v) 
{   return v.Print(s, false); }

} // namespace SOPHYA

#endif


