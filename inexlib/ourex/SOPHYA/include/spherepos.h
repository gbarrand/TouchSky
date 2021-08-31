//------------------------------------------------------------------------------------
//  classes SphereCoordSys , FIO_SphereCoordSys 
//     Definition of a reference coordinate system for spherical maps  
// SOPHYA class library - (C)  UPS+LAL IN2P3/CNRS  , CEA-Irfu 
//        G. Le Meur   2000
//        R. Ansari - updated May 2015 
//------------------------------------------------------------------------------------

#ifndef SPHEREPOS_H_SEEN
#define SPHEREPOS_H_SEEN

#include "machdefs.h"
#include "ppersist.h"
#include <string>
#include "unitvector.h"
#include "rotation3d.h"
#include "wcsdef.h"
#include "anydataobj.h"


namespace SOPHYA {

//................. SphereCoordSys class .................
 
class SphereCoordSys : public  AnyDataObj {
public:

//! Default constructor - optional specification of the coordinate system.
  SphereCoordSys(WCSId wid=WCS_Unknown);
//! copy constructor 
  SphereCoordSys(const SphereCoordSys& a);
//! destructor 
  virtual ~SphereCoordSys() { }

//! copy operator 
  SphereCoordSys& operator = (const SphereCoordSys& a);

//! set or changes the reference coordinate system 
  inline void   setWCSId(WCSId cid) { wcsid_=cid; }
//! return the identification of the reference coordinate system 
  inline WCSId  getWCSId() const {return wcsid_; }

//! set or changes the description associated with the SphereCoordSys object
  inline void setDescription(std::string const& desc)  { description_=desc; }
//! return the description associated with the SphereCoordSys object
  inline std::string getDescription() const {return description_; }

//! defines a rotated coordinate system with respect to the frame defined by WCSId wcsid_
  inline void RotateFrame(EulerRotation3D const& rot)   { fgrotated_=true;  euler_=rot; }
//! return true if the Sphere Coordinate system is rotated with respect to the frame defined by WCSId wcsid_
  inline bool isRotated() const { return fgrotated_; }
//! return the rotation object defining the coordinate system with respect to the frame  defined by WCSId wcsid_
  inline EulerRotation3D getRotation() const { return euler_; }

//! text (ascii) representation of the SphereCoordSys on stream \b os
  virtual std::ostream& Print(std::ostream& os) const;

private:
  WCSId wcsid_;
  std::string description_;
  bool fgrotated_;   //   if true, the system axis is rotated  
  EulerRotation3D euler_;
};

//! operator << overloading - Prints the SphereCoordSys object \b scs on the stream \b s 
inline std::ostream& operator<<(std::ostream& s, const SphereCoordSys& scs) 
{   return scs.Print(s); }

// Classe pour la gestion de persistance

class FIO_SphereCoordSys : public  PPersist  {
public:
  FIO_SphereCoordSys();
  FIO_SphereCoordSys(std::string const & filename);
  FIO_SphereCoordSys(const SphereCoordSys & obj);
  FIO_SphereCoordSys(SphereCoordSys * obj);
  virtual ~FIO_SphereCoordSys();
  virtual AnyDataObj* DataObj();
  virtual   void        SetDataObj(AnyDataObj & o);
  inline operator SphereCoordSys() { return(*dobj); }
protected :
  virtual void       ReadSelf(PInPersist&);
  virtual void       WriteSelf(POutPersist&) const;  
  SphereCoordSys * dobj;
  bool ownobj;
};



} // Fin du namespace

#endif
