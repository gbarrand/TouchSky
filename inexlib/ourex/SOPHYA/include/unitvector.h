//------------------------------------------------------------------------------------
// class UnitVector  
// SOPHYA class library - (C)  UPS+LAL IN2P3/CNRS  , (C) CEA-Irfu-SPP 
//   Utility functions for spherical trigonometry 
// B. Revenu , G. Le Meur    2000  ,  R. Ansari 2006, 2015  
//------------------------------------------------------------------------------------

#ifndef UNITVECTOR_H_SEEN
#define UNITVECTOR_H_SEEN

#include <iostream>
#include <stdio.h>
#include <string.h>
#include "vector3d.h"

namespace SOPHYA { 

//-----------------------------------------------------------------------
//----  Specialisation de la classe Vector3d pour des vecteurs unitaires 
class UnitVector : public Vector3d 
{  
public:  
  //! Default constructor : unit vector in the x direction
  UnitVector() : Vector3d(1.,0.,0.) { }
  //! Constructor: Unit vector along the x,y,z cartesian coordinates
  UnitVector(double x, double y, double z); 
  //! contructor: Unit vector along the direction specified by ll 
  UnitVector(const LongitudeLatitude & ll) : Vector3d(ll,1.)  { }
  //! copy constructor 
  UnitVector(const UnitVector & v)
  { _x=v._x; _y=v._y; _z=v._z; }
  //! constructor from a general Vector3d object 
  UnitVector(const Vector3d&);

  //! desctructor 
  virtual ~UnitVector() { }

  //! copy operator (*this) = v
  UnitVector& operator=(const UnitVector& v)
  {  _x=v._x; _y=v._y; _z=v._z;  return *this; }

  //! text (ascii) representation of the vector on stream \b os  
  virtual std::ostream& Print(std::ostream& os, bool fgnn=true) const;
  
  //! return a unit vector along Ox
  static inline UnitVector ux() { return UnitVector(1.,0.,0.); } 
  //! return a unit vector along Oy
  static inline UnitVector uy() { return UnitVector(0.,1.,0.); } 
  //! return a unit vector along Oz
  static inline UnitVector uz() { return UnitVector(0.,0.,1.); } 
  //! return the unit vector along the radial direction (= (*this))
  inline UnitVector ur() const  { return UnitVector(*this); }
  //! return the unit vector along the theta direction 
  inline UnitVector utheta() const  { return Vector3d::utheta(); }
  //! return the unit vector along the phi direction 
  inline UnitVector uphi() const  { return Vector3d::uphi(); }
};

} // namespace SOPHYA

#endif


