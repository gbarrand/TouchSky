//------------------------------------------------------------------------------------
// SOPHYA class library - (C)  UPS+LAL IN2P3/CNRS  , CEA-Irfu 
// classes to perform rotation in 3D 
//        R. Ansari - April 2015  
//------------------------------------------------------------------------------------

#ifndef ROTATION3D_H_SEEN
#define ROTATION3D_H_SEEN

#include "vector3d.h"
#include "objfio.h"

namespace SOPHYA { 
//-----------------------------------------------------------
//----------------- class EulerRotation3D -------------------

class EulerRotation3D : public AnyDataObj {
// gestionnaire PPF
  friend class ObjFileIO<EulerRotation3D>;
public:
  //! default constructor 
  explicit EulerRotation3D(); 
  // rotation definition 
  EulerRotation3D(double phi, double theta, double psi);
  //! copy constructor 
  EulerRotation3D(EulerRotation3D const& a);
  //! copy operator 
  EulerRotation3D& operator = (EulerRotation3D const& a);
  //! return the value of the Euler phi angle 
  inline double Phi() const { return phi_; }
  //! return the value of the Euler phi angle 
  inline double Theta() const { return theta_; }
  //! return the value of the Euler psi angle 
  inline double Psi() const { return psi_; }
  // Direct rotation (R) to (RR) : apply rotation to v (defined in R), returning its coordinates in the rotated frame RR
  Vector3d Rotate(Vector3d const& v) const;
  // Inverse rotation (RR) to (R) : apply rotation to v (defined in RR), returning its coordinates in the original frame R
  Vector3d RotateBack(Vector3d const& v) const;
  // print method 
  virtual std::ostream& Print(std::ostream& os, bool fgprb=false) const;

protected:
  //! copy function from a 
  void CopyFrom(EulerRotation3D const& a);

  double phi_, theta_, psi_; 
  //  the two rotation matrices [ ax ; ay ; az ] 
  double ax_[3], ay_[3], az_[3];          //!< 3 rows of the rotation matrix, from R to RR 
  double arx_[3], ary_[3], arz_[3];       //!< 3 rows of the rotation matrix, from RR to R 
};

//! operator << overloading - Prints the Vector3d object \b v on the stream \b s - no endl added
inline std::ostream& operator<<(std::ostream& s, const EulerRotation3D& rot) 
{ return rot.Print(s, false); }

//! Writes the Rotation3D object in the POutPersist stream \b os
inline POutPersist& operator << (POutPersist& os, EulerRotation3D & obj)
{ ObjFileIO<EulerRotation3D> fio(&obj);  fio.Write(os);  return(os); }
//! Reads the Rotation3D object from the PInPersist stream \b is 
inline PInPersist& operator >> (PInPersist& is, EulerRotation3D & obj)
{ ObjFileIO<EulerRotation3D> fio(&obj); is.SkipToNextObject(); fio.Read(is); return(is); }

} // namespace SOPHYA

#endif
