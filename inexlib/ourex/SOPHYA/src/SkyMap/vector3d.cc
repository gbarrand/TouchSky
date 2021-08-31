//   3-D Geometry - classes Vector3d SphVector3d 
//      B. Revenu , G. Le Meur  2000
//      R. Ansari 2006, 2015 
// LAL (Orsay) / IN2P3-CNRS  IRFU/SPP (CEA)

#include "machdefs.h"
#include <math.h>
#include "pexceptions.h"
#include "vector3d.h"

/*
  ------------------------------------------------------------------------------------
  SOPHYA class library - (C)  UPS+LAL IN2P3/CNRS  , (C) CEA-Irfu-SPP 
  3-D Geometry  : Vector3d class 
  B. Revenu, G. Le Meur   2000
  Updated R. Ansari , Jan 2015 
  ------------------------------------------------------------------------------------
*/

using namespace std;

namespace SOPHYA {

/*!
   \class Vector3d
   \ingroup SkyMap
   \brief this class represents a vector in 3 dimensions

   The vector is internally represented with cartesian coordinates and 
   the class provides basic vector computations

   All computations are made with angles in radians and with spherical
   coordinates (theta, phi).
   Concerning Euler's angles, the reference is :
   "Classical Mechanics" 2nd edition, H. Goldstein, Addison Wesley
   see also http://mathworld.wolfram.com/EulerAngles.html

   \sa SOPHYA::LongitudeLatitude  
   \sa SOPHYA::UnitVector 
   \sa SOPHYA::EulerRotation3D 

   \code
   // Create two arbitrary vectors
   Vector3d v1(1.,2.,3.);
   Vector3d v2(3.,4.,5.);
   // print the vectors and their norms, as well 
   cout << "v1: "<<v1<<" v2: "<<v2<<endl;
   cout << "v1.Norm()= "<<v1.Norm()<<" v2.Norm()= "<<v2.Norm()<<endl;
   // compute the scalar product and angles between vectors 
   cout << "v1.v2= "<<v1*v2<<" angle(v1,v2)= "<<v1.SepAngle(v2)<<endl;
   // compute a linear combination of the two vectors
   Vector3d v3 = 5.*v1+v2*3.;
   // normalize the two vectors and compute the same linear combination
   v1.NormalizeSelf();  v2.NormalizeSelf();
   Vector3d v3u = 5.*v1+v2*3.;
   cout << "v3: "<<v3<<" v3u: "<<v3u<<endl;
   // compute the vector product of u_x^u_y
   Vector3d uz=Vector3d(1.,0.,0.)^Vector3d(0.,1.,0.);
   cout << "uz=u_x^u_y: "<<uz<<endl;
   \endcode 
*/

double Vector3d::zerotol_ = 1.e-9; 
/*!
  \param zthr : zero threshold for null vectors or vectors equality. if (zthr < 0.) use std::numeric_limits<double>::min()
  \param prt : if > 0 , print the new zero threshold value
*/
void Vector3d::setZeroTolerance(double zthr, int prt)
{
  if (zthr<0.)  {
    if (prt>0)  cout<<"Vector3d::setZeroTolerance() zthr<0. -> using std::numeric_limits<double>::min()"<<endl;
    zthr=std::numeric_limits<double>::min();
  }
  zerotol_=zthr;
  if (prt>0)  cout<<"Vector3d::setZeroTolerance() setting difftol_ to "<<zthr<<endl;  
  return;
}

/*--Methode--*/
Vector3d::Vector3d(const LongitudeLatitude & ll, double norm)
{
  SetThetaPhi(ll.Theta(), ll.Phi(), norm);
}

/*--Methode--*/
Vector3d::Vector3d(const SphVector3D & v)
{
  SetThetaPhi(v.Theta(), v.Phi(), v.Norm());
}


/*!
  \param theta : angle between the vector and the Oz axis (in radian)
  \param phi : angle between the vector projection on the xOy plane and Ox axis (in radian)
  \param norm : vector norm (or length) 

  \warning can throw ParmError exception if theta out of range (<0 or >Pi), or phi out of 
  range (<0 or >2Pi) or if norm is negative 
*/
void Vector3d::SetThetaPhi(double theta,  double phi, double norm)
{
  if( (theta<0.)||(theta>M_PI) ) 
    throw ParmError("Vector3d::SetThetaPhi(theta,phi[,norm]) Out of range theta <0 or >Pi");
  if( (phi<0.)||(phi>2.*M_PI) ) 
    throw ParmError("Vector3d::SetThetaPhi(theta,phi[,norm]) Out of range phi <0 or >2*Pi");
  if (norm<0.)  
    throw ParmError("Vector3d::SetThetaPhi(theta,phi[,norm]) Out of range norm <0 ");
  _z=norm*cos(theta);
  double xyl=norm*sin(theta);
  _x=xyl*cos(phi);
  _y=xyl*sin(phi);
  return;
}

//! Constructor: unit vector with longitude-latitude specification 

//! Normalize the vector (-> unit length) for non zero vectors
Vector3d& Vector3d::NormalizeSelf() 
{
  double norm=this->Norm();
  if( norm > zerotol_ )  (*this)*=(1./norm);
  else throw MathExc("Vector3d::NormalizeSelf() zero norm vector");
  return *this;
} 

//! return the Theta,Phi as a LongitudeLatitude object 
LongitudeLatitude  Vector3d::GetThetaPhi() const
{
  SphVector3D sv(*this);
  return LongitudeLatitude(sv.Theta(), sv.Phi());
}


/*!
  \warning can throw ZeroValExc exception if one the vectors has zero norm
*/
double Vector3d::SepAngle(const Vector3d& v) const 
//    angular gap between 2 vectors in [0,Pi]
{
  double vn=this->Norm()*v.Norm();
  if (vn<zerotol_)  throw ZeroValExc("Vector3d::SepAngle");
  return acos((_x*v._x+_y*v._y+_z*v._z)/vn);
}

/*!
  Return a vector along u_theta, assuming the original vector is along u_r 

  \param fgnorm : if true (default), return a unit length vector, else same norm (length) as the original vector

  \warning can throw ZeroValExc exception if null vector (0-norm) vector
*/
Vector3d Vector3d::VperpSamePhi(bool fgnorm) const 
//    perpendicular vector, with equal phi
{
  double vn=Norm();
  if (vn<zerotol_)  throw ZeroValExc("Vector3d::VperpSamePhi");
  double stet=fabs(_z)/vn;
  double ctet=-sqrt(1-stet*stet);
  //DBG  cout << "*DBG*VperpSamePhi()/utheta() this:"<<(*this)<<"Z()="<<Z()<<" stet="<<stet<<" ctet="<<ctet<<endl;
  if (-ctet<zerotol_)  {
    //DBG    cout << "**DBG**VperpSamePhi()/utheta() - ctet<min()"<<endl;
    if (fgnorm) return Vector3d((stet>0.)?1.:-1.,0.,0.);
    else return Vector3d((stet>0.)?vn:-vn,0.,0.);
  }
  double xyn=_x*_x+_y*_y;
  double cphi=_x/xyn;
  double sphi=_y/xyn;
  if (fgnorm) return Vector3d(cphi*stet,sphi*stet,ctet);
  return Vector3d(cphi*stet*vn,sphi*stet*vn,ctet*vn);
}

/*!
  \param fgnorm : if true (default), return a unit length vector, else same norm (length) as the original vector

  \warning can throw ZeroValExc exception if null vector (0-norm) vector
*/
Vector3d Vector3d::VperpSameTheta(bool fgnorm) const 
//    perpendicular vector with equal theta 
{ 
  double vn=Norm();
  if (vn<zerotol_)  throw ZeroValExc("Vector3d::VperpSameTheta");
  double ctet=fabs(Z())/vn;
  double stet=sqrt(1-ctet*ctet);
  //DBG cout << "*DBG*VperpSameTheta()/uphi() this:"<<(*this)<<" stet="<<stet<<" ctet="<<ctet<<endl;
  if (stet<zerotol_)  {
    if (fgnorm) return Vector3d(0.,(ctet>0.)?1.:-1.,0.);
    else return Vector3d(0.,(ctet>0.)?vn:-vn,0.);
  }
  double xyn=_x*_x+_y*_y;
  double sphi=_x/xyn;
  double cphi=-_y/xyn;
  if (fgnorm) return Vector3d(cphi*stet,sphi*stet,ctet);
  return Vector3d(cphi*stet*vn,sphi*stet*vn,ctet*vn);
}

bool Vector3d::isEqual(Vector3d const& v)   const
{ 
  double dx=_x-v._x;   double dy=_y-v._y;   double dz=_z-v._z; 
  if ((dx>zerotol_)||(-dx>zerotol_)||(dy>zerotol_)||(-dy>zerotol_)||
      (dz>zerotol_)||(-dz>zerotol_))  return false;
  return true;
}


/*!
  The rotation is defined by a set of three rotations, first rotate by angle phi around Oz, 
  (Ox becomes Ox'), followed by rotation around the new Ox axis with angle theta, then finaly 
  rotation around the new Oz axis by angle psi 
  \param phi : first rotation around Oz angle 
  \param theta : second rotation angle around the new Ox axis 
  \param psi : third rotation angle around the new Oz axis 
*/
Vector3d Vector3d::Euler(double phi, double theta, double psi) const 
//
//    Euler's rotations
//-- 
{
  double cpsi=cos(psi);
  double ctheta=cos(theta);
  double cphi=cos(phi);
  double spsi=sin(psi);
  double stheta=sin(theta);
  double sphi=sin(phi);
  double xnew=(cpsi*cphi-ctheta*sphi*spsi)*_x
    +(cpsi*sphi+ctheta*cphi*spsi)*_y
    +spsi*stheta*_z;
  double ynew=(-spsi*cphi-ctheta*sphi*cpsi)*_x
    +(-spsi*sphi+ctheta*cphi*cpsi)*_y
    +cpsi*stheta*_z;
  double znew=stheta*sphi*_x-stheta*cphi*_y+ctheta*_z;
  return Vector3d(xnew,ynew,znew);
}
//++
Vector3d Vector3d::InvEuler(double phi, double theta, double psi) const 
//
//    inverse rotation
//-- 
{
  double cpsi=cos(psi);
  double ctheta=cos(theta);
  double cphi=cos(phi);
  double spsi=sin(psi);
  double stheta=sin(theta);
  double sphi=sin(phi);
  double xnew=(cpsi*cphi-ctheta*sphi*spsi)*_x
    -(spsi*cphi+ctheta*sphi*cpsi)*_y
    +sphi*stheta*_z;
  double ynew=(cpsi*sphi+ctheta*cphi*spsi)*_x
    +(-spsi*sphi+ctheta*cphi*cpsi)*_y
    -cphi*stheta*_z;
  double znew=stheta*spsi*_x+stheta*cpsi*_y+ctheta*_z;
  return Vector3d(xnew,ynew,znew);
}
//++
Vector3d Vector3d::Rotate(const Vector3d& omega, double phi) const
//
//    rotation of angle phi around an axis omega (Maxwell's rule)
//-- 
{
  Vector3d rotationaxis=omega;
  rotationaxis.Normalize();
  double n=this->Psc(rotationaxis);
  Vector3d myself=*this;
  Vector3d rotate=n*rotationaxis+(myself-n*rotationaxis)*cos(phi)-(myself^rotationaxis)*sin(phi);
  return rotate;
} 

/* --Methode-- */
/*!
  \param os : the output stream 
  \param fgnn : if true, endl is pushed to the stream 
*/
ostream& Vector3d::Print(ostream& os, bool fgnn) const 
{
  os << "Vector3(X=" << _x << ",Y=" << _y << ",Z=" << _z << ")"; 
  if (fgnn) os << endl;
  return os;
}

/*!
   \class SphVector3D
   \ingroup SkyMap
   \brief this class provides the representation of a vector in 3 dimensions in spherical coordinates

   \sa SOPHYA::Vector3d 

   \code 
   Vector3d ux(1.,0.,0.);
   Vector3d va(1.,1.,0.);
   Vector3d vb(1.,1.,1.);
   SphVector3D sux(ux);
   SphVector3D sva(va);
   SphVector3D svb(vb);
   cout << "(1,0,0)->sux: "<<sux<<"\n (1,1,0)->sva: "<<sva<<"\n (1,1,1)->svb: "<<svb<<endl;
   Vector3d vas(sva); 
   Vector3d vbs(svb); 
   Vector3d dva=va-vas;
   Vector3d dvb=vb-vbs;
   cout << " dva: "<<dva<<" dvb: "<<dvb<<endl;
   cout << " va==vas ? "<<(va.isEqual(vas)?"true":"false")
        << " vb==vbs ? "<<(vb.isEqual(vbs)?"true":"false")<<endl;
   \endcode 
*/


/* --Methode-- */
SphVector3D::SphVector3D(double theta, double phi, double norm)
{
  if( (theta<0.)||(theta>M_PI) ) 
    throw ParmError("SphVector3D::SphVector3D(theta,phi[,norm]) Out of range theta <0 or >Pi");
  if( (phi<0.)||(phi>2.*M_PI) ) 
    throw ParmError("SphVector3D::SphVector3D(theta,phi[,norm]) Out of range phi <0 or >2*Pi");
  if (norm<0.)  
    throw ParmError("SphVector3D::SphVector3D(theta,phi[,norm]) Out of range norm <0 ");
  _norm=norm; 
  _theta=theta;
  _phi=phi;
}

/* --Methode-- */
SphVector3D::SphVector3D(const Vector3d &v)
{
  if (v==Vector3d(0.,0.,0.))  {
    _norm=0.;  _theta=0.;  _phi=0.;
  } 
  else {
    _norm=v.Norm();
    _theta=acos(v.Z()/_norm);
    Angle at(_theta);
    if ((at==Angle(0.))||(at==Angle::OnePi()))  _phi=0.;
    else _phi=atan2(v.Y(), v.X());
  }
}

/* --Methode-- */
ostream& SphVector3D::Print(ostream& os, bool fgnn) const 
{
  os << "SphVector3D(Theta=" << _theta << ",Phi=" << _phi << ",norm=" << _norm << ")"; 
  if (fgnn) os << endl;
  return os;
}

} // FIN namespace SOPHYA 
