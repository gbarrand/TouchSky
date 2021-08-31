//------------------------------------------------------------------------------------
// SOPHYA class library - (C)  UPS+LAL IN2P3/CNRS  , CEA-Irfu 
// classes to perform rotation in 3D 
//        R. Ansari - April 2015  
//------------------------------------------------------------------------------------

#include "rotation3d.h"
#include "unitvector.h"
#include <iomanip>

using namespace std;

namespace SOPHYA {

/*!
   \class EulerRotation3D
   \ingroup SkyMap
   \brief this class can be used to define a rotation in 3-dimensions following Euler's 
   convention and then applied to Vector3d objects. 

   There are two reference frames, the original frame \b (R) and the rotated frame \b (RR), which correspond 
   to \b R rotated through the three Euler rotations. 

   For definition of Euler angles and other conventions, see for example :  
     http://mathworld.wolfram.com/EulerAngles.html
     https://en.wikipedia.org/wiki/Euler_angles

   Basic usage example :
   \code
   //  rotation of phi=Pi/4. around Oz, theta=Pi/2 around the new Ox, and then psi=0. around the new Oz
   EulerRotation3D euler(Angle::OnePiCst()/4., Angle::OnePiCst()/2., 0.);
   //  compute and print the uz vector after rotation 
   cout << euler.Rotate(UnitVector::uz()) << endl;
   \endcode 

   If we want to define a rotated frame (RR) such that the direction (theta, phi) in the original frame (R) 
   corresponds to the Oz direction in (RR) , with psi=0 :
   \code 
   double theta=Angle::OnePiCst()/5.; 
   double phi=Angle::TwoPiCst()/7.;
   double rphi=phi+Angle::PioTwoCst(); 
   if (rphi>Angle::TwoPiCst()) rphi-=Angle::TwoPiCst();
   double rtet=theta;
   EulerRotation3D euler(rphi, rtet, 0.);
   UnitVector u(LongitudeLatitude(theta,phi));
   cout << " euler.Rotate(u):"<<euler.Rotate(u)<<endl;
   // Printing  EulerRotation3D objects
   EulerRotation3D rot1(0., Angle::PioTwoCst(), 0.);
   cout << rot1;
   EulerRotation3D rot3(Angle::PioTwoCst()/2., Angle::PioTwoCst(), Angle::OnePiCst()/6.);
   rot3.Print(cout, true);
   \endcode

   More generally, and following https://en.wikipedia.org/wiki/Euler_angles , if the rotated frame (RR) is 
   represented by the three vectors \f$ \mathbf{X'}, \mathbf{Y'} , \mathbf{Z'} \f$ with the following cartesian 
   coordinates in the original frame (R) \f$ \mathbf{X}, \mathbf{Y} , \mathbf{Z} \f$:
   \f[
   \mathbf{X'} : (x1, x2, x3)  \hspace{5mm} \mathbf{Y'} : (y1, y2, y3)  \hspace{5mm} \mathbf{Z'} : (z1, z2, z3)
   \f]
   the three Euler rotation \f$ angles (\varphi_r, \theta_r, \psi_r) \f$  will be given by :
   \f[
   \varphi_r = \arccos \left( -z2 / \sqrt{1-z3^2} \right)  \hspace{5mm}
   \theta_r = \arccos ( z3 )  \hspace{5mm}
   \psi_r = \arccos  \left( y3 / \sqrt{1-z3^2} \right)  
   \f]
   If the rotation is simply defined by the direction of \f$ \mathbf{Z'} \f$ corresponding  two angles 
   \f$ (\theta, \varphi) \f$:
   \f[
   z1 = \sin ( \theta ) \cos ( \varphi)  \hspace{5mm}  z2 = \sin ( \theta ) \sin ( \varphi ) \hspace{5mm}
   z3 = \cos ( \theta ) ; y3 = sin(\theta)  \hspace{5mm}   \sin ( \theta ) = \sqrt{1-z3^2} 
   \f]
   the rotation angles can be written as :
   \f[
   \varphi_r = \arccos \left( - \sin (\varphi ) \right) = \varphi + \frac{\pi}{2}   \hspace{5mm}
   \theta_r = \arccos ( \cos ( \theta ) ) = \theta    \hspace{5mm}
   \psi_r = \arccos  \left( \sin (\theta) / \sin (\theta) \right) = 0   \hspace{5mm}
   \f]
*/

/*!
  The default constructor defines two identical frames (R) = (RR)  
  The two rotation matrices are equal to 3x3 Identity matrices 
*/
EulerRotation3D::EulerRotation3D()
{
  // keeping the euler angles   
  phi_=0.;   theta_=0.;   psi_=0.;
  // Two rotations matrices = Identity 
  ax_[0]=1.;  ax_[1]=ax_[2]=0.;
  ay_[1]=1.;  ay_[0]=ay_[2]=0.;
  az_[2]=1.;  az_[0]=az_[1]=0.;
  arx_[0]=1.;  arx_[1]=arx_[2]=0.;
  ary_[1]=1.;  ary_[0]=ary_[2]=0.;
  arz_[2]=1.;  arz_[0]=arz_[1]=0.;
}

/*!
  The rotation is defined by a set of three rotations, first rotate by angle phi around Oz, 
  (Ox becomes Ox'), followed by rotation around the new Ox axis with angle theta, then finaly 
  rotation around the new Oz axis by angle psi 
  \param phi : first rotation around Oz angle 
  \param theta : second rotation angle around the new Ox axis 
  \param psi : third rotation angle around the new Oz axis 
*/

EulerRotation3D::EulerRotation3D(double phi, double theta, double psi)
{
  double cpsi=cos(psi);
  double ctheta=cos(theta);
  double cphi=cos(phi);
  double spsi=sin(psi);
  double stheta=sin(theta);
  double sphi=sin(phi);
  // keeping the euler angles   
  phi_=phi;   theta_=theta;   psi_=psi;
  // Rotation matrix, from (R) to (RR) 
  ax_[0]=(cpsi*cphi-ctheta*sphi*spsi);
  ax_[1]=(cpsi*sphi+ctheta*cphi*spsi);
  ax_[2]=(spsi*stheta);  
  ay_[0]=(-spsi*cphi-ctheta*sphi*cpsi);
  ay_[1]=(-spsi*sphi+ctheta*cphi*cpsi);
  ay_[2]=cpsi*stheta;
  az_[0]=stheta*sphi;
  az_[1]=(-stheta*cphi);
  az_[2]=ctheta;
  // Rotation matrix, from (RR) to (R) 
  arx_[0]=(cpsi*cphi-ctheta*sphi*spsi);
  arx_[1]=-(spsi*cphi+ctheta*sphi*cpsi);
  arx_[2]=(sphi*stheta); 
  ary_[0]=(cpsi*sphi+ctheta*cphi*spsi);
  ary_[1]=(-spsi*sphi+ctheta*cphi*cpsi);
  ary_[2]=(-cphi*stheta);
  arz_[0]=(stheta*spsi);
  arz_[1]=(stheta*cpsi);
  arz_[2]=ctheta;

}

// Copy constructor 
EulerRotation3D::EulerRotation3D(EulerRotation3D const& a)
{
  CopyFrom(a);
}

// Copy operator 
EulerRotation3D& EulerRotation3D::operator = (EulerRotation3D const& a)
{
  CopyFrom(a); 
  return *this; 
}

// fonction de copie 
void EulerRotation3D::CopyFrom(EulerRotation3D const& a)
{
  phi_=a.phi_;  theta_=a.theta_;  psi_=a.psi_; 
  for(int i=0; i<3; i++) {
    ax_[i]=a.ax_[i];  ay_[i]=a.ay_[i];  az_[i]=a.az_[i]; 
    arx_[i]=a.arx_[i];  ary_[i]=a.ary_[i];  arz_[i]=a.arz_[i]; 
  }
}

/*! 
  \brief Direct rotation (R) to (RR) 

  Apply the rotation to the Vector3d \b v defined in \b (R) and return its coordinates in the 
  rotated frame \b (RR) as a Vector3d object.
*/ 
Vector3d EulerRotation3D::Rotate(Vector3d const& v) const
{
  double xnew=ax_[0]*v.X()+ax_[1]*v.Y()+ax_[2]*v.Z();
  double ynew=ay_[0]*v.X()+ay_[1]*v.Y()+ay_[2]*v.Z();
  double znew=az_[0]*v.X()+az_[1]*v.Y()+az_[2]*v.Z();
  return Vector3d(xnew,ynew,znew);
}

/*! 
  \brief Inverse rotation (RR) to (R) 

  Apply the rotation to the Vector3d \b v defined in the rotated frame \b (RR) and return its coordinates in the 
  original frame \b (R) as a Vector3d object.
*/ 
Vector3d EulerRotation3D::RotateBack(Vector3d const& v) const
{
  double xnew=arx_[0]*v.X()+arx_[1]*v.Y()+arx_[2]*v.Z();
  double ynew=ary_[0]*v.X()+ary_[1]*v.Y()+ary_[2]*v.Z();
  double znew=arz_[0]*v.X()+arz_[1]*v.Y()+arz_[2]*v.Z();
  return Vector3d(xnew,ynew,znew);
}
/*!
  \brief text (ascii) representation of the rotation on stream \b os

  \param fge : if true , prints also the (R) base vectrors (ux,uy,uz) vectors in the rotated frame (RR) , 
   as well the (RR) base vectors (ux',uy',uz') in the original frame (R).
*/
std::ostream& EulerRotation3D::Print(std::ostream& os, bool fgprb)  const
{
  os << " --- EulerRotation3D(phi="<<phi_<<" ,theta="<<theta_<<" ,psi="<<psi_<<") A(R->RR)  Ainv(RR->R)"<<endl;
  os<<std::fixed;  // or std::scientific 
  os << "   |"<<setw(10)<<ax_[0]<<" ,"<<setw(10)<<ax_[1]<<" ,"<<setw(10)<<ax_[2]
     <<" |       |"<<setw(10)<<arx_[0]<<" , "<<setw(10)<<arx_[1]<<" , "<<setw(10)<<arx_[2]<<" |"<<endl;
  os << "A= |"<<setw(10)<<ay_[0]<<" ,"<<setw(10)<<ay_[1]<<" ,"<<setw(10)<<ay_[2]
     <<" | Ainv= |"<<setw(10)<<ary_[0]<<" , "<<setw(10)<<ary_[1]<<" , "<<setw(10)<<ary_[2]<<" |"<<endl;
  os << "   |"<<setw(10)<<az_[0]<<" ,"<<setw(10)<<az_[1]<<" ,"<<setw(10)<<az_[2]
     <<" |       |"<<setw(10)<<arz_[0]<<" , "<<setw(10)<<arz_[1]<<" , "<<setw(10)<<arz_[2]<<" |"<<endl;
  os.unsetf(ios_base::floatfield);
  if (fgprb) {
    os << "u_x: (R) ---> (RR) :"<<Rotate(UnitVector::ux())<<" (RR) ---> (R)"<<RotateBack(UnitVector::ux())<<endl;
    os << "u_y: (R) ---> (RR) :"<<Rotate(UnitVector::uy())<<" (RR) ---> (R)"<<RotateBack(UnitVector::uy())<<endl;
    os << "u_z: (R) ---> (RR) :"<<Rotate(UnitVector::uz())<<" (RR) ---> (R)"<<RotateBack(UnitVector::uz())<<endl;
  }
  return os;
}

//----------------------------------------------------------
// Classe pour la gestion de persistance de EulerRotation3D
// ObjFileIO<EulerRotation3D>
//----------------------------------------------------------

/* --Methode-- */
DECL_TEMP_SPEC  /* equivalent a template <> , pour SGI-CC en particulier */
void        ObjFileIO<EulerRotation3D>::WriteSelf(POutPersist& s) const
{
  if (dobj == NULL)
    throw NullPtrError("ObjFileIO<EulerRotation3D>::WriteSelf() dobj=NULL");
  int_4 ver = 1;
  s.Put(ver);   // ecriture numero de version PPF
  s << dobj->phi_ << dobj->theta_ << dobj->psi_;
  s.PutR8s(dobj->ax_,3);
  s.PutR8s(dobj->ay_,3);
  s.PutR8s(dobj->az_,3);
  s.PutR8s(dobj->arx_,3);
  s.PutR8s(dobj->ary_,3);
  s.PutR8s(dobj->arz_,3);

  return;
}

/* --Methode-- */
DECL_TEMP_SPEC  /* equivalent a template <> , pour SGI-CC en particulier */
void        ObjFileIO<EulerRotation3D>::ReadSelf(PInPersist& s)
{
  int_4 ver;
  s.Get(ver);   // Lecture numero de version PPF

  if (dobj == NULL) dobj = new EulerRotation3D();
  s >> dobj->phi_ >> dobj->theta_ >> dobj->psi_;
  s.GetR8s(dobj->ax_,3);
  s.GetR8s(dobj->ay_,3);
  s.GetR8s(dobj->az_,3);
  s.GetR8s(dobj->arx_,3);
  s.GetR8s(dobj->ary_,3);
  s.GetR8s(dobj->arz_,3);

  return;
}


} // FIN namespace SOPHYA 
