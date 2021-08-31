/* ------------------------------------------------------------------------------------
   SOPHYA class library - (C)  UPS+LAL IN2P3/CNRS  , (C) CEA-Irfu-SPP 
   Spherical map in HEALPix pixelisation    
   G. Le Meur, F. Touze   2000
   Updated: R. Ansari 2015  
   ------------------------------------------------------------------------------------  */

#include "machdefs.h"
#include <math.h>
#include <complex>

#include "pexceptions.h"
#include "fiondblock.h"
#include "strutil.h"

#define SPHEREHEALPIX_CC_BFILE  // avoid extern template declarations 
#include "spherehealpix.h"


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


using namespace std;

namespace SOPHYA {

/*!
  \class SphereHEALPix
  \ingroup SkyMap
  \brief Spherical maps in HEALPix pixelisation scheme.

  Class implementing spherical maps, in the HEALPix pixelisation scheme, 
  with template data types (double, float, complex, ...)

\verbatim
    Adapted from :
    -----------------------------------------------------------------------
     version 0.8.2  Aug97 TAC  Eric Hivon, Kris Gorski
    -----------------------------------------------------------------------
    by G. Le Meur , F. Touze  (01/2000)
 
    the sphere is split in 12 diamond-faces containing nside**2 pixels each
    the numbering of the pixels (in the nested scheme) is similar to
    quad-cube
    In each face the first pixel is in the lowest corner of the diamond
    the faces are                    (x,y) coordinate on each face

        .   .   .   .   <--- North Pole
       / \ / \ / \ / \                          ^        ^     
      . 0 . 1 . 2 . 3 . <--- z = 2/3             \      / 
       \ / \ / \ / \ /                        y   \    /  x  
      4 . 5 . 6 . 7 . 4 <--- equator               \  /      
       / \ / \ / \ / \                              \/      
      . 8 . 9 .10 .11 . <--- z = -2/3              (0,0) : lowest corner 
       \ / \ / \ / \ /      
        .   .   .   .   <--- South Pole
\endverbatim
    phi:0               2Pi                                 

   in the ring scheme pixels are numbered along the parallels
   the first parallel is the one closest to the north pole and so on  
   on each parallel, pixels are numbered starting from the one closest
    to phi = 0

    nside MUST be a power of 2 (<= 8192)

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

/* --Methode-- */

/*! 
  \brief Constructor with optional parameter specifying the pixelisation scheme RING (default) or NESTED

  The default constructor creates a 0 size map in RING pixelisation 

  \param fgring : true -> RING scheme , false -> NESTED 
*/

template<class T>
SphereHEALPix<T>::SphereHEALPix(bool fgring) 
  : HEALPixBase(fgring), pixels_(), sliceBeginIndex_(), sliceLenght_()
{
}

/*! \brief Constructor with specification of nside and optional pixelisation scheme
  
  \param <m> : "nside" of the Healpix algorithm
  \param <fgring> : if true -> RING pixelisation (default), if not NESTED

  The total number of pixels will be Npix =  12*nside**2

  nside MUST be a power of 2 (<= 8192)
*/

template<class T>
SphereHEALPix<T>::SphereHEALPix(int_4 m, bool fgring)
  : HEALPixBase(fgring)

{
  setNSide(m);
}

/*!  
  \brief Copy constructor, sharing or copying map data 
  
  \param share : true -> shares the map data, false -> copies the data 
*/
template<class T>
SphereHEALPix<T>::SphereHEALPix(const SphereHEALPix<T>& s, bool share)
  : pixels_(s.pixels_, share), sliceBeginIndex_(s.sliceBeginIndex_, share), 
    sliceLenght_(s.sliceLenght_, share)
{
  copyFrom(s);
  if(s.mInfo_) this->mInfo_= new DVList(*s.mInfo_);
}

template<class T>
SphereHEALPix<T>::SphereHEALPix(const SphereHEALPix<T>& s)
  : pixels_(s.pixels_), sliceBeginIndex_(s.sliceBeginIndex_), 
				sliceLenght_(s.sliceLenght_)
{
  copyFrom(s);
  if(s.mInfo_) this->mInfo_= new DVList(*s.mInfo_);
  //  CloneOrShare(s);
}

//! Clone if \b a is not temporary, share if temporary
/*! \sa NDataBlock::CloneOrShare(const NDataBlock<T>&) */
template<class T>
void SphereHEALPix<T>::CloneOrShare(const SphereHEALPix<T>& a)
{
  copyFrom(a);
  pixels_.CloneOrShare(a.pixels_);
  sliceBeginIndex_.CloneOrShare(a.sliceBeginIndex_);
  sliceLenght_.CloneOrShare(a.sliceLenght_);
  if (this->mInfo_) {delete this->mInfo_; this->mInfo_ = NULL;}
  if (a.mInfo_) this->mInfo_ = new DVList(*(a.mInfo_));
}

//! Share data with a
template<class T>
void SphereHEALPix<T>::Share(const SphereHEALPix<T>& a)
{
  copyFrom(a);
  pixels_.Share(a.pixels_);
  sliceBeginIndex_.Share(a.sliceBeginIndex_);
  sliceLenght_.Share(a.sliceLenght_);
  if (this->mInfo_) {delete this->mInfo_; this->mInfo_ = NULL;}
  if (a.mInfo_) this->mInfo_ = new DVList(*(a.mInfo_));
}

////////////////////////// methodes de copie/share
template<class T>
SphereHEALPix<T>& SphereHEALPix<T>::Set(const SphereHEALPix<T>& a)
{
  if (this != &a)   { 
      if (a.NbPixels() < 1) 
	throw RangeCheckError("SphereHEALPix<T>::Set(a ) - SphereHEALPix a not allocated ! ");
      if (NbPixels() < 1) CloneOrShare(a);
      else CopyElt(a);
      
      if (this->mInfo_) delete this->mInfo_;
      this->mInfo_ = NULL;
      if (a.mInfo_) this->mInfo_ = new DVList(*(a.mInfo_));
  }
  return(*this);
}

template<class T>
SphereHEALPix<T>& SphereHEALPix<T>::CopyElt(const SphereHEALPix<T>& a)
{
  if (NbPixels() < 1) 
    throw RangeCheckError("SphereHEALPix<T>::CopyElt(a)  - Not Allocated SphereHEALPix ! ");
  if (NbPixels() != a.NbPixels()) 
    throw(SzMismatchError("SphereHEALPix<T>::CopyElt(a) SizeMismatch")) ;
  if (fgring_ == a.fgring_)   pixels_ = a.pixels_; 
  else {
    if (fgring_)  for (int_4 k=0; k< nPix_; k++) 
      pixels_(k) = a.pixels_(HEALPixUtils::ring2nest(nSide_, k));
    else  for (int_4 k=0; k< nPix_; k++) 
      pixels_(k) = a.pixels_(HEALPixUtils::nest2ring(nSide_, k));			     
  }
  return(*this);
}

template<class T>
SphereHEALPix<T>::~SphereHEALPix()
{
}

/*!
  \brief Set or changes the pixelisation resolution by defining the HEALPix \b nside parameter

  redefinition of the base clase SOPHYA::HEALPixBase::setNSide() method 
  can throw ParmError exception if \b m is invalid  

  \param nside : nside should be a power of 2 ( nside = 2^p ) and 1 <= nside <= 8192   
*/
template<class T>
void SphereHEALPix<T>::setNSide(int_4 nside, bool fgring)
{
  HEALPixBase::setNSide(nside, fgring);
  pixels_.ReSize(nPix_, true);
  SetThetaSlices();
}

/*!  \fn   void SOPHYA::SphereHEALPix::Resize(int_4 m)
  \param <m>   "nside" of the HEALPix algorithm

  The total number of pixels will be Npix =  12*nside**2

  nside MUST be a power of 2 (<= 8192)
*/
template<class T>
void SphereHEALPix<T>::Resize(int_4 m)
{
  if ((m <= 0 && nSide_ > 0))  {
    if (SphericalMap_GetGlobalPrintLevel()>0) 
      cout << "SphereHEALPix<T>::Resize(m) with m<=0, NOT resized" << endl;
    return;
  }
  setNSide(m);
}


//! return type of the map pixelisation : RING or NESTED
template<class T>
string SphereHEALPix<T>::TypeOfMap() const 
{
  if (fgring_)  return string("RING");
  else return string("NESTED");
}


template<class T>
int_4 SphereHEALPix<T>::NbPixels() const
{  
  return(nPix_);
}


/*! \fn uint_4 SOPHYA::SphereHEALPix::NbThetaSlices() const

   \return number of slices in theta direction on the  sphere 
*/
template<class T>
uint_4 SphereHEALPix<T>::NbThetaSlices() const 
{
  if (nSide_<=0)  return 0;
  uint_4 nbslices = uint_4(4*nSide_-1);
  return nbslices;
}

//! Return the theta angle for slice defined by \b index
template<class T>
r_8 SphereHEALPix<T>::ThetaOfSlice(int_4 index) const
{
  uint_4 nbslices = uint_4(4*nSide_-1);
  if (nSide_<=0)  nbslices = 0;
  if (index<0 || index >= (int_4)nbslices) 
    throw RangeCheckError(" SphereHEALPix::ThetaOfSlice()  index out of range");
  r_8 theta, phi0;
  PixThetaPhi(sliceBeginIndex_(index), theta, phi0);
  return theta;
}

//! Return true : All theta slices have a symmetric slice at Pi-Theta in SphereHEALPix
template <class T>
bool SphereHEALPix<T>::HasSymThetaSlice() const 
{
  return true;
}
//! Return the slice index for the symmetric slice at theta=Pi-ThetaOfSlice(idx) 
template <class T>
int_4 SphereHEALPix<T>::GetSymThetaSliceIndex(int_4 idx) const
{
  if(idx < 0 || idx >= (int_4)NbThetaSlices()) 
    throw RangeCheckError("SphereHEALPix::GetSymThetaSliceIndex index out of range"); 
  return (NbThetaSlices()-1-idx);
}

/*!  \fn void SOPHYA::SphereHEALPix::GetThetaSlice(int_4 index,r_8& theta,TVector<r_8>& phi,TVector<T>& value) const 

 For a theta-slice with index 'index', return :
 
   the corresponding "theta" 

   a vector containing the phi's of the pixels of the slice

   a vector containing the corresponding values of pixels 
*/
template<class T>
void SphereHEALPix<T>::GetThetaSlice(int_4 index,r_8& theta,TVector<r_8>& phi,TVector<T>& value) const 
{
  if (index<0 || index >= (int_4)NbThetaSlices()) 
    throw RangeCheckError(" SphereHEALPix::GetThetaSlice()  index out of range");

  int_4 iring= sliceBeginIndex_(index);
  int_4 lring  =  sliceLenght_(index);

  phi.ReSize(lring);
  value.ReSize(lring);

  double TH= 0.;
  double FI= 0.;
  if (fgring_) {  // RING pixelisation scheme
    for(int_4 kk = 0; kk < lring;kk++)  {
      PixThetaPhi(kk+iring,TH,FI);
      phi(kk)= FI;
      value(kk)= pixels_(kk+iring);
    }
    PixThetaPhi(iring, theta, FI);
  }
  else {  // NESTED pixelisation scheme
    for(int_4 kk = 0; kk < lring;kk++)  {
      int kkn = HEALPixUtils::ring2nest(nSide_, kk+iring);
      PixThetaPhi(kkn,TH,FI);
      phi(kk)= FI;
      value(kk)= pixels_(kkn);
    }
    PixThetaPhi(HEALPixUtils::ring2nest(nSide_,iring), theta, FI);    
  }
  //  theta= TH;
}
/*! \fn void SOPHYA::SphereHEALPix::GetThetaSlice(int_4 sliceIndex,r_8& theta, r_8& phi0, TVector<int_4>& pixelIndices,TVector<T>& value) const

   For a theta-slice with index 'index', return : 

   the corresponding "theta" 

   the corresponding "phi" for first pixel of the slice 

    a vector containing indices of the pixels of the slice

   (equally distributed in phi)

    a vector containing the corresponding values of pixels 
*/

template<class T>
void SphereHEALPix<T>::GetThetaSlice(int_4 sliceIndex,r_8& theta, r_8& phi0, 
				     TVector<int_4>& pixelIndices,TVector<T>& value) const 

{

  if (sliceIndex<0 || sliceIndex >= (int_4)NbThetaSlices()) 
    throw RangeCheckError(" SphereHEALPix::GetThetaSlice() index out of range");
  int_4 iring= sliceBeginIndex_(sliceIndex);
  int_4 lring  =  sliceLenght_(sliceIndex);
  pixelIndices.ReSize(lring);
  value.ReSize(lring);

  if (fgring_) {  // RING pixelisation scheme
    for(int_4 kk = 0; kk < lring;kk++)  {
      pixelIndices(kk)= kk+iring;
      value(kk)= pixels_(kk+iring);
    }
    PixThetaPhi(iring, theta, phi0);
  }
  else {  // NESTED pixelisation scheme
    for(int_4 kk = 0; kk < lring;kk++)  {
      int_4 kkn = HEALPixUtils::ring2nest(nSide_, kk+iring);
      pixelIndices(kk)= kkn;
      value(kk)= pixels_(kkn);
    }
    PixThetaPhi(HEALPixUtils::ring2nest(nSide_,iring), theta, phi0);    
  }
}

//! return a pointer to the specified slice pixel data in RING ordering, NULL in NESTED
template<class T>
T* SphereHEALPix<T>::GetThetaSliceDataPtr(int_4 sliceIndex) 

{
  if (sliceIndex<0 || sliceIndex >= (int_4)NbThetaSlices()) 
    throw RangeCheckError(" SphereHEALPix::GetThetaSliceDataPtr(): index out of range");
  if (fgring_) 
    return pixels_.Begin()+sliceBeginIndex_(sliceIndex);
  else return NULL;
}

template<class T>
void SphereHEALPix<T>::SetThetaSlices()  
{
  sliceBeginIndex_.ReSize(4*nSide_-1);
  sliceLenght_.ReSize(4*nSide_-1);
  int_4 sliceIndex;
  int_4 offp = 0;
  for (sliceIndex=0; sliceIndex<  nSide_-1; sliceIndex++)  {
    //      sliceBeginIndex_(sliceIndex)  = 2*sliceIndex*(sliceIndex+1);      
    sliceBeginIndex_(sliceIndex)  = offp;
    sliceLenght_(sliceIndex) = 4*(sliceIndex+1);
    offp += sliceLenght_(sliceIndex);
  }
  for (sliceIndex= nSide_-1; sliceIndex<  3*nSide_; sliceIndex++)  {
    //      sliceBeginIndex_(sliceIndex)  = 2*nSide_*(2*sliceIndex-nSide_+1);
    sliceBeginIndex_(sliceIndex)  = offp;
    sliceLenght_(sliceIndex) = 4*nSide_;
    offp += sliceLenght_(sliceIndex);
  }
  for (sliceIndex= 3*nSide_; sliceIndex< 4*nSide_-1; sliceIndex++) {
    int_4 nc= 4*nSide_-1-sliceIndex;
      //      sliceBeginIndex_(sliceIndex)  = nPix_-2*nc*(nc+1); 
    sliceBeginIndex_(sliceIndex)  = offp; 
    sliceLenght_(sliceIndex) = 4*nc;
    offp += sliceLenght_(sliceIndex);    
  }
}



/*!
  \return value of the \b k th pixel 
*/
template<class T>
T& SphereHEALPix<T>::PixVal(int_4 k)
{
  if((k < 0) || (k >= nPix_)) 
      throw RangeCheckError("SphereHEALPix::PixVal()  index out of range");
  return pixels_(k);
}

/*!
  \return value of the \b k th pixel 
*/
template<class T>
T  SphereHEALPix<T>::PixVal(int_4 k) const
{
  if((k < 0) || (k >= nPix_)) 
    throw RangeCheckError("SphereHEALPix::PIxVal() const  index out of range");
  return *(pixels_.Data()+k);
}


/*! \fn bool SOPHYA::SphereHEALPix::ContainsSph(double theta, double phi) const

\return true if teta,phi in map 
*/
template<class T>
bool SphereHEALPix<T>::ContainsSph(double theta, double phi) const
{
return(true);
}

/*! \fn int_4 SOPHYA::SphereHEALPix::PixIndexSph(double theta,double phi) const

 \return "RING" index of the pixel corresponding to direction (theta, phi).
 */
template<class T>
int_4 SphereHEALPix<T>::PixIndexSph(double theta,double phi) const

{
  if (fgring_) return HEALPixUtils::ang2pix_ring(nSide_,theta,phi);
  else return HEALPixUtils::ang2pix_nest(nSide_,theta,phi);
}


//!  \return (theta,phi) coordinates of middle of  pixel with "RING" index k
template<class T>
void SphereHEALPix<T>::PixThetaPhi(int_4 k,double& theta,double& phi) const
{
  if (fgring_) HEALPixUtils::pix2ang_ring(nSide_,k,theta,phi);
  else HEALPixUtils::pix2ang_nest(nSide_,k,theta,phi);
}



//   ...... Operations de calcul  ......

//! Fill a SphereHEALPix with a constant value \b a
template <class T>
T SphereHEALPix<T>::SetPixels(T a) 
{
  if (NbPixels() < 1) 
    throw NotAllocatedError("SphereHEALPix<T>::SetPixels(T a)  - not allocated map ! ");
  pixels_ = a;
  return (a);
}

/*! Add a constant value \b x to a SphereHEALPix */
template <class T>
SphereHEALPix<T>& SphereHEALPix<T>::AddCst(T a)
 {
  if (NbPixels() < 1) 
    throw RangeCheckError("SphereHEALPix<T>::AddCst(T )  - SphereHEALPix not dimensionned ! ");
  //   pixels_ += a; 
   pixels_.AddCst(a); 
  return (*this);
}

//  
/*!
  Substract a constant value \b from the SphereHEALPix object pixel values 

   \param fginv : if false (default), perform the operation in normal order map.pixel[k] -= a
   if true, the operands order is exchanged: map.pixel[k] = a-map.pixel[k] 
*/
template <class T>
SphereHEALPix<T>& SphereHEALPix<T>::SubCst(T a,bool fginv) 
{
  if (NbPixels() < 1) 
    throw RangeCheckError("SphereHEALPix<T>::SubCst(T )  - SphereHEALPix not dimensionned ! ");
  pixels_.SubCst(a,fginv); 
  return (*this);
} 

/*! multiply a SphereHEALPix by a constant value \b a */
template <class T>
SphereHEALPix<T>& SphereHEALPix<T>::MulCst(T a) 
{
  if (NbPixels() < 1) 
    throw RangeCheckError("SphereHEALPix<T>::MulCst(T )  - SphereHEALPix not dimensionned ! ");
  pixels_.MulCst(a); 
  return (*this);
}

/*!
   divide a SphereHEALPix by a constant value \b a 

   \param fginv : if false (default), perform the operation in normal order map.pixel[k] /= a
   if true, the operands order is exchanged: map.pixel[k] = a / map.pixel[k] 
*/

template <class T>
SphereHEALPix<T>& SphereHEALPix<T>::DivCst(T a, bool fginv) 
{
  if (NbPixels() < 1) 
    throw RangeCheckError("SphereHEALPix<T>::DivCst(T )  - SphereHEALPix not dimensionned ! ");
  pixels_.DivCst(a, fginv); 
  return (*this);
} 

//  >>>> Operations avec 2nd membre de type SphereHEALPix
/*! Add two SphereHEALPix pixel wise */

template <class T>
SphereHEALPix<T>& SphereHEALPix<T>::AddElt(const SphereHEALPix<T>& a)
{
  if (NbPixels() != a.NbPixels() )
    throw(SzMismatchError("SphereHEALPix<T>::AddElt(a) SizeMismatch")) ;
  if (fgring_ != a.fgring_) 
    throw(ParmError("SphereHEALPix<T>::AddElt(a) different pixelisation RING<>NESTED")) ;

  pixels_.AddElt(a.pixels_);
  return (*this);
}

/*! 
  Substract two SphereHEALPix pixel wise 

  \param fginv : if false (default), perform the operation in normal order map.pixel[k] -= a
  if true, the operands order is exchanged: map.pixel[k] = a-map.pixel[k] 
*/
template <class T>
SphereHEALPix<T>& SphereHEALPix<T>::SubElt(const SphereHEALPix<T>& a, bool fginv)
{
  if (NbPixels() != a.NbPixels() )
    throw(SzMismatchError("SphereHEALPix<T>::SubElt(a) SizeMismatch")) ;
  if (fgring_ != a.fgring_) 
    throw(ParmError("SphereHEALPix<T>::SubElt(a) different pixelisation RING<>NESTED")) ;

  pixels_.SubElt(a.pixels_, fginv);
  return (*this);
}

/*! Multiply two SphereHEALPix pixel wise  */
template <class T>
SphereHEALPix<T>& SphereHEALPix<T>::MulElt(const SphereHEALPix<T>& a)
{
  if (NbPixels() != a.NbPixels() )
    throw(SzMismatchError("SphereHEALPix<T>::MulElt(a) SizeMismatch")) ;
  if (fgring_ != a.fgring_) 
    throw(ParmError("SphereHEALPix<T>::MulElt(a) different pixelisation RING<>NESTED")) ;

  pixels_.MulElt(a.pixels_);
  return (*this);
}

/*! 
  Divide two SphereHEALPix pixel wise - No protection for divide by 0

  \param fginv : if false (default), perform the operation in normal order map.pixel[k] /= a
  if true, the operands order is exchanged: map.pixel[k] = a / map.pixel[k] 
*/
template <class T>
SphereHEALPix<T>& SphereHEALPix<T>::DivElt(const SphereHEALPix<T>& a, bool fginv)
{
  if (NbPixels() != a.NbPixels() )
    throw(SzMismatchError("SphereHEALPix<T>::DivElt(a) SizeMismatch")) ;
  if (fgring_ != a.fgring_) 
    throw(ParmError("SphereHEALPix<T>::DivElt(a) different pixelisation RING<>NESTED")) ;
  pixels_.DivElt(a.pixels_,fginv);
  return (*this);
}




template <class T>
void SphereHEALPix<T>::print(ostream& os) const
{
  this->Show(os);
  os << "SphereHEALPix<T>(" << TypeOfMap() << ") NSide= " 
     << nSide_ << " nPix_   = " << nPix_  << " omeg_ = " << omeg_   << endl;

  if(this->mInfo_) os << "  DVList Info= " << *(this->mInfo_) << endl;
  os << "... Pixel values : ";
  for(int i=0; i < nPix_; i++)
    {
      if(i%5 == 0) os << endl;
      os <<  pixels_(i) <<", ";
    }
  os << endl;


}


//*******************************************************************

#ifdef __CXX_PRAGMA_TEMPLATES__
#pragma define_template SphereHEALPix<uint_2>
#pragma define_template SphereHEALPix<int_4>
#pragma define_template SphereHEALPix<r_8>
#pragma define_template SphereHEALPix<r_4>
#pragma define_template SphereHEALPix< complex<r_4> >
#pragma define_template SphereHEALPix< complex<r_8> >
#endif
#if defined(ANSI_TEMPLATES) || defined(GNU_TEMPLATES)
template class SphereHEALPix<uint_1>;
template class SphereHEALPix<uint_2>;
template class SphereHEALPix<int_1>;
template class SphereHEALPix<int_2>;
template class SphereHEALPix<int_4>;
template class SphereHEALPix<int_8>;
template class SphereHEALPix<r_4>;
template class SphereHEALPix<r_8>;
template class SphereHEALPix< complex<r_4> >;
template class SphereHEALPix< complex<r_8> >;
#ifdef SO_LDBLE128
template class SphereHEALPix<r_16>;
template class SphereHEALPix< complex<r_16> >;
#endif
#endif

} // FIN namespace SOPHYA 


