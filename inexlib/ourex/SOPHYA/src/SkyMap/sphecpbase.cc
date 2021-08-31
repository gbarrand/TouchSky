/* ------------------------------------------------------------------------------------
  SOPHYA class library - (C)  UPS+LAL IN2P3/CNRS , (C) CEA-Irfu-SPP
  ECP (Equal Cylindrical Projection) spherical pixelisation base class 
  R. Ansari 2015 
  ------------------------------------------------------------------------------------ */

#include <math.h>

#include "machdefs.h"
#include "pexceptions.h"
#include "sphecpbase.h"

using namespace std;

namespace SOPHYA {

//---------------------------------------------------------------------------------
//------------------------- Implementation de SphECPBase --------------------------
//---------------------------------------------------------------------------------

/*!
   \class SphECPBase
   \ingroup SkyMap 

   \brief base class implementing ECP (Equatorial Cylindrical Projection) pixelisation 

*/

/*  Decembre 2018 : En effectuant la comparaison SphericalTransformServer<T> (SOPHYA / Samba) et SharpSHTServer<T> (lib-sharp) 
    pour avoir des resultats quasi-identiques, il faut que le premier pixel soit a zero en phi et pas decale d'un 1/2 pixel 
    Introduction de la constante ECPPHIOFF - idem ds  sphereecp.cc 
    Si on veut avoir le premier pixel de chaque tranche aligne en phi=0, mettre cette valeur a 0. , 0.5 si decale d'un 1/2 pixel 
    Changer ds sphereecp.cc et  sphecpbase.cc  en meme temps */
#define ECPPHIOFF  0.


/*-- Methode --*/
SphECPBase::SphECPBase() 
  : SphericalPixelisationBase(SPix_ECP),
    _partial(false), _ntheta(0), _nphi(0), _npix(0), _theta1(0.), _theta2(Angle::OnePiCst()), _phi1(0.), _phi2(Angle::TwoPiCst()),
    _dtheta(0.), _dphi(0.)

{
}

/*-- Methode --*/
SphECPBase::SphECPBase(int m) 
  : SphericalPixelisationBase(SPix_ECP),
    _partial(false), _ntheta(m), _nphi(2*m), _npix(2*(size_t)m*(size_t)m), 
    _theta1(0.), _theta2(Angle::OnePiCst()), _phi1(0.), _phi2(Angle::TwoPiCst()),
    _dtheta(Angle::OnePiCst()/(double)m), _dphi(Angle::OnePiCst()/(double)m)

{
  if ((m<3)||(m>1048576))  throw ParmError("SphECPBase(int m) Out of range m value (m<3)||(m>1048576) !");
}

/*-- Methode --*/
SphECPBase::SphECPBase(int ntet, int nphi)
  : SphericalPixelisationBase(SPix_ECP),
    _partial(false), _ntheta(ntet), _nphi(nphi), _npix((size_t)ntet*(size_t)nphi), 
    _theta1(0.), _theta2(Angle::OnePiCst()), _phi1(0.), _phi2(Angle::TwoPiCst()),
    _dtheta(Angle::OnePiCst()/(double)ntet), _dphi(Angle::TwoPiCst()/(double)nphi)
{
  if ((ntet<3)||(ntet>1048576))  throw ParmError("SphECPBase(ntet, nphi) Out of range ntet value (ntet<3)||(ntet>1048576) !");
  if ((nphi<3)||(nphi>2097152))  throw ParmError("SphECPBase(ntet, nphi) Out of range nphi value (nphi<3)||(nphi>2097152) !");
}


/*-- Methode --*/
SphECPBase::SphECPBase(r_8 tet1, r_8 tet2, int ntet, r_8 phi1, r_8 phi2, int nphi) 
  : SphericalPixelisationBase(SPix_ECP),
    _partial(true), _ntheta(ntet), _nphi(nphi), _npix((size_t)ntet*(size_t)nphi), 
    _theta1(tet1), _theta2(tet2), _phi1(phi1), _phi2(phi2)
{
  if ((ntet<3)||(ntet>1048576))  
    throw ParmError("SphECPBase(tet1,tet2,ntet,phi1,phi2,nphi) Out of range ntet value (ntet<3)||(ntet>1048576) !");
  if ((nphi<3)||(nphi>2097152))  
    throw ParmError("SphECPBase(tet1,tet2,ntet,phi1,phi2,nphi) Out of range nphi value (nphi<3)||(nphi>2097152) !");
  if ((tet1>Angle::OnePiCst())||(tet1<0.)||(tet2>Angle::OnePiCst())||(tet2<0.)||(tet2<=tet1)) 
    throw ParmError("SphECPBase(tet1,tet2,ntet,phi1,phi2,nphi) bad theta limits (tet1,tet2) !");
  if ((phi1<0.)||(phi1>Angle::TwoPiCst())||(phi2<0.)||(phi2>Angle::TwoPiCst())||(phi2<=phi1)) 
    throw ParmError("SphECPBase(tet1,tet2,ntet,phi1,phi2,nphi) bad theta limits (tet1,tet2) !");
  _dtheta = (_theta2-_theta1)/(double)_ntheta;
  _dphi = (_phi2-_phi1)/(double)_nphi;
  if ((_dtheta<1.e-12)||(_dphi<1.e-12))   
    throw ParmError("SphECPBase(tet1,tet2,ntet,phi1,phi2,nphi) negative or small dTheta or dPhi (<1.e-12) !");
}

/*-- Methode --*/
void SphECPBase::copyFrom(SphECPBase const& a)
{
  _partial=a._partial;   _ntheta=a._ntheta;  _nphi=a._nphi;   _npix=a._npix;
  _theta1 = a._theta1;   _theta2 = a._theta2;   _phi1 = a._phi1;  _phi2 = a._phi2;   
  _dtheta = a._dtheta;   _dphi = a._dphi;
}

/*-- Methode --*/
bool SphECPBase::CheckSameGeometry(const SphECPBase& a)
{
  if (_partial != a._partial) return false;
  if ((fabs(_theta1-a._theta1)>1.e-39) || (fabs(_theta2-a._theta2)>1.e-39))  return false;
  if ((fabs(_phi1-a._phi1)>1.e-39) || (fabs(_phi2-a._phi2)>1.e-39))  return false;
  if ((fabs(_dtheta-a._dtheta)>1.e-39) || (fabs(_dphi-a._dphi)>1.e-39))  return false;
  if ((_ntheta!=a._ntheta)||(_nphi!=a._nphi))  return false;
  return true;
} 


/*-- Methode --*/
/*!
  can throw ParmError if m is out of range ( 3 <= m <= 1048576 )
  \param m : m is the number of rings (or slices) in theta    
*/
void SphECPBase::setPixelisationParameter(int_4 m)
{
  if ((m<3)||(m>1048576))  
    throw ParmError("SphECPBase::setPixelisationParameter(m) Out of range m value (m<3)||(m>1048576) !");
  _ntheta=m;   _nphi=2*m;
  _dtheta = (_theta2-_theta1)/(double)_ntheta;
  _dphi = (_phi2-_phi1)/(double)_nphi;
  if ((_dtheta<1.e-12)||(_dphi<1.e-12))   
    throw ParmError("SphECPBase::setPixelisationParameter(m) negative or small dTheta or dPhi (<1.e-12) !");
  return;
}

/*-- Methode --*/
int_4 SphECPBase::getPixelisationParameter() const
{
  return _ntheta; 
}

/*-- Methode --*/
size_t SphECPBase::getNbPixels() const
{
  return _npix;
}

/*-- Methode --*/
int_8 SphECPBase::getIndex(LongitudeLatitude const& ll) const
{
  int_4 it = (ll.Theta()-_theta1)/_dtheta;
  int_4 jp = (ll.Phi()-_phi1)/_dphi;
  if ((it<0)||(it>=_ntheta)||(jp<0)||(jp>=_nphi))  
    throw RangeCheckError("SphECPBase::getIndex(ll) - out of range or out of map ll ");
  return ((int_8)it*(int_8)_nphi+(int_8)jp);
}


/*-- Methode --*/
LongitudeLatitude SphECPBase::getLongLat(int_8 idx) const
{
  if ((idx<0)||(idx>=(int_8)_npix))  throw RangeCheckError("SphECPBase::getLongLat(idx) - out of range index idx ");
  int_8 it = idx / _nphi;
  int_8 jp = idx % _nphi;
  return LongitudeLatitude(_theta1+((double)it+0.5)*_dtheta, _phi1+((double)jp+ECPPHIOFF)*_dphi); 
}

/*-- Methode --*/
double SphECPBase::getPixSolAngle(int_8 idx) const
{
  if ((idx<0)||(idx>=(int_8)_npix)||(_npix<1))  throw RangeCheckError("SphECPBase::getLongLat(idx) - out of range index idx or _npix=0");
  int_8 it = idx / _nphi;
  double theta = _theta1+((double)it+0.5)*_dtheta;
  return (_dtheta*_dphi*sin(theta)); 
}

/*-- Methode --*/
Angle SphECPBase::getAngularResolution() const 
{
  return Angle(Angle::TwoPiCst()*(cos(_theta1)-cos(_theta2))/(double)_npix); 
}


/*-- Methode --*/
int_4 SphECPBase::ResolutionToNTheta(double res)
{
  return Angle::OnePiCst()/res;
}

/*-- Methode --*/
double SphECPBase::NThetaToResolution(int_4 ntheta)
{
  return Angle::OnePiCst()/(double)ntheta;
}


} // Fin du namespace
