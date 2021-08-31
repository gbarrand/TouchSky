/* ------------------------------------------------------------------------------------
   SOPHYA class library - (C)  UPS+LAL IN2P3/CNRS  , (C) CEA-Irfu-SPP 
   SphericalMap with Theta-Phi (IGLOO) pixelisation 
   R. Ansari 2000-2015
   Updated: 2015  (modified pixelisation scheme)
   ------------------------------------------------------------------------------------  */

#include "smathconst.h"
#include <complex>
#include "fiondblock.h"
#include <iostream>

#define SPHERETHETAPHI_CC_BFILE  // avoid extern template declarations 
#include "spherethetaphi.h"

using namespace std;

namespace SOPHYA {

//----- global print level for spherical maps -----
static int_4  sphmap_map_prt_lev_ = 0;
// Set global print level 
void SphericalMap_SetGlobalPrintLevel(int_4 lev)
{ sphmap_map_prt_lev_=lev; }
// Get global print level
int_4 SphericalMap_GetGlobalPrintLevel()
{ return sphmap_map_prt_lev_; }
//----------------------------------------------------

/*!
  \class SphereThetaPhi
  \ingroup SkyMap

  \brief Spherical map with equal latitude (iso-theta) rings 

  \sa SphThetaPhiBase

  Class implementing spherical maps, with equal latitude (iso-theta) rings 
  pixelisation scheme - with template data types (double, float, complex, ...)

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

/* ---- old scheme (pre June 2015 
    sphere splitted with respect to theta, phi : each hemisphere is 
    splitted into (m-1) parallels (equator does not enter into account).
    This operation defines m slices, each of which is splitted into 
    equidistant meridians. This splitting is realized in such a way that 
    all pixels have the same area and are as square as possible.

    One begins with the hemisphere with positive z, starting from the pole
    toward the equator. The first pixel is the polar cap ; it is circular
    and centered on theta=0. 
-------------- */

/*!
  \brief Default constructor with the optional specification of pixelisation variant

  \param fgby3 : if true, the number of pixels along each ring would be a multiple of 3 (default)
*/
template <class T>
SphereThetaPhi<T>::SphereThetaPhi(bool fgby3)
  : SphThetaPhiBase(fgby3), pixels_() 
{
}

/*!  
  \brief Constructor with specification of number of rings (or slices) in theta from 0 to Pi.

  \param nring is the number of rings (or slices) in theta (nring >= 3). The north polar cap forms 
  the first ring and the south polar cap the last ring.   
  \param fgby3 : if true, the number of pixels along each ring would be a multiple of 3 (default)

  There is one pixel at the each pole and if the number of rings or slices is odd (nring%2==1) 
  there is one ring (nring/2+1) centered on the equator (theta=pi/2). Otherwise, 
  if nring is even (nring%2==0), then the equator separates the two rings  (nring/2) and (nring/2+1). 
*/
template <class T>
SphereThetaPhi<T>::SphereThetaPhi(int_4 nring, bool fgby3)
  : SphThetaPhiBase(fgby3)
{
  setNbRings(nring);
}

//! Copy constructor: shares the map data (pixels) if share==true, clones (copies) map data otherwise
template <class T>
SphereThetaPhi<T>::SphereThetaPhi(const SphereThetaPhi<T>& s, bool share)
  : SphThetaPhiBase(s.IsRingNPixMultipleOf3()), pixels_(s.pixels_ , share) 
{
  copyFrom(s);
  if (s.mInfo_) this->mInfo_= new DVList(*s.mInfo_);
}

//! Copy constructor (map pixel data would be shared between the two maps)
template <class T>
SphereThetaPhi<T>::SphereThetaPhi(const SphereThetaPhi<T>& s)
  : SphThetaPhiBase(s.IsRingNPixMultipleOf3()), pixels_(s.pixels_) 
{
  copyFrom(s);
  if (s.mInfo_) this->mInfo_= new DVList(*s.mInfo_);
}

//! destructor 
template <class T>
SphereThetaPhi<T>::~SphereThetaPhi()
{
}

/*! \brief define the pixelisation resolution - \b nring rings along theta , from 0 to Pi. 

    (base class SphThetaPhiBase method redefined to ensure map pixel data allocation) */ 
template <class T>
void SphereThetaPhi<T>::setNbRings(int_4 nring)
{
  SphThetaPhiBase::setNbRings(nring);
  pixels_.ReSize(NPix_, true);
} 


/*!  \brief re-pixelize the sphere if (m > 0) - \b m rings (or slices) along along theta , from 0 to Pi. 

  map pixel resolution would be ~180/m degrees , approximate resolution in arcmin given below for few m-values:
  m=128 -> 85' ; m=256 -> 42' ; m=512 -> 21' ; m=1024 -> 10.5' ; m=2048 -> 5' ; m=4096 -> 2.5' ; m=10800 -> 1' 
*/
  
template <class T>
void SphereThetaPhi<T>::Resize(int_4 m)
{
  if ((m <= 0) && (NTheta_ > 0) ) {
    if (SphericalMap_GetGlobalPrintLevel()>0) 
      cout << "SphereThetaPhi<T>::Resize(m) with m<=0 - NOT resized" << endl;
    return;
  }
  setNbRings(m); 
}

//! Clone or share the SphereThetaPhi object \b a
template<class T>
void  SphereThetaPhi<T>::CloneOrShare(const  SphereThetaPhi<T>& a)
{
  copyFrom(a);
  pixels_.CloneOrShare(a.pixels_);
  if (this->mInfo_) {delete this->mInfo_; this->mInfo_ = NULL;}
  if (a.mInfo_) this->mInfo_ = new DVList(*(a.mInfo_));
}
//! Share the pixel data with object \b a
template<class T>
void  SphereThetaPhi<T>::Share(const  SphereThetaPhi<T>& a)
{
  copyFrom(a);
  pixels_.Share(a.pixels_);
  if (this->mInfo_) {delete this->mInfo_; this->mInfo_ = NULL;}
  if (a.mInfo_) this->mInfo_ = new DVList(*(a.mInfo_));
}

////////////////////////// methodes de copie/share
//! Perform data copy or shares the data 
template<class T>
SphereThetaPhi<T>& SphereThetaPhi<T>::Set(const SphereThetaPhi<T>& a)
{
  if (this != &a)    { 
    if (a.NbPixels() < 1) 
      throw NotAllocatedError("SphereThetaPhi<T>::Set(a ) - map a has 0 size !");
    if (NbPixels() < 1) CloneOrShare(a);
    else {
      if (isSamePixelisation(a)) pixels_=a.pixels_;
      else throw SzMismatchError("SphereThetaPhi<T>::Set(a) not same pixelisation parameters !");
    }
    if (this->mInfo_) delete this->mInfo_;
    this->mInfo_ = NULL;
    if (a.mInfo_) this->mInfo_ = new DVList(*(a.mInfo_));
  }
  return(*this);
}


/* --Methode-- */
//!    Return total number of pixels 
template <class T>
int_4 SphereThetaPhi<T>::NbPixels() const
{
  return(NPix_);
}

//!    Return value of pixel with index \b idx
template <class T>
T& SphereThetaPhi<T>::PixVal(int_4 idx)
{
  if((idx < 0) || (idx >= NPix_))  {
      throw RangeCheckError("SphereThetaPhi::PixVal(idx) Pixel index out of range");
  }
  return pixels_(idx);
}

//!    Return value of pixel with index k
template <class T>
T SphereThetaPhi<T>::PixVal(int_4 idx) const
{
  if((idx < 0) || (idx >= NPix_))  {
      throw RangeCheckError("SphereThetaPhi::PixVal(idx) Pixel index out of range");
  }
  return pixels_(idx);
}

//! Return true if teta,phi in map  
template <class T>
bool SphereThetaPhi<T>::ContainsSph(double /*theta*/, double /*phi*/) const
{
  return(true);
}

//!    Return index of the pixel corresponding to direction (theta, phi).
template <class T>
int_4 SphereThetaPhi<T>::PixIndexSph(double theta, double phi) const
{
  return getIndex(LongitudeLatitude(theta, phi));
}

//!   Return (theta,phi) coordinates of the center of the \b idx th pixel of the map 
template <class T>
void SphereThetaPhi<T>::PixThetaPhi(int_4 idx, double& theta, double& phi) const
{
  LongitudeLatitude ll=getLongLat(idx); 
  theta = ll.Theta();   phi = ll.Phi();
}


/*!
  \brief Pixel Solid angle for pixel \b idx in steradians

  \param idx : pixel index in the map 

  if an invalid pixel index (negative or > NbPixels()) is specified, the average pixel solid angle 
  (Omega_ = 4 Pi / NbPixels) is returned. Otherwise, the pixel \b idx coverd solid angle is returned.
*/
template <class T>
double SphereThetaPhi<T>::PixSolAngle(int_4 idx) const

{
  if ((idx<0)||(idx>=NPix_))  return Omega_;
  return getPixSolAngle(idx);
}

template <class T>
uint_4 SphereThetaPhi<T>::NbThetaSlices() const
{
  return getNbRings();
}

template <class T>
r_8 SphereThetaPhi<T>::ThetaOfSlice(int_4 kt) const
{
  return getRingTheta(kt);
}

template <class T>
int_4 SphereThetaPhi<T>::GetSymThetaSliceIndex(int_4 kt) const
{
  if ((NTheta_%2==1)&&(kt==NTheta_/2))  return -1;
  return NTheta_-1-kt;
}

template <class T>
bool SphereThetaPhi<T>::HasSymThetaSlice() const
{
  return true;
}

/*!    
  \brief return a Theta slice information
  For a theta-slice with index 'index', return : 
  the corresponding "theta" 
  a vector containing the phi's of the pixels of the slice
  a vector containing the corresponding values of pixels 
*/
template <class T>
void SphereThetaPhi<T>::GetThetaSlice(int_4 kt,r_8& theta, TVector<r_8>& phi, TVector<T>& value) const 

{

  if(kt < 0 || kt >= (int_4)NbThetaSlices()) 
    throw RangeCheckError("SphereThetaPhi::GetThetaSlice(kt...) index out of range"); 
    
  int_4 lring  = getRingNbPixels(kt);
  phi.ReSize(lring);
  value.ReSize(lring);
  int_4 idx0=getIndex(kt,0);
  double phi0;
  PixThetaPhi(idx0, theta, phi0);
  double Fi=phi0;
  double DFi = DeuxPi/(double)getRingNbPixels(kt);
  for(int_4 kk = 0; kk < lring; kk++)   {
    value(kk)= pixels_(idx0+kk);
    phi(kk)= Fi;
    Fi += DFi;
  }
  return;
}

/*
  \brief return information on a theta slice 
  For a theta-slice with index 'index', return : 
  the corresponding "theta" 
  the corresponding "phi" for first pixel of the slice 
  a vector containing the indices of the pixels of the slice
  (equally distributed in phi)
  a vector containing the corresponding values of pixels 
*/
template <class T>
void SphereThetaPhi<T>::GetThetaSlice(int_4 kt,r_8& theta, r_8& phi0,TVector<int_4>& pixelIndices, TVector<T>& value) const
{
  if(kt < 0 || kt >= (int_4)NbThetaSlices()) 
    throw RangeCheckError("SphereThetaPhi::GetThetaSlice(kt...) index out of range"); 

  int_4 lring  = getRingNbPixels(kt);
  pixelIndices.ReSize(lring);
  value.ReSize(lring);
  int_4 idx0=getIndex(kt,0);
  PixThetaPhi(idx0, theta, phi0);
  for(int_4 kk = 0; kk < lring; kk++)  {
    pixelIndices(kk)=kk+idx0;
    value(kk)= pixels_(kk+idx0);
  }
  return;
}

//! return a pointer to the specified slice pixel data
template <class T>
T* SphereThetaPhi<T>::GetThetaSliceDataPtr(int_4 index)
{
  if(index < 0 || index >= (int_4)NbThetaSlices()) 
    throw RangeCheckError("SphereThetaPhi::GetThetaSliceDataPtr(idx)  idx out of range"); 
  return pixels_.Begin()+Index(index,0);
}


template <class T>
void SphereThetaPhi<T>::print(ostream& os) const
{
  this->Show(os);
  os << "SphereThetaPhi<T> NTheta_= " << NTheta_ << " NPix_    = " << NPix_ 
     << " Omega_  =  " << Omega_   << endl;
  if(this->mInfo_) os << "  DVList Info= " << *(this->mInfo_) << endl;

  os << "... RingNbPixels Values : ";
  int_4 i;
  for(i=0; i < NTheta_; i++)
    {
      if(i%5 == 0) os << endl;
      os << getRingNbPixels(i) <<", ";
    }
  os << endl;

  os << "... CNphi_ Values : ";
  for(i=0; i < NTheta_+1; i++)
    {
      if(i%5 == 0) os << endl;
      os << CNphi_(i) <<", ";
    }
  os << endl;

  os << "... Pixel Values : ";
  for(i=0; i < NPix_; i++)
    {
      if(i%5 == 0) os << endl;
      os <<  pixels_(i) <<", ";
    }
  os << endl;
}

//   ...... Operations de calcul  ......


/*!
  throws a NotAllocatedError exception if the map has zero size 
*/ 
template <class T>
T SphereThetaPhi<T>::SetPixels(T a) 
{
  if (NbPixels() < 1) 
    throw NotAllocatedError("SphereThetaPhi<T>::SetPixels(T a)  - Not allocated spherical map ! ");
  pixels_ = a;
  return (a);
}

/*!
  throws a NotAllocatedError exception if the map has zero size 
*/ 
template <class T>
SphereThetaPhi<T>& SphereThetaPhi<T>::AddCst(T a)
 {
  if (NbPixels()< 1) 
    throw NotAllocatedError("SphereThetaPhi<T>::AddCst(T )  - SphereThetaPhi not dimensionned ! ");
  pixels_.AddCst(a); 
  return (*this);
}

/*!
   \param fginv : if false (default), perform the operation in normal order map.pixel[k] -= a
   if true, the operands order is exchanged: map.pixel[k] = a-map.pixel[k] 

  throws a NotAllocatedError exception if the map has zero size 
*/ 
template <class T>
SphereThetaPhi<T>& SphereThetaPhi<T>::SubCst(T a,bool fginv) 
{
  if (NbPixels()< 1) 
    throw NotAllocatedError("SphereThetaPhi<T>::SubCst(T )  - SphereThetaPhi not dimensionned ! ");
  pixels_.SubCst(a,fginv); 
  return (*this);
} 

/*!
  throws a NotAllocatedError exception if the map has zero size 
*/ 
template <class T>
SphereThetaPhi<T>& SphereThetaPhi<T>::MulCst(T a) 
{
  if (NbPixels()< 1) 
    throw NotAllocatedError("SphereThetaPhi<T>::Mul(T )  - SphereThetaPhi not dimensionned ! ");
  pixels_.MulCst(a); 
  return (*this);
}

/*!
   \param fginv : if false (default), perform the operation in normal order map.pixel[k] /= a
   if true, the operands order is exchanged: map.pixel[k] = a / map.pixel[k] 

  throws a NotAllocatedError exception if the map has zero size 
*/ 
template <class T>
SphereThetaPhi<T>& SphereThetaPhi<T>::DivCst(T a, bool fginv) 
{
  if (NbPixels()< 1) 
    throw NotAllocatedError("SphereThetaPhi<T>::Div(T )  - SphereThetaPhi not dimensionned ! ");
  pixels_.DivCst(a, fginv); 
  return (*this);
} 

//  >>>> Operations avec 2nd membre de type SphereThetaPhi
/*!
  throws NotAllocatedError exception if the map has zero size or 
  SzMismatchError exception if the two maps do not have the same pixelisation scheme (number of pixels ...)
*/ 
template <class T>
SphereThetaPhi<T>& SphereThetaPhi<T>::AddElt(const SphereThetaPhi<T>& a)
{
  if (NbPixels()< 1) 
    throw NotAllocatedError("SphereThetaPhi<T>::AddElt(T )  - SphereThetaPhi not dimensionned ! ");
  if (!isSamePixelisation(a))
    throw(SzMismatchError("SphereThetaPhi<T>::AddElt(const SphereThetaPhi<T>& a) different pixelisation ")) ;
  pixels_.AddElt(a.pixels_);
  return (*this);
}

/*!
  throws NotAllocatedError exception if the map has zero size or 
  SzMismatchError exception if the two maps do not have the same pixelisation scheme (number of pixels ...)

  \param fginv : if false (default), perform the operation in normal order map.pixel[k] -= a
  if true, the operands order is exchanged: map.pixel[k] = a-map.pixel[k] 
*/ 
template <class T>
SphereThetaPhi<T>& SphereThetaPhi<T>::SubElt(const SphereThetaPhi<T>& a, bool fginv)
{
  if (NbPixels()< 1) 
    throw NotAllocatedError("SphereThetaPhi<T>::SubElt(T )  - SphereThetaPhi not dimensionned ! ");
  if (!isSamePixelisation(a))
    throw(SzMismatchError("SphereThetaPhi<T>::SubElt(const SphereThetaPhi<T>& a) different pixelisation ")) ;
  pixels_.SubElt(a.pixels_,fginv);
  return (*this);
}

/*!
  throws NotAllocatedError exception if the map has zero size or 
  SzMismatchError exception if the two maps do not have the same pixelisation scheme (number of pixels ...)
*/ 
template <class T>
SphereThetaPhi<T>& SphereThetaPhi<T>::MulElt(const SphereThetaPhi<T>& a)
{
  if (NbPixels()< 1) 
    throw NotAllocatedError("SphereThetaPhi<T>::MulElt(T )  - SphereThetaPhi not dimensionned ! ");
  if (!isSamePixelisation(a))
    throw(SzMismatchError("SphereThetaPhi<T>::MulElt(const SphereThetaPhi<T>& a) different pixelisation ")) ;
  pixels_.MulElt(a.pixels_);
  return (*this);
}

/*!
  throws NotAllocatedError exception if the map has zero size or 
  SzMismatchError exception if the two maps do not have the same pixelisation scheme (number of pixels ...)
  
  \param fginv : if false (default), perform the operation in normal order map.pixel[k] /= a
  if true, the operands order is exchanged: map.pixel[k] = a / map.pixel[k] 
*/ 
template <class T>
SphereThetaPhi<T>& SphereThetaPhi<T>::DivElt(const SphereThetaPhi<T>& a, bool fginv)
{
  if (NbPixels()< 1) 
    throw NotAllocatedError("SphereThetaPhi<T>::DivElt(T )  - SphereThetaPhi not dimensionned ! ");
  if (!isSamePixelisation(a))
    throw(SzMismatchError("SphereThetaPhi<T>::DivElt(const SphereThetaPhi<T>& a) different pixelisation ")) ;
  pixels_.DivElt(a.pixels_,fginv);
  return (*this);
}


#ifdef __CXX_PRAGMA_TEMPLATES__
#pragma define_template SphereThetaPhi<int_4>
#pragma define_template SphereThetaPhi<r_8>
#pragma define_template SphereThetaPhi<r_4>
#pragma define_template SphereThetaPhi< complex<r_4> >
#pragma define_template SphereThetaPhi< complex<r_8> >
#endif
#if defined(ANSI_TEMPLATES) || defined(GNU_TEMPLATES)
template class SphereThetaPhi<uint_1>;
template class SphereThetaPhi<uint_2>;
template class SphereThetaPhi<int_1>;
template class SphereThetaPhi<int_2>;
template class SphereThetaPhi<int_4>;
template class SphereThetaPhi<int_8>;
template class SphereThetaPhi<r_4>;
template class SphereThetaPhi<r_8>;
template class SphereThetaPhi< complex<r_4> >;
template class SphereThetaPhi< complex<r_8> >;
#ifdef SO_LDBLE128
template class SphereThetaPhi<r_16>;
template class SphereThetaPhi< complex<r_16> >;
#endif

#endif

} // FIN namespace SOPHYA 
