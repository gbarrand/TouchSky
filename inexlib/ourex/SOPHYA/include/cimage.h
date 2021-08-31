//------------------------------------------------------------------------------------
// Class  Image<T> and ancillary classes 
// SOPHYA class library - (C)  UPS+LAL IN2P3/CNRS  , CEA-Irfu 
// The class Image<T> is intended for representing an 2D map of intensities in a given 
// photometric band - typically an astronomical image. 
// Initial version:   R.Ansari, C.Magneville 07/2000  
// Major redesign: Jan 2015 
//                     R. Ansari, J.E.Campagne, C. Magneville
//------------------------------------------------------------------------------------
#ifndef CIMAGE_H_SEEN
#define CIMAGE_H_SEEN

#include "tarray.h"
#include "imgcoordtr.h"
#include "imgsensresp.h"

#include <string>

namespace SOPHYA {
//--------------------------------------------------------------------------------   
//------------------------------  Image<T> class ---------------------------------
//--------------------------------------------------------------------------------   

// Forward declaration for the PPF handler class
template <class T> class FIO_Image;

//! Class for handling images (2D array of light intensity measurements), astronomical images for example
template <class T>
class Image : public TArray<T> {
public:

  #include "tmatrix_tsnl.h"  /* For two level name look-up gcc >= 3.4 */

  //! Default constructor , non allocated image array 
  Image();
  //! Constructor with image size  
  Image(sa_size_t sizx, sa_size_t sizy, bool fzero=true);
  //! Copy constructor - pixel data is shared between objects 
  Image(const Image<T>& a);
  //! Copy constructor with share/clone flag, (share=false -> Clone pixel data, Share pixel data otherwise) 
  Image(const Image<T>& a, bool share);
  //! Destructor 
  virtual ~Image();

  //! defining or changing the pixel array size. 
  void SetSize(sa_size_t sx, sa_size_t sy, bool fzero=true);
  //! alias for SetSize() : defining or changing the pixel array size. 
  inline void ReSize(sa_size_t sx, sa_size_t sy, bool fzero=true)  { SetSize(sx, sy, fzero); }

  //! element access operator , const version 
  inline T const& operator()(sa_size_t ix, sa_size_t jy) const;
  //! element access operator  
  inline T&       operator()(sa_size_t ix, sa_size_t jy);

  //! Returns the image size along X - alias for SizeX()
  inline sa_size_t  XSize() const {return size_[0];}
  //! Returns the image size along Y - alias for SizeY()
  inline sa_size_t  YSize() const {return size_[1];}

  //! copy operator 
  inline Image<T>&  operator = (const Image<T>& a)  { CopyFrom(a); return *this; }

  //! defines the coordinate mapping between the image space and real (sky, earth ...) space 
  inline void setCoordMapping(ImgCoordTransform const& tr)  { coordtr_=tr;  fgtrset_=true; }
  //! defines the sensor response and wavelength band-pass associated with the image 
  inline void setSensorResponse(ImgSensorResponse const& sr)  { sensresp_=sr; fgsrset_=true; }
  //! access to the coordinate mapping object
  inline ImgCoordTransform & getCoordMapping()  { fgtrset_=true;  return coordtr_; }
  //! access to the coordinate mapping object - const version 
  inline ImgCoordTransform const& getCoordMapping(bool & fgset) const { fgset=fgtrset_;  return coordtr_; }
  //! access to the sensor response object
  inline ImgSensorResponse & getSensorResponse()  { fgsrset_=true;  return sensresp_;  }
  //! access to the sensor response object - const version 
  inline ImgSensorResponse const& getSensorResponse(bool & fgset)  { fgset=fgsrset_;  return sensresp_;  }

  //! Extract a SubImage defined by the two pixels ranges (rx, ry) 
  Image<T> SubImage(Range rx, Range ry) const ;

//! Show infos on stream \b os (\b si to display DvList)
  virtual void   Show(std::ostream& os, bool si=false) const;

  //! deep copy function 
  void CopyFrom(const Image<T>& a);

  //------------ Basic arithmetic operations on pixels 
  //! operator = : set all image pixels to the constant value \b x
  inline  Image<T>&  operator = (T x) {SetT(x); return(*this); }
  //! Add \b x to all pixels 
  inline  Image<T>&  operator += (T x)        { AddCst(x, *this);  return *this; }
  //! Substract \b x from all pixels 
  inline  Image<T>&  operator -= (T x)        { SubCst(x, *this);  return *this; }
  //! Multiply all pixels by \b x
  inline  Image<T>&  operator *= (T x)        { MulCst(x, *this);  return *this; }
  //! Divide all pixels by \b x
  inline  Image<T>&  operator /= (T x)        { DivCst(x, *this);  return *this; }

  //! Operator Image<T> += Image<T> (pixel by pixel addition in place)
  inline  Image<T>&  operator += (const Image<T>& a)  { AddElt(a, *this); return *this; }
  //! Operator Image<T> -= Image<T> (pixel by pixel subtraction in place)
  inline  Image<T>&  operator -= (const Image<T>& a)  { SubElt(a, *this); return *this; }
  //! Operator Image<T> *= Image<T> (pixel by pixel product in place)
  inline  Image<T>&  operator *= (const Image<T>& a)  { MulElt(a, *this); return *this; }
  //! Operator Image<T> /= Image<T> (pixel by pixel division in place)
  inline  Image<T>&  operator /= (const Image<T>& a)  { DivElt(a, *this); return *this; }

  //! special method to copy the ImgCoordTransform and ImgSensorResponse if set from the objet a
  void CopyTrResp(const Image<T>& a);

  // declaration for the PPF handler class
  friend class  FIO_Image<T>;
protected:
  ImgCoordTransform coordtr_;      //!< description of the coordinate mapping between image space and real space
  ImgSensorResponse sensresp_;     //!< description of the image sensor and band-pass filter 
  bool fgtrset_;                   //!< true if coordtr_ has been defined 
  bool fgsrset_;                   //!< true if sensresp_ has been defined 

};

//  Image<T> is always a packed array , so the access operator is optimized and simplified 
template <class T>
inline T const& Image<T>::operator()(sa_size_t ix, sa_size_t jy) const
{
#ifdef SO_BOUNDCHECKING
  CheckBound(ix, jy, 0, 0, 0, 4);
#endif
  return ( *( mNDBlock.Begin()+ jy*size_[0]+ix ) );
}

//! () : Return the pixel value (element) for pixel \b ix (column index) and \b jy (line index)
template <class T>
inline T & Image<T>::operator()(sa_size_t ix, sa_size_t jy) 
{
#ifdef SO_BOUNDCHECKING
  CheckBound(ix, jy, 0, 0, 0, 4);
#endif
  return ( *( mNDBlock.Begin()+ jy*size_[0]+ix ) );
}

//! Prints the Image<T> type, size, and coordinate mapping transform and SensorResponse on the stream \b os
template <class T>
inline std::ostream& operator << (std::ostream& os, const Image<T>& a)
{ a.Show(os);    return(os);  }


////////////////////////////////////////////////////////////////
// Surcharge d'operateurs A (+,-,*,/) (T) x

/*! \ingroup SkyMap \fn operator+(const Image<T>&,T)
  \brief Operator Image = Image + constant */
template <class T> inline Image<T> operator + (const Image<T>& a, T b)
{
  Image<T> result;  result.SetTemp(true);  a.AddCst(b, result);  
  result.CopyTrResp(a);  return result;
}

/*! \ingroup SkyMap \fn operator+(T,const Image<T>&)
  \brief Operator Image = constant + Image */
template <class T> inline Image<T> operator + (T b, const Image<T>& a)
{
  Image<T> result(a,false);  result.SetTemp(true);  result.AddCst(b, result); 
  result.CopyTrResp(a);  return result;
}

/*! \ingroup SkyMap \fn operator-(const Image<T>&,T)
  \brief Operator Image = Image - constant */
template <class T> inline Image<T> operator - (const Image<T>& a, T b)
{
  Image<T> result(a,false); result.SetTemp(true);  a.SubCst(b,result); 
  result.CopyTrResp(a);  return result;
}

/*! \ingroup SkyMap \fn operator-(T,const Image<T>&)
  \brief Operator Image = constant - Image */
template <class T> inline Image<T> operator - (T b,const Image<T>& a)
{
  Image<T> result;  result.SetTemp(true);  a.SubCst(b,result,true); 
  result.CopyTrResp(a);  return result;
}

/*! \ingroup SkyMap \fn operator*(const Image<T>&,T)
  \brief Operator Image = Image * constant */
template <class T> inline Image<T> operator * (const Image<T>& a, T b)
{
  Image<T> result;  result.SetTemp(true);  a.MulCst(b, result); 
  result.CopyTrResp(a);  return result;
}

/*! \ingroup SkyMap \fn operator*(T,const Image<T>&)
  \brief Operator Image = constant * Image */
template <class T> inline Image<T> operator * (T b, const Image<T>& a)
{
  Image<T> result; result.SetTemp(true); a.MulCst(b,result); 
  result.CopyTrResp(a);  return result;
}

/*! \ingroup SkyMap \fn operator/(const Image<T>&,T)
  \brief Operator Image = Image / constant */
template <class T> inline Image<T> operator / (const Image<T>& a, T b)
{
  Image<T> result; result.SetTemp(true);  a.DivCst(b,result); 
  result.CopyTrResp(a);  return result;
}

/*! \ingroup SkyMap \fn operator/(T,const Image<T>&)
  \brief Operator Image = constant / Image  */
template <class T> inline Image<T> operator / (T b, const Image<T>& a)
{
  Image<T> result; result.SetTemp(true);  a.DivCst(b, result, true); 
  result.CopyTrResp(a);  return result;
}

////////////////////////////////////////////////////////////////
// Surcharge d'operateurs B = -A

/*! \ingroup SkyMap \fn operator - (const Image<T>&)
  \brief Operator - Returns an image with pixels equal to the opposite of
  the original image pixels.  */
template <class T> inline Image<T> operator - (const Image<T>& a)
{
  Image<T> result; result.SetTemp(true);  a.NegateElt(result); 
  result.CopyTrResp(a);  return result;
}

////////////////////////////////////////////////////////////////
// Surcharge d'operateurs C = A (+,-,&&,/) B

/*! \ingroup SkyMap \fn operator+(const Image<T>&,const Image<T>&)
  \brief Operator Image = Image + Image (pixel by pixel sum) */
template <class T>
inline Image<T> operator + (const Image<T>& a,const Image<T>& b)
{ 
  Image<T> result; result.SetTemp(true); a.AddElt(b, result);    
  result.CopyTrResp(a);  return result;
}

/*! \ingroup SkyMap \fn operator-(const Image<T>&,const Image<T>&)
  \brief Operator Image = Image - Image (pixel by pixel difference) */
template <class T>
inline Image<T> operator - (const Image<T>& a,const Image<T>& b)
{ 
  Image<T> result; result.SetTemp(true);  a.SubElt(b, result);    
  result.CopyTrResp(a);  return result;
}

/*! \ingroup SkyMap \fn operator * (const Image<T>&,const Image<T>&)
  \brief Operator Image = Image * Image (pixel by pixel product) */

template <class T>
inline Image<T> operator * (const Image<T>& a,const Image<T>& b)
{ 
  Image<T> result; result.SetTemp(true);  a.MulElt(b, result);    
  result.CopyTrResp(a);  return result;
}

/*! \ingroup SkyMap \fn operator / (const Image<T>&,const Image<T>&)
  \brief Operator Image = Image / Image (pixel by pixel ratio) */
template <class T>
inline Image<T> operator / (const Image<T>& a,const Image<T>& b)
{ 
  Image<T> result; result.SetTemp(true);  a.DivElt(b, result, false, false);    
  result.CopyTrResp(a);  return result;
}

//! shorhand (typedef) for images with unsigned short integer pixels 
typedef Image<uint_2> ImageU2;
//! shorhand (typedef) for images with signed integer pixels 
typedef Image<int_4>  ImageI4;
//! shorhand (typedef) for images with single precision float pixels 
typedef Image<r_4>    ImageR4;
//! shorhand (typedef) for images with double precision float pixels 
typedef Image<r_8>    ImageR8;



//--------- extern template declarations (if needed) -----------
#if defined ( NEED_EXT_DECL_TEMP ) && !defined( CIMAGE_CC_BFILE )
extern template class Image<uint_1>;
extern template class Image<uint_2>;
extern template class Image<uint_4>;
extern template class Image<uint_8>;
extern template class Image<int_1>;
extern template class Image<int_2>;
extern template class Image<int_4>;
extern template class Image<int_8>;
extern template class Image<r_4>;
extern template class Image<r_8>;
extern template class Image< std::complex<r_4> >;
extern template class Image< std::complex<r_8> >;
#ifdef SO_LDBLE128
extern template class Image<r_16>;
extern template class Image< std::complex<r_16> >;
#endif
#endif  // Fin de if defined ( NEED_EXT_DECL_TEMP )

}  // End of namespace SOPHYA

#endif


