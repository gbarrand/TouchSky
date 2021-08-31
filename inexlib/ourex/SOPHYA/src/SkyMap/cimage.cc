//  Classes image heritant de TMatrix<T>
//                       R.Ansari, C.Magneville 07/2000    
// LAL (Orsay) / IN2P3-CNRS  DAPNIA/SPP (Saclay) / CEA

#include "machdefs.h"
#include <stdio.h>
#include <stdlib.h>
#include "pexceptions.h"
#include "datatype.h"
#include "fioarr.h"
#include <typeinfo>

#define CIMAGE_CC_BFILE  // avoid extern template declarations 
#include "cimage.h"

using namespace std;

namespace SOPHYA {

/*!
   \class Image
   \ingroup SkyMap 

   This class specializes the TArray class, for representing intensity 
   (or grey-level) images. The Image<T> objects are always packed 
   array objects, with an optimized element access method.
   This class instances have also an ImgCoordTransform and an ImgSensorResponse attributes 
   describing the coordinate mapping and image sensor response. 

   \sa SOPHYA::ImgSensorResponse
   \sa SOPHYA::ImgCoordTransform

  This class can be instanciated for the following data types:
\verbatim
  ---Unsigned integer data types: 
  uint_1 uint_2  int_4 int_8 
  ---Signed integer data types: 
  int_1 int_2 int_4 int_8 
  ---Float and complex data types  
  r_4  r_8  complex<r_4>  complex<r_8>
  ---And if support for long double (SO_LDBLE128) enabled 
  r_16  complex<r_16>  
\endverbatim

*/

/* --Methode-- */
template <class T>
Image<T>::Image()
  : TArray<T>() , coordtr_() , sensresp_() , fgtrset_(false) , fgsrset_(false) 
{
}

/* --Methode-- */
template <class T>
Image<T>::Image(sa_size_t sizx, sa_size_t sizy, bool fzero)
  : TArray<T>(sizx, sizy, 0,0,0,fzero) , coordtr_() , sensresp_() , fgtrset_(false) , fgsrset_(false) 
{
}

/* --Methode-- */
template <class T>
Image<T>::Image(const Image<T>& a)
  : TArray<T>(a) ,  coordtr_(a.coordtr_) , sensresp_(a.sensresp_) , fgtrset_(a.fgtrset_) , fgsrset_(a.fgsrset_) 
{
}

/*!
  \param share : if true, the pixel data is shared, duplicated (cloned) if false.
*/
/* --Methode-- */
template <class T>
Image<T>::Image(const Image<T>& a, bool share)
  : TArray<T>(a, share) ,  coordtr_(a.coordtr_) , sensresp_(a.sensresp_) , fgtrset_(a.fgtrset_) , fgsrset_(a.fgsrset_) 
{
}

/* --Methode-- */
template <class T>
Image<T>::~Image()
{
}

/*!
  \param sx,sy : pixel array sizes 
  \param fzero : if true, the pixels are set to 0, even if the array size has not been changed 
  \warning  existing data will be lost if image size is changed
*/
/* --Methode-- */
template <class T>
void Image<T>::SetSize(sa_size_t sx, sa_size_t sy, bool fzero)
{
  if ((sx==size_[0])&&(sy==size_[1]))  {
    if (fzero) (*this)=T(0); 
    return;
  }
  sa_size_t sz[2]={0,0};
  sz[0]=sx, sz[1]=sy;
  TArray<T>::SetSize(2,sz,1,fzero);
}

/* --Methode-- */
template <class T>
Image<T> Image<T>::SubImage(Range rx, Range ry) const 
{
  TArray<T> sa = SubArray(rx, ry, Range::first(), Range::first(), Range::first(), true);
  Image<T> reti(sa.SizeX(), sa.SizeY(), false);
  reti.CopyElt(sa);
  reti.CopyTrResp(*this);
  reti.getCoordMapping().setImageReferencePoint((double)rx.Start(), (double)ry.Start());
  return reti;
}

/* --Methode-- */
template <class T>
void Image<T>::Show(ostream& os, bool si) const
{
  os << "Image<"<<DataTypeInfo<T>::getTypeName()<<">("<<SizeX()<<","<<SizeY()<<") \n";
  os << coordtr_ << sensresp_ << endl;
  DVList* dvlp=getInfoPointer();
  if (si && (dvlp!=NULL)) os << *(dvlp) << endl;
  return;
}

/* --Methode-- */
template <class T>
void Image<T>::CopyFrom(const Image<T>& a)
{
  if (a.NbDimensions() < 1) 
    throw NotAllocatedError("Image<T>::CopyFrom(a ) - Image a not allocated ! ");
  if (NbDimensions() < 1)  SetSize(a.SizeX(), a.SizeY(), false); 
  if ((a.SizeX()!=SizeX())||(a.SizeY()!=SizeY()))  
    throw SzMismatchError("Image<T>::CopyFrom(a) the two images have different sizes");
  TArray<T>::CopyElt(a); //JEC inheritance of Template method (voir http://womble.decadent.org.uk/c++/template-faq.html Q:2)
  coordtr_=a.coordtr_;
  sensresp_=a.sensresp_;
  fgtrset_=a.fgtrset_; 
  fgsrset_=a.fgsrset_;
}

/* --Methode-- */
template <class T>
void Image<T>::CopyTrResp(const Image<T>& a)
{
  if (a.fgtrset_)  { coordtr_=a.coordtr_;    fgtrset_=true; }
  if (a.fgsrset_)  { sensresp_=a.sensresp_;  fgsrset_=true; }
}

///////////////////////////////////////////////////////////////
#ifdef __CXX_PRAGMA_TEMPLATES__
#pragma define_template Image<uint_1>
#pragma define_template Image<uint_2>
#pragma define_template Image<uint_4>
#pragma define_template Image<uint_8>
#pragma define_template Image<int_1>
#pragma define_template Image<int_2>
#pragma define_template Image<int_4>
#pragma define_template Image<int_8>
#pragma define_template Image<r_4>
#pragma define_template Image<r_8> 
#pragma define_template Image< complex<r_4> > 
#pragma define_template Image< complex<r_8> > 
#ifdef SO_LDBLE128
#pragma define_template Image<r_16> 
#pragma define_template Image< complex<r_16> > 
#endif
#endif

#if defined(ANSI_TEMPLATES) || defined(GNU_TEMPLATES)
template class Image<uint_1>;
template class Image<uint_2>;
template class Image<uint_4>;
template class Image<uint_8>;
template class Image<int_1>;
template class Image<int_2>;
template class Image<int_4>;
template class Image<int_8>;
template class Image<r_4>;
template class Image<r_8>;
template class Image< complex<r_4> >;
template class Image< complex<r_8> >;
#ifdef SO_LDBLE128
template class Image<r_16>;
template class Image< complex<r_16> >;
#endif
#endif

} // FIN namespace SOPHYA 
