/* ------------------------------------------------------------------------------------
   SOPHYA class library - (C)  UPS+LAL IN2P3/CNRS  , (C) CEA-Irfu-SPP 
   Spherical map in HEALPix pixelisation    
   G. Le Meur, F. Touze   2000
   Updated: R. Ansari 2015  
   ------------------------------------------------------------------------------------  */
#ifndef SPHEREHEALPIX_SEEN
#define SPHEREHEALPIX_SEEN

#include "sphericalmap.h"
#include "ndatablock.h"
#include "tvector.h"

#include "shealpixbase.h"

namespace SOPHYA {

//------------------ CLASSE SphereHEALPix<T> -------------------------------------

//  forward declaration for PPF/Fits handler classes 
template<class T>
class FIO_SphereHEALPix;

template<class T>
class FITS_SphereHEALPix;

template<class T>
class SphereHEALPix : public SphericalMap<T> , public HEALPixBase
{
public :
  //! return the size index (=nside) corresponding to resolution res (in radian)
  static inline int_4 ResolToSizeIndex(double res)
         { return  HEALPixUtils::ResolToSizeIndex(res); }
  //! return the size index (=nside) corresponding to resolution res (in radian)
  static inline int_4 ResolToNSide(double res)
  { return  HEALPixUtils::ResolToSizeIndex(res); }
  //! return the pixel resolution (in radian) for the size index (=nside) m 
  static inline double  SizeIndexToResol(int_4 m) 
  { return  HEALPixUtils::SizeIndexToResol(m); }
  //! return the pixel resolution (in radian) for the size index (=nside) m 
  static inline double  NSideToResol(int_4 m) 
  { return  HEALPixUtils::SizeIndexToResol(m); }

  //----- Constructeurs 
  SphereHEALPix(bool fgring=true);
  SphereHEALPix(int_4 m, bool fgring=true);
  SphereHEALPix(const SphereHEALPix<T>& s, bool share);
  SphereHEALPix(const SphereHEALPix<T>& s);
  //! destructor 
  virtual ~SphereHEALPix();


//--- (re)definition of HEALPixBase methods 
  virtual void setNSide(int_4 nside, bool fgring);
  //! set of changes the pixelisation resolution by defining the HEALPix \b nside parameter
  inline  void setNSide(int_4 nside)  
  { setNSide(nside, fgring_); }

// ------------------ Definition of SphericalMap abstract methods

//! return the total mumber of pixels in the map 
  virtual int_4 NbPixels() const;

//! access to the pixel number \b k (return a reference to the pixel content) 
  virtual T& PixVal(int_4 k);
//! access to the pixel number \b k - const version - (return a const reference to the pixel content) 
  virtual T  PixVal(int_4 k) const;
  
  virtual uint_4 NbThetaSlices() const;
  virtual r_8  ThetaOfSlice(int_4 index) const;
  virtual bool  HasSymThetaSlice() const;
  virtual int_4 GetSymThetaSliceIndex(int_4 idx) const;
  
  virtual void GetThetaSlice(int_4 index,r_8& theta,TVector<r_8>& phi,TVector<T>& value) const;
  virtual void GetThetaSlice(int_4 sliceIndex,r_8& theta, r_8& phi0, TVector<int_4>& pixelIndices,TVector<T>& value) const ;
  virtual T*   GetThetaSliceDataPtr(int_4 sliceIndex);
  
  virtual bool ContainsSph(double theta, double phi) const;
  virtual int_4 PixIndexSph(double theta,double phi) const;
  
  virtual void PixThetaPhi(int_4 k,double& theta,double& phi) const;
  
/*! \brief Pixel Solid angle  (steradians)

    All the pixels have the same solid angle. The dummy argument is
    for compatibility with eventual pixelizations which would not 
   fulfil this requirement.
*/
  virtual double PixSolAngle(int_4 dummy=0) const {return omeg_;}
  
//--- Changement et acces a l'unite physique associee
//! Define or change the physical unit of the data.
  virtual void SetUnits(const Units& un)  
  { pixels_.SetUnits(un); } 
/*!  \brief Return the physical unit of the data. 
  Flag sdone is set to true if SetUnits() has been called.  */
   virtual Units GetUnits(bool& sdone) const
  { return pixels_.GetUnits(sdone); } 

// --------------- Specific methods 

//! define map resolution/size (number of pixels) according to healpix \b nside parameter
  virtual void Resize(int_4 nside);
//! return RING or NESTED 
  virtual std::string TypeOfMap() const;
//! return true if map in RING pixelisation scheme - false otherwise 
  inline bool IfRING() const { return fgring_; } 
//! return true if map in NESTED pixelisation scheme - false otherwise 
  inline bool IfNESTED() const { return ( (fgring_) ? false : true ); } 

/*! \return value of healpix nside */
  inline virtual int_4 SizeIndex() const {return(nSide_);}

//! prints map characteristics and pixel values on stream \b os 
  void print(std::ostream& os) const;
//! prints map characteristics and pixel values on stream \b os (alias for print() )
  inline void Print(std::ostream& os) const { print(os); }

//---------- operator overload to access pixel values 

/*! \brief operator [] overloading : access to pixel \b idx 
  \warning no check is performed , 0 <= idx < NbPixels()  */
  inline T& operator[](int_4 idx) { return pixels_(idx); }
/*! \brief operator [] overloading : access to pixel \b idx (const version)
  \warning no check is performed , 0 <= idx < NbPixels()  */
  inline T  operator[](int_4 idx) const { return pixels_(idx); }

/*! \brief operator () overloading : access to the pixel in the direction defined by \b ll  */
  inline T& operator()(LongitudeLatitude const& ll) { return pixels_(getIndex(ll)); };
/*! \brief operator () overloading (const version) : access to the pixel in the direction defined by \b ll  */
  inline T  operator()(LongitudeLatitude const& ll) const { return pixels_(getIndex(ll)); };
/*! \brief operator () overloading : access to the pixel in the direction defined by \b (theta,phi)  */
  inline T& operator()(double theta,double phi) { return pixels_(getIndex(LongitudeLatitude(theta,phi))); };
/*! \brief operator () overloading (const version) : access to the pixel in the direction defined by \b (theta,phi)  */
  inline T  operator()(double theta,double phi) const { return pixels_(getIndex(LongitudeLatitude(theta,phi))); };

//--------------------- Operations diverses  = , +=, ...
//! map content copy - can handle copy between SphereHEALPix maps with different schemes (RING<>NESTED)
  SphereHEALPix<T>& Set(const SphereHEALPix<T>& a);
//! copy operator : (*this)=a - call the copy method Set()
  inline  SphereHEALPix<T>& operator = (const SphereHEALPix<T>& a) 
                                                        {return Set(a);}
// A += -= *= /= x (ajoute, soustrait, ... x a tous les elements)

//! set all map pixels to the value \b v
  virtual T SetPixels(T v); 
//! copy (=) operator - sets all map pixels to value \b v
  inline  SphereHEALPix<T>& operator = (T v) { SetPixels(v);  return(*this); }

//! Add the constant \b a to all map pixels
  virtual SphereHEALPix<T>& AddCst(T a); 
//! Add the constant \b a to all map pixels (operator +=  method AddCst() )
  inline  SphereHEALPix<T>&  operator += (T x)  { return AddCst(x); }
//! Substract the constant \b a  from all map pixels
  virtual SphereHEALPix<T>& SubCst(T a, bool fginv=false); 
//! Substract the constant \b a from all map pixels (operator -=  method SubCst() )
  inline   SphereHEALPix<T>&  operator -= (T x)  { return SubCst(x); }
//! Multiply all map pixels by the constant \b a
  virtual SphereHEALPix<T>& MulCst(T a); 
//! Multiply all map pixels by the constant \b a (operator *=   method MulCst() )
  inline  SphereHEALPix<T>&  operator *= (T x)  { return MulCst(x); }
//! Divide all map pixels by the constant \b a
  virtual SphereHEALPix<T>& DivCst(T a, bool fginv=false); 
//! Divide all map pixels by the constant \b a (operator /=  method DivCst() )
  inline  SphereHEALPix<T>&  operator /= (T x)  { return DivCst(x); }

//--------  (*this) += -=  *= /= (map)  : pixel by pixel operations 
  //! In place addition (pixel wise) of two SphereHEALPix (*this) += a
  virtual SphereHEALPix<T>&  AddElt(const SphereHEALPix<T>& a);
  //! Pixel wise in place addition operator of two SphereHEALPix (*this) += a  (method AddElt() )
  inline  SphereHEALPix<T>&  operator += (const SphereHEALPix<T>& a)  { return AddElt(a); }
  //! In place subtraction (pixel wise) of two SphereHEALPix (*this) -= a
  virtual SphereHEALPix<T>&  SubElt(const SphereHEALPix<T>& a, bool fginv=false);
  //! Pixel wise in place subtraction operator of two SphereHEALPix (*this) -= a (method SubElt() )
  inline  SphereHEALPix<T>&  operator -= (const SphereHEALPix<T>& a)  { return SubElt(a); }
  //! In place product (pixel wise) of two SphereHEALPix (*this) *= a
  virtual SphereHEALPix<T>&  MulElt(const SphereHEALPix<T>& a);
  //! Pixel wise in place multiplication operator of two SphereHEALPix (*this) *= a  (method MulElt() )
  inline  SphereHEALPix<T>&  operator *= (const SphereHEALPix<T>& a)  { return MulElt(a); }
  //! In place division (pixel wise) of two SphereHEALPix (*this) /= a
  virtual SphereHEALPix<T>&  DivElt(const SphereHEALPix<T>& a, bool fginv=false); 
  //! Pixel wise in place multiplication operator of two SphereHEALPix (*this) /= a  (method DivElt() )
  inline  SphereHEALPix<T>&  operator /= (const SphereHEALPix<T>& a)  { return DivElt(a); }

//! return the sum of all map pixel values 
  virtual T Sum() const  
  { return pixels_.Sum(); }
//! return the product of all map pixel values 
  virtual T Product() const 
  { return pixels_.Product(); }
//! return the product of all map pixel (p) values-squared (Sum[ p^2 ])  
  virtual T SumSq() const 
  { return pixels_.SumSq(); }
//! return the minumum and maximum of all map pixel values (can not be applied to maps with complex data) 
  virtual void MinMax(T& min, T& max) const
  { return pixels_.MinMax(min,max); }

/*  Acces to the DataBlock  */
//! Acces to the map pixels data NDataBlock<T> object
  inline       NDataBlock<T>& DataBlock()       { return pixels_; }
//! Acces to the map pixels data NDataBlock<T> object (const version)
  inline const NDataBlock<T>& DataBlock() const { return pixels_; }

  void CloneOrShare(const SphereHEALPix<T>& a);
  void Share(const SphereHEALPix<T>& a);
  SphereHEALPix<T>& CopyElt(const SphereHEALPix<T>& a);

//! assign a new object Id (or DataRef Id) - useful for PPF write operations
// Reza 02/2007 : Est-il suffisant de faire l'operation sur pixels_ ?
  inline void RenewObjId() { pixels_.RenewObjId(); }

  //! return true if data hold in temporary NDataBlock objects 
  inline  bool   IsTemp(void) const {

    if (sliceBeginIndex_.IsTemp() != pixels_.IsTemp() || sliceLenght_.IsTemp() != pixels_.IsTemp() )
      throw PException(" l'etat 'temporaire' de la spherehealpix est incoherent"); 
    return pixels_.IsTemp();
  }
  
  //! change the NDataBlock objects temporary state 
  inline  void SetTemp(bool temp=false) const 
  {
    pixels_.SetTemp(temp);
    sliceBeginIndex_.SetTemp(temp);
    sliceLenght_.SetTemp(temp);
  };

 // friend declaration for classes which handle persistence and FITS IO
 friend class FIO_SphereHEALPix<T>;
 friend class FITS_SphereHEALPix<T>;
       
protected :

// ------------- méthodes internes ----------------------
//! initiliaze the structures for access to the rings 
  void SetThetaSlices(); 

// ------------- variables internes -----------------------

  NDataBlock<T> pixels_;                 //!< map pixel data 
  NDataBlock<int_4> sliceBeginIndex_;    //!< the index of the first pixel in each ring  (valid only in RING mode) 
  NDataBlock<int_4> sliceLenght_;        //!< the number of pixels in each ring  (valid only in RING mode) 

};

////////////////////////////////////////////////////////////////
// Surcharge d'operateurs A (+,-,*,/) (T) x
/*! \ingroup SkyMap \fn operator+(const SphereHEALPix<T>&,T)
  \brief Operator SphereHEALPix = SphereHEALPix + constant */
template <class T> inline SphereHEALPix<T> operator + (const SphereHEALPix<T>& a, T b)
    {SphereHEALPix<T> result; result.CloneOrShare(a); result.SetTemp(true); 
    result.AddCst(b); return result;}
/*! \ingroup SkyMap \fn operator+(T,const SphereHEALPix<T>&)
  \brief Operator SphereHEALPix = constant + SphereHEALPix */
template <class T> inline SphereHEALPix<T> operator + (T b,const SphereHEALPix<T>& a)
    {SphereHEALPix<T> result; result.CloneOrShare(a); result.SetTemp(true); 
    result.AddCst(b); return result;}

/*! \ingroup SphereHEALPix\fn operator-(const SphereHEALPix<T>&,T)
  \brief Operator SphereHEALPix = SphereHEALPix - constant */
template <class T> inline SphereHEALPix<T> operator - (const SphereHEALPix<T>& a, T b)
    {SphereHEALPix<T> result; result.CloneOrShare(a); result.SetTemp(true); 
    result.SubCst(b); return result;}

/*! \ingroup  \fn operator-(T,const SphereHEALPix<T>&)
  \brief Operator SphereHEALPix = constant - SphereHEALPix */
template <class T> inline SphereHEALPix<T> operator - (T b,const SphereHEALPix<T>& a)
    {SphereHEALPix<T> result; result.CloneOrShare(a); result.SetTemp(true); 
    result.SubCst(b,true); return result;}

/*! \ingroup SkyMap \fn operator*(const SphereHEALPix<T>&,T)
  \brief Operator SphereHEALPix = SphereHEALPix * constant */
template <class T> inline SphereHEALPix<T> operator * (const SphereHEALPix<T>& a, T b)
    {SphereHEALPix<T> result; result.CloneOrShare(a); result.SetTemp(true); 
    result.MulCst(b); return result;}

/*! \ingroup SkyMap \fn operator*(T,const SphereHEALPix<T>&)
  \brief Operator SphereHEALPix = constant * SphereHEALPix */
template <class T> inline SphereHEALPix<T> operator * (T b,const SphereHEALPix<T>& a)
    {SphereHEALPix<T> result; result.CloneOrShare(a); result.SetTemp(true); 
    result.MulCst(b); return result;}

/*! \ingroup SkyMap \fn operator/(const SphereHEALPix<T>&,T)
  \brief Operator SphereHEALPix = SphereHEALPix / constant */
template <class T> inline SphereHEALPix<T> operator / (const SphereHEALPix<T>& a, T b)
    {SphereHEALPix<T> result; result.CloneOrShare(a); result.SetTemp(true); 
    result.DivCst(b); return result;}

/*! \ingroup SkyMap \fn operator/(T,const SphereHEALPix<T>&)
  \brief Operator SphereHEALPix = constant / SphereHEALPix  */
template <class T> inline SphereHEALPix<T> operator / (T b, const SphereHEALPix<T>& a)
    {SphereHEALPix<T> result; result.CloneOrShare(a); result.SetTemp(true); 
    result.DivCst(b, true); return result;}

////////////////////////////////////////////////////////////////
// Surcharge d'operateurs C = A (+,-) B

/*! \ingroup SkyMap \fn operator+(const SphereHEALPix<T>&,const SphereHEALPix<T>&)
  \brief Operator SphereHEALPix = SphereHEALPix + SphereHEALPix */
template <class T>
inline SphereHEALPix<T> operator + (const SphereHEALPix<T>& a,const SphereHEALPix<T>& b)
    { SphereHEALPix<T> result; result.SetTemp(true); 
    if (b.IsTemp())  { result.Share(b); result.AddElt(a); }
    else { result.CloneOrShare(a); result.AddElt(b); }
    return result; }

/*! \ingroup SkyMap \fn operator-(const SphereHEALPix<T>&,const SphereHEALPix<T>&)
  \brief Operator SphereHEALPix = SphereHEALPix - SphereHEALPix */
template <class T>
inline SphereHEALPix<T> operator - (const SphereHEALPix<T>& a,const SphereHEALPix<T>& b)
    { SphereHEALPix<T> result; result.SetTemp(true); 
    result.CloneOrShare(a); result.SubElt(b); 
    return result; }

////////////////////////////////////////////////////////////////
// Surcharge d'operateurs C = A (*,/) B

/*! \ingroup SkyMap \fn operator*(const SphereHEALPix<T>&,const SphereHEALPix<T>&)
  \brief Operator SphereHEALPix = SphereHEALPix * SphereHEALPix (pixel by pixel multiply) */
template <class T>
inline SphereHEALPix<T> operator * (const SphereHEALPix<T>& a,const SphereHEALPix<T>& b)
    { SphereHEALPix<T> result; result.SetTemp(true); 
    if (b.IsTemp())  { result.Share(b); result.MulElt(a); }
    else { result.CloneOrShare(a); result.MulElt(b); }
    return result; }

/*! \ingroup SkyMap \fn operator/(const SphereHEALPix<T>&,const SphereHEALPix<T>&)
  \brief Operator SphereHEALPix = SphereHEALPix / SphereHEALPix (pixel by pixel divide) */
template <class T>
inline SphereHEALPix<T> operator / (const SphereHEALPix<T>& a,const SphereHEALPix<T>& b)
    { SphereHEALPix<T> result; result.SetTemp(true); 
    result.CloneOrShare(a); result.DivElt(b); 
    return result; }

//--------- extern template declarations (if needed) -----------
#if defined ( NEED_EXT_DECL_TEMP ) && !defined( SPHEREHEALPIX_CC_BFILE )
extern template class SphereHEALPix<uint_1>;
extern template class SphereHEALPix<uint_2>;
extern template class SphereHEALPix<int_1>;
extern template class SphereHEALPix<int_2>;
extern template class SphereHEALPix<int_4>;
extern template class SphereHEALPix<int_8>;
extern template class SphereHEALPix<r_4>;
extern template class SphereHEALPix<r_8>;
extern template class SphereHEALPix< std::complex<r_4> >;
extern template class SphereHEALPix< std::complex<r_8> >;
#ifdef SO_LDBLE128
extern template class SphereHEALPix<r_16>;
extern template class SphereHEALPix< std::complex<r_16> >;
#endif
#endif  // Fin de if defined ( NEED_EXT_DECL_TEMP )

} // Fin du namespace

#endif
