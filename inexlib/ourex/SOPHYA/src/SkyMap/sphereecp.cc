/* ------------------------------------------------------------------------------------
   SOPHYA class library - (C)  UPS+LAL IN2P3/CNRS  , (C) CEA-Irfu-SPP 
   Spherical map in ECP (Equal Cylindrical Projection) pixelisation 
   R.Ansari Sept 2004
   Updated: 2015 
   ------------------------------------------------------------------------------------  */

#include "sopnamsp.h"
#include <math.h>

#define SPHEREECP_CC_BFILE  // avoid extern template declarations 
#include "sphereecp.h"

/*!
  \class SOPHYA::SphereECP
  \ingroup SkyMap
  \brief Spherical maps in Equatorial Cylindrical Projection

  Class implementing spherical maps, with a pixelisation
  corresponding to the an Equatorial Cylindrical Projection (ECP).
  Pixel size varie from equator to poles. The sphere is divided 
  into a number of slices along the parallels. Each slice is subdivided
  into the same number of pixels.
  This class provides the possibility to have partial coverage.

  This class can be instanciated for the following data types:
\verbatim
  Integer data types: 
  uint_1 uint_2  int_1 int_2 int_4 int_8 
  float and complex data types  
  r_4  r_8  complex<r_4>  complex<r_8>
  and if support for long double (SO_LDBLE128) enabled 
  r_16  complex<r_16>  
\endverbatim

*/

/*  Decembre 2018 : En effectuant la comparaison SphericalTransformServer<T> (SOPHYA / Samba) et SharpSHTServer<T> (lib-sharp) 
    pour avoir des resultats quasi-identiques, il faut que le premier pixel soit a zero en phi et pas decale d'un 1/2 pixel 
    Introduction de la constante ECPPHIOFF - idem ds  sphecpbase.cc 
    Si on veut avoir le premier pixel de chaque tranche aligne en phi=0, mettre cette valeur a 0. , 0.5 si decale d'un 1/2 pixel 
    Changer ds sphereecp.cc et  sphecpbase.cc  en meme temps */
#define ECPPHIOFF  0.

using namespace std;

namespace SOPHYA {

//! Default constructor
template <class T>
SphereECP<T>::SphereECP()
  : SphECPBase(), _pixels()
{
  _outofmapidx = -99;
  _outofmapnphi = 0;
  _outofmapntet = 0;
  _outofmappix = 0;
  _outofmapval = 0;
}

/*!
  \brief  Constructor with the pixelisation parameter m
  
  Complete coverage.
  The sphere is divided into \b m slices in theta (0..pi). each slice is divided
  into \b 2m pixels (along phi) 
*/
template <class T>
SphereECP<T>::SphereECP(int m)
  : SphECPBase(m), _pixels(2*m,m,0,0,0,true)
{
  _outofmapidx = -99;
  _outofmapnphi = 0;
  _outofmapntet = 0;
  _outofmappix = 0;
  _outofmapval = 0;
}

/*! 
  Complete coverage.
  \b ntet slices in theta (0..pi) and \b nphi pixels (along phi) for 
  each slice in theta
*/
template <class T>
SphereECP<T>::SphereECP(int ntet, int nphi)
  : SphECPBase(ntet, nphi), _pixels(nphi,ntet,0,0,0,true)
{
  _outofmapidx = -99;
  _outofmapnphi = 0;
  _outofmapntet = 0;
  _outofmappix = 0;
  _outofmapval = 0;
}

/*! 
  Partial coverage : 
  \b ntet slices in theta , in the range (tet1 .. tet2) and \b nphi pixels (along phi) for 
  each slice in theta in the range (phi1 .. phi2) 
*/
template <class T>
SphereECP<T>::SphereECP(r_8 tet1, r_8 tet2, int ntet, r_8 phi1, r_8 phi2, int nphi)
  : SphECPBase(tet1,tet2,ntet,phi1,phi2,nphi), _pixels(nphi,ntet,0,0,0,true)
{
  _outofmapidx = _pixels.Size()+659;
  // Reza, 23 sep 2004 
  // pour des cartes partielles, et des pixels sur la sphere, mais 
  // en dehors de la couverture, on code le numero de tranche en theta, 
  // compte a partir de theta=0,phi=0 +  _outofmapidx
  // theta -> idx =  _outofmapidx + theta/_dtheta*_outofmapnphi +  _outofmapntet
  _outofmapntet = 0;
  _outofmappix = 0;
  _outofmapval = 0;
  _outofmapntet = M_PI/_dtheta;
  _outofmapnphi = 2*M_PI/_dphi;
}

//! Copy constructor - shares the pixel data if \b share=true
template <class T>
SphereECP<T>::SphereECP(const SphereECP<T>& a, bool share)
  : SphECPBase(), _pixels(a._pixels, share)
{
  CopyGeometry(a);
}

//! Copy constructor - shares the pixel data 
template <class T>
SphereECP<T>::SphereECP(const SphereECP<T>& a)
  : SphECPBase(), _pixels(a._pixels)
{
  CopyGeometry(a);
}

template <class T>
SphereECP<T>::~SphereECP()
{
}


/*! 
  \brief Clone geometry and data using the object \b a
*/
template <class T>
void SphereECP<T>::Clone(const SphereECP<T>& a)
{
  CopyGeometry(a);
  _pixels.Clone(a._pixels);
}

//! Clone if \b the \b a map pixel array is not temporary, share otherwise 
template <class T>
void SphereECP<T>::CloneOrShare(const SphereECP<T>& a)
{
  CopyGeometry(a);
  _pixels.CloneOrShare(a._pixels);
}

//! Share the map pixels data array with \b a
template <class T>
void SphereECP<T>::Share(const SphereECP<T>& a)
{
  CopyGeometry(a);
  _pixels.Share(a._pixels);
}

/*!
  If the spherical map is already allocated, CheckSameGeometry() is 
  called for checking that the two spherical maps have the same 
  pixelisation scheme and if yes, the pixel content is copied from \b a.
  For non allocated maps,  CloneOrShare() is called. 
*/

template <class T>
SphereECP<T>&  SphereECP<T>::Set(const SphereECP<T>& a)
{
  if (this == &a) return(*this);
  if (a._pixels.NbDimensions() < 1) 
    throw NotAllocatedError("SphereECP<T>::Set(const SphereECP<T>& ) not allocated a map");
  if (_pixels.NbDimensions() < 1)  CloneOrShare(a);
  if (!CheckSameGeometry(a))  throw SzMismatchError("SphereECP<T>::Set(const SphereECP<T>& a) : not same pixelisation scheme");
  _pixels.Set(a._pixels); 
  CopyGeometry(a);
  return *this ;
}

/*!
  \brief Extract a partial map from a full ECP skymap
  Theta and Phi limits are adjusted automatically to correspond to the source map pixel boundaries
*/
template <class T>
SphereECP<T> SphereECP<T>::ExtractPartial(r_8 tet1, r_8 tet2, r_8 phi1, r_8 phi2)
{
  if (IsPartial())      
    throw  ParmError("SphereECP::ExtractPartial() source map NOT a full sky map");
  if( (tet1 > M_PI) || (tet1 < 0.) || (tet2 > M_PI) || (tet2 < 0.) || 
      (phi1 < 0.) || (phi1 > 2*M_PI) || (phi1 < 0.) || (phi1 > 2*M_PI) ||
      (tet2 <= tet1) || (phi2 <= phi1) )
    throw  ParmError("SphereECP::ExtractPartial() Bad range for theta/phi limits");

// correction/calcul des limites en theta et nombre de rangees en theta  
  sa_size_t ntet = (sa_size_t)((tet2-tet1)/_dtheta + 0.5);
  sa_size_t offt = (sa_size_t)(tet1/_dtheta + 1e-3);
  tet1 = _dtheta*offt;
  tet2 = tet1+ntet*_dtheta;
// correction/calcul des limites en phi et nombre de colonnes en phi
  sa_size_t nphi = (sa_size_t)((phi2-phi1)/_dphi + 0.5);
  sa_size_t offp = (sa_size_t)(phi1/_dphi + 1e-3);
  phi1 = _dphi*offp;
  phi2 = phi1+nphi*_dphi;

// On cree une carte partielle avec les bons parametres
  SphereECP<T> pmap(tet1, tet2, ntet, phi1, phi2, nphi);
// On recopie les valeurs de pixels de la zone correspondante
  pmap.GetPixelArray() = _pixels.SubArray(Range(offp, 0, nphi, 0), Range(offt, 0, ntet, 0),
	                                  Range::first(), Range::first(), Range::first());
  return pmap;
}

template <class T>
string SphereECP<T>::TypeOfMap() const
{
  return string("ECP");
}

template <class T>
int_4 SphereECP<T>::NbPixels() const
{
  return _npix;
}

template <class T>
T& SphereECP<T>::PixVal(int_4 k)
{
  // On pourrait etre plus strict en demandant aussi k<_outofmapidx+_outofmapnphi*_outofmapntet (Reza 23/09/04)
  if ((_partial) && (k>=_outofmapidx))   return _outofmappix;
  if((k < 0) || (k >= _pixels.Size()) )  {
    //DBG    cout << "*DBG* SphereECP<T>::PixVal(k="<<k<<") size()="<<size()<<" outomidx="<<_outofmapidx<<endl;
    throw RangeCheckError("SphereECP::PixVal() Pixel index out of range");
  }
  return _pixels.DataBlock()(k);
}

template <class T>
T  SphereECP<T>::PixVal(int_4 k)  const
{
  // On pourrait etre plus strict en demandant aussi k<_outofmapidx+_outofmapnphi*_outofmapntet (Reza 23/09/04)
  if ((_partial) && (k>=_outofmapidx))   return _outofmapval;
  if((k < 0) || (k >= _pixels.Size()) )  {
    //DBG    cout << "*DBG* SphereECP<T>::PixVal(k="<<k<<") const size()="<<size()<<" outomidx="<<_outofmapidx<<endl;
     throw RangeCheckError("SphereECP::PixVal() Pixel index out of range");
  }
  //  return _pixels.DataBlock()(k);
  return _pixels.DataBlock()(k);
}

template <class T>
bool  SphereECP<T>::ContainsSph(double theta, double phi) const
{
  if (_partial) {
    if ( (theta >= _theta1) && (theta <= _theta2) &&
	 (phi >= _phi1) && (phi <= _phi2) )  return true;
    else return false;
  }
  else return false;
}

template <class T>
int_4  SphereECP<T>::PixIndexSph(double theta, double phi) const
{
  if (_partial) {
    if((theta > M_PI) || (theta < 0.)) return(-1);
    if((phi < 0.) || (phi > 2*M_PI)) return(-1);
    if (_partial) {
      if ( (theta < _theta1) || (theta > _theta2) ||
	   (phi < _phi1) || (phi > _phi2) )  return _outofmapidx+theta/_dtheta*_outofmapnphi+phi/_dphi;
    }
    int_4 it = (theta-_theta1)/_dtheta;
    int_4 jp = (phi-_phi1)/_dphi;
    return (it*_pixels.SizeX()+jp);
  }
  else return getIndex(LongitudeLatitude(theta,phi));
}

template <class T>
void  SphereECP<T>::PixThetaPhi(int_4 k, double& theta, double& phi) const
{
  if ((_partial) && (k>=_outofmapidx)) {
    theta = (((k-_outofmapidx) / _outofmapnphi)+0.5)*_dtheta;
    phi = (((k-_outofmapidx) % _outofmapnphi)+ECPPHIOFF)*_dphi;;
    return;
  }
  LongitudeLatitude ll=getLongLat(k);
  theta=ll.Theta(); 
  phi=ll.Phi();
  return;
}

template <class T>
double  SphereECP<T>::PixSolAngle(int_4 k) const
{
  if ((_partial) && (k>=_outofmapidx)) {
    //DBG    cout << "*DBG*SphereECP<T>::PixSolAngle(k="<<k<<") partial - _nphi="<<_nphi<<" _outofmapnphi="<<_outofmapnphi<<endl;
    double theta = (((k-_outofmapidx) / _outofmapnphi)+0.5)*_dtheta;
    return (_dtheta*_dphi*sin(theta));
  }
  else return getPixSolAngle(k);
  /*
  if((k < 0) || (k >= _pixels.Size()) )  
     throw RangeCheckError("SphereECP::PixSolAngle(int_4 k) Pixel index out of range");

  int_4 it = k / _pixels.SizeX();
  double theta = _theta1+it*_dtheta;
  return (_dtheta*_dphi*sin(theta));
  */
}

template <class T>
int_4  SphereECP<T>::SizeIndex() const
{
  return _ntheta; //  CHECK - Reza Juin 2015 
}

template <class T>
void  SphereECP<T>::Resize(int_4 m)
{
  if ( (m <= 0) || ( m == _pixels.SizeY()) ) {
    if (SphericalMap_GetGlobalPrintLevel()>0)  
      cout << " SphereECP<T>::Resize(int_4 " << m << ") m<0 ou m=NTheta - Ne fait rien " << endl;
    return;
  }
  int mphi = 2*m;
  if (_partial && (_pixels.Size()>0))  mphi = m*_pixels.SizeX()/_pixels.SizeY();
  if (SphericalMap_GetGlobalPrintLevel()>0) 
    cout << " SphereECP<T>::Resize(" << m 
	 << ") -> _pixels.Resize(" << mphi << "," << m << ")" << endl; 
  sa_size_t sz[5] = {0,0,0,0,0};
  sz[0] = mphi;  sz[1] = m;
  _pixels.ReSize(2,sz);
  _outofmapidx = _pixels.Size()+659;
  _dtheta = (_theta2-_theta1)/(double)_ntheta;
  _dphi = (_phi2-_phi1)/(double)_nphi;
  if ((_dtheta<1.e-12)||(_dphi<1.e-12))   
    throw ParmError("SphereECP<T>::Resize(m) negative or small dTheta or dPhi (<1.e-12) !");
  _outofmapntet = M_PI/_dtheta;
  _outofmapnphi = 2*M_PI/_dphi;
  return;
}

template <class T>
uint_4  SphereECP<T>::NbThetaSlices() const
{
  return _ntheta;
}

template <class T>
r_8  SphereECP<T>::ThetaOfSlice(int_4 index) const
{
  if( (index < 0) || (index >= _ntheta) )  
     throw RangeCheckError("SphereECP::ThetaOfSlice() theta index out of range");
  return (_theta1 + ((double)index+0.5)*_dtheta);
 
}

template <class T>
void  SphereECP<T>::GetThetaSliceInfo(int_4 index,r_8& theta, r_8& phi0, int_4& nphi, T* & ptr)
{
  if( (index < 0) || (index >= _ntheta) )  
    throw RangeCheckError("SphereECP::GetThetaSliceInfo() theta index out of range");
  theta=_theta1+((double)index+0.5)*_dtheta;
  phi0=_phi1+ECPPHIOFF*_dphi;
  nphi=_nphi;
  ptr=&(_pixels(0,index,0,0));
}

template <class T>
void  SphereECP<T>::GetThetaSlice(int_4 index,r_8& theta, 
				  TVector<r_8>& phi, TVector<T>& value) const 
{
  if( (index < 0) || (index >= _ntheta) )  
     throw RangeCheckError("SphereECP::GetThetaSlice() index out of range");

  theta = _theta1 + ((double)index+0.5)*_dtheta;
  int_4 nphit = _nphi; 
  if (_partial) nphit=2.*M_PI/_dphi;  
  phi.ReSize(nphit); 
  value.ReSize(nphit); 
  int_4 ioff = _phi1/_dphi;
  int_4 i;
  for(i=0; i<ioff; i++) {
    phi(i) = _phi1 + ((double)(i-ioff)+ECPPHIOFF)*_dphi;
    value(i) = _outofmapval;
  }
  for(i=ioff; i<ioff+_pixels.SizeX(); i++) {
    phi(i) = _phi1 + ((double)(i-ioff)+ECPPHIOFF)*_dphi;
    value(i) = _pixels(i-ioff,index,0);
  }
  for(i=ioff+_pixels.SizeX(); i<nphit; i++) {
    phi(i) = _phi1 + ((double)(i-ioff)+ECPPHIOFF)*_dphi;
    value(i) = _outofmapval;
  }
  return;
}

template <class T>
void  SphereECP<T>::GetThetaSlice(int_4 index, r_8& theta, r_8& phi0, 
				  TVector<int_4>& pixidx, TVector<T>& value) const
{
  if( (index < 0) || (index >= _ntheta) )  
    throw RangeCheckError("SphereECP::GetThetaSlice() index out of range");
  
  theta = _theta1 + ((double)index+0.5)*_dtheta;
  int_4 ioff = _phi1/_dphi;
  phi0 = _phi1 - ((double)ioff-ECPPHIOFF)*_dphi;

  int_4 nphit = _nphi; 
  if (_partial) nphit=2.*M_PI/_dphi;  
  //  cout << " *DBG*SphereECP<T>::GetThetaSlice() - nphit="<<nphit<<endl;
  pixidx.ReSize(nphit); 
  value.ReSize(nphit);
 
  int_4 i;
  for(i=0; i<ioff; i++) {
    pixidx(i) = _outofmapidx+theta/_dtheta*_outofmapnphi+phi0/_dphi+i;
    value(i) = _outofmapval;
  }
  for(i=ioff; i<ioff+_pixels.SizeX(); i++) {
    pixidx(i) = index*_pixels.SizeX()+(i-ioff);
    value(i) = _pixels(i-ioff,index,0);
  }
  for(i=ioff+_pixels.SizeX(); i<nphit; i++) {
    pixidx(i) = _outofmapidx+theta/_dtheta*_outofmapnphi+phi0/_dphi+i;
    value(i) = _outofmapval;
  }
  return;

}

template <class T>
T*  SphereECP<T>::GetThetaSliceDataPtr(int_4 index)
{
  if( (index < 0) || (index >= _pixels.SizeY()) )  
    throw RangeCheckError("SphereECP::GetThetaSliceDataPtr() index out of range");
  int_4 ioff = _phi1/_dphi;
  int_4 nphit = _nphi;
  if (_partial) nphit=2.*M_PI/_dphi;  
// Correction bug signale par cmv le 27/08/2008, debordement memoire lors
// de synthese par SphericalTransformServer ...
  if ( (ioff != 0) || (nphit!=_pixels.SizeX()) ) return NULL;
  return _pixels.Data() + index*_pixels.SizeX();
}

template <class T>
void  SphereECP<T>::Show(ostream& os) const
{
  SphericalMap<T>::Show(os);
  if (_partial)
    os << "SphereECP<T>::Print() - Partial ECP Map NPhi=" << _pixels.SizeX() 
       << " x NTheta= " <<  _pixels.SizeY() << " SolidAngle=" << PixSolAngle() << "\n"
       << "ThetaLimits= " << _theta1 << " .. " << _theta2 
       << "  PhiLimits= " << _phi1 << " .. " << _phi2 << endl;
  else 
    os << "SphereECP<T>::Print() - Full ECP Map NPhi=" << _pixels.SizeX() 
       << " x NTheta= " <<  _pixels.SizeY() << " SolidAngle=" << PixSolAngle() << endl;
}

template <class T>
void  SphereECP<T>::Print(ostream& os) const
{
  Show(os);
  os << _pixels;
}


template <class T>
SphereECP<T>&  SphereECP<T>::SetCst(T a)
{
  if (_pixels.NbDimensions() < 1) 
    throw NotAllocatedError("SphereECP<T>::SetCst() not allocated map");
  _pixels.SetCst(a);
  return *this ;
}

template <class T>
SphereECP<T>&  SphereECP<T>::AddCst(T a)
{
  if (_pixels.NbDimensions() < 1) 
    throw NotAllocatedError("SphereECP<T>::AddCst(T a) not allocated map");
  _pixels.Add(a);
  return *this ;
}

template <class T>
SphereECP<T>&  SphereECP<T>::SubCst(T a, bool fginv)
{
  if (_pixels.NbDimensions() < 1) 
    throw NotAllocatedError("SphereECP<T>::SubCst(T a, bool) not allocated map");
  _pixels.Sub(a, fginv);
  return *this ;
}

template <class T>
SphereECP<T>&  SphereECP<T>::MulCst(T a)
{
  if (_pixels.NbDimensions() < 1) 
    throw NotAllocatedError("SphereECP<T>::MulCst(T a) not allocated map");
  _pixels.Mul(a);
  return *this ;
}

template <class T>
SphereECP<T>&  SphereECP<T>::DivCst(T a, bool fginv)
{
  if (_pixels.NbDimensions() < 1) 
    throw NotAllocatedError("SphereECP<T>::Div(T a) not allocated map");
  _pixels.Div(a, fginv);
  return *this ;
}

template <class T>
SphereECP<T>&  SphereECP<T>::AddElt(const SphereECP<T>& a)
{
  if (!CheckSameGeometry(a))  
    throw SzMismatchError("SphereECP<T>::AddElt(const SphereECP<T>& a) : not same pixelisation scheme");
  _pixels += a._pixels;
  return *this ;
}

template <class T>
SphereECP<T>&  SphereECP<T>::SubElt(const SphereECP<T>& a)
{
  if (!CheckSameGeometry(a))  
    throw SzMismatchError("SphereECP<T>::SubElt(const SphereECP<T>& a) : not same pixelisation scheme");
  _pixels -= a._pixels;
  return *this ;
}

template <class T>
SphereECP<T>&  SphereECP<T>::MulElt(const SphereECP<T>& a)
{
  if (!CheckSameGeometry(a))  
    throw SzMismatchError("SphereECP<T>::MulElt(const SphereECP<T>& a) : not same pixelisation scheme");
  _pixels.Mul(a._pixels);
  return *this ;
}

template <class T>
SphereECP<T>&  SphereECP<T>::DivElt(const SphereECP<T>& a)
{
  if (!CheckSameGeometry(a))  
    throw SzMismatchError("SphereECP<T>::Div(const SphereECP<T>& a) : not same pixelisation scheme");
  _pixels.Div(a._pixels);
  return *this ;
}




#ifdef __CXX_PRAGMA_TEMPLATES__
#pragma define_template SphereECP<int_4>
#pragma define_template SphereECP<r_4>
#pragma define_template SphereECP<r_8>
#pragma define_template SphereECP< complex<r_4> >
#pragma define_template SphereECP< complex<r_8> >
#endif
#if defined(ANSI_TEMPLATES) || defined(GNU_TEMPLATES)
template class SphereECP<uint_1>;
template class SphereECP<uint_2>;
template class SphereECP<int_1>;
template class SphereECP<int_2>;
template class SphereECP<int_4>;
template class SphereECP<int_8>;
template class SphereECP<r_4>;
template class SphereECP<r_8>;
template class SphereECP< complex<r_4> >;
template class SphereECP< complex<r_8> >;
#ifdef SO_LDBLE128
template class SphereECP<r_16>;
template class SphereECP< complex<r_16> >;
#endif
#endif

}// Fin du namespace
