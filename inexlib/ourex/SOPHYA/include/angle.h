//------------------------------------------------------------------------------------
// SOPHYA class library - (C)  UPS+LAL IN2P3/CNRS  , (C) CEA-Irfu-SPP 
// Class Angle - conversion between different angle units 
// R. Ansari, LAL/IN2P3-CNRS, Univ. Paris Sud - 2006 
//------------------------------------------------------------------------------------

#ifndef ANGLE_H_SEEN
#define ANGLE_H_SEEN

#include "machdefs.h"
#include <math.h>
#include <iostream>
#include <limits>       // std::numeric_limits


namespace SOPHYA { 

//! Class to ease angle conversions (radian <> degree <> arcmin <> arcsec)
class Angle {
public:
  enum AngleUnit { Radian, Degree, ArcMin, ArcSec };
  //! Constructor with specification of angle value in radian 
  explicit Angle(double val) { _angrad = val; }
  //! Constructor with specification of angle value and unit
  explicit Angle(double val, Angle::AngleUnit un);
  //! Copy constructor
  Angle(Angle const& a) { _angrad = a._angrad; }

  //! Conversion to radian
  inline double ToRadian() const { return _angrad; }
  //! Conversion to degree
  inline double ToDegree() const { return _angrad*_rad2deg; }
  //! Conversion to arcmin
  inline double ToArcMin() const { return _angrad*_rad2min; }
  //! Conversion to arcsec
  inline double ToArcSec() const { return _angrad*_rad2sec; }

  //! return the angle value in radian
  inline operator double () const { return _angrad; }

  //! return -1 (<) , 0 (==), 1 (>) when comparing the two angles (== within the tolerance getZeroTolerance() )
  inline int Compare(Angle const& a)   const
  { 
    double dt=_angrad-a._angrad;  
    if (dt<-zerotol_) return -1;
    else if (dt>zerotol_)  return 1;
    else return 0;
  }

  //! return the Pi radians (180 deg) angle 
  static inline Angle OnePi() { return Angle(_api); }
  //! return the 2xPi radians (360 deg) angle 
  static inline Angle TwoPi() { return Angle(_a2pi); }
  //! return the Pi/2 radians (90 deg) angle 
  static inline Angle PioTwo() { return Angle(_apio2); }

  //! define the tolerance when checking angles equality 
  static void setZeroTolerance(double zthr=1.e-9, int prt=0);
  //! return the tolerance value used for checking angles equality 
  static inline double getZeroTolerance() { return zerotol_; } 

  //! return the constant _pi ( = Pi ) 
  static inline double OnePiCst() { return _api; }
  //! return the constant _2pi  ( = 2xPi )
  static inline double TwoPiCst() { return _a2pi; }
  //! return the constant _pio2 ( = Pi/2 ) 
  static inline double PioTwoCst() { return _apio2; }

protected:  
  double _angrad;  //!<< angle in radians 

  static double _deg2rad;  //!< deg -> radian conversion factor
  static double _min2rad;  //!< arcmin -> radian conversion factor
  static double _sec2rad;  //!< arcsec -> radian conversion factor
  static double _rad2deg;  //!< rad -> degree conversion factor
  static double _rad2min;  //!< rad -> arcmin conversion factor
  static double _rad2sec;  //!< rad -> arcmin conversion factor
  static double _api;      //!< Pi (180 deg) angle 
  static double _a2pi;     //!< 2xPi (360 deg) angle 
  static double _apio2;    //!< Pi/2 (90 deg) angle 

  static double zerotol_;  //!<< tolerance when comparing angles 
  
};

//! operator << overloading - Prints the Angle object \b ll on the stream \b os
inline std::ostream& operator<<(std::ostream& s, Angle const & a) 
{ s<<"Angle("<<a.ToRadian()<<" rad->"<<a.ToDegree()<<" deg)"; return s;  }

//! Equality operator (==) definition for two Angle objects   
inline bool operator == (Angle const & a, Angle const & b)
{ return (a.Compare(b)==0); }
//! Non-Equality operator (!=) definition for two Angle objects   
inline bool operator != (Angle const & a, Angle const & b)
{ return (a.Compare(b)!=0); }
//! less than comparison operator (<) definition for two Angle objects   
inline bool operator < (Angle const & a, Angle const & b)
{ return (a.Compare(b)==-1); }
//! greater than comparison operator (>) definition for two Angle objects   
inline bool operator > (Angle const & a, Angle const & b)
{ return (a.Compare(b)==1); }
//! less or equal than comparison operator (<=) definition for two Angle objects   
inline bool operator <= (Angle const & a, Angle const & b)
{ return (a.Compare(b)<=0); }
//! greater or equal than comparison operator (>=) definition for two Angle objects   
inline bool operator >= (Angle const & a, Angle const & b)
{ return (a.Compare(b)>=0); }

//! Angle conversion: Radian to degree 
inline double RadianToDegree(double ar) 
{ return Angle(ar).ToDegree(); }
//! Angle conversion: Degree to radian 
inline double DegreeToRadian(double ad) 
{ return Angle(ad,Angle::Degree).ToRadian(); }
//! Angle conversion: Arcminute to radian  
inline double ArcminToRadian(double aam) 
{ return Angle(aam,Angle::ArcMin).ToRadian(); }


} // namespace SOPHYA

#endif


