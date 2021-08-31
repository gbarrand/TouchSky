/* ------------------------------------------------------------------------------------
   SOPHYA class library - (C)  UPS+LAL IN2P3/CNRS  , (C) CEA-Irfu-SPP 
   SphericalMap with Theta-Phi (IGLOO) pixelisation 
   R. Ansari 2000-2015
   Updated: 2015  (modified pixelisation scheme)
   ------------------------------------------------------------------------------------  */
#ifndef SPHERETHETAPHI_SEEN
#define SPHERETHETAPHI_SEEN

#include "sphericalmap.h"
#include "ndatablock.h"
#include "tvector.h"

#include "sphthetaphibase.h"


namespace SOPHYA {

template <class T>
class FIO_SphereThetaPhi;  

template<class T>
class FITS_SphereThetaPhi;


// ***************** Class SphereThetaPhi *****************************
template <class T>
class SphereThetaPhi : public SphericalMap<T> , public SphThetaPhiBase
{
public :
  //! return the size index value corresponding to resolution \b res (in radian)
  static inline int_4 ResolToSizeIndex(double res)
  { return SphThetaPhiBase::ResolutionToNTheta(res); }
  //! return the pixel resolution (in radian) for the size index \b m 
  static inline double  SizeIndexToResol(int_4 m) 
  { return SphThetaPhiBase::NThetaToResolution(m); }

  // constructor 
  explicit SphereThetaPhi(bool fgby3=true);
  // constructor with number of rings (from 0 to Pi)
  explicit SphereThetaPhi(int_4 nring, bool fgby3=true);
  // copy constructor with share/clone flag 
  SphereThetaPhi(const SphereThetaPhi<T>& s, bool share);
  // copy constructor 
  SphereThetaPhi(const SphereThetaPhi<T>& s);

  // destructor 
  virtual ~SphereThetaPhi();

  //! define or changes pixelisation resolution by specifying the number of rings (or slices) along theta 
  virtual  void setNbRings(int_4 nring); 
  //! define the pixelisation resolution 
  inline void setNbRings(int_4 nring, bool fgby3) 
  { FgBy3_=fgby3;  setNbRings(nring); }

  // map content copy operation 
  SphereThetaPhi<T>& Set(SphereThetaPhi<T> const & a);
  //! Copy operator: operator notation for Set(SphereThetaPhi<T> const & a)
  inline  SphereThetaPhi<T>& operator = (SphereThetaPhi<T> const & a)  { return Set(a); }

// ------------ Definition of SphericalMap abstract methods -
/* retourne le nombre de pixels */ 
  virtual int_4 NbPixels() const;

/* retourne la valeur du pixel d'indice k */ 
  virtual T&       PixVal(int_4 k);
  virtual T        PixVal(int_4 k) const;
  
/* Return true if teta,phi in map  */
  virtual bool ContainsSph(double theta, double phi) const;
/*    Return index of the pixel corresponding to direction (theta, phi). */
  virtual int_4 PixIndexSph(double theta, double phi) const;  
/* retourne les coordonnees Spheriques du centre du pixel d'indice k */ 
  virtual void  PixThetaPhi(int_4 k, double& theta, double& phi) const;

/* retourne/fixe l'angle Solide de Pixel   (steradians) */ 
  virtual double PixSolAngle(int_4 dummy=0) const;
 
/* retourne/fixe la valeur du parametre de decoupage m */ 
//! Return the pixelisation parameter (number of slices in a hemisphere)
  inline virtual int_4 SizeIndex() const { return( NTheta_); }

//--- Changement et acces a l'unite physique associee
//! Define or change the physical unit of the data.
  virtual void SetUnits(const Units& un)  
  { pixels_.SetUnits(un); } 
/*!  \brief Return the physical unit of the data. 
  Flag sdone is set to true if SetUnits() has been called.  */
  virtual Units GetUnits(bool& sdone) const
  { return pixels_.GetUnits(sdone); } 

// ------------- Specific methods  ----------------------
  virtual void Resize(int_4 m);

  inline virtual std::string TypeOfMap() const {return std::string("TETAFI");};

/*!   Return values of theta,phi which limit the pixel with  index \b idx 
  \warning  DEPRECATED - use getPixelBoundary()  */
  inline void Limits(int_4 k,double& th1,double& th2,double& phi1,double& phi2)
  {  return getPixelBoundary(k, th1, th2, phi1, phi2); }
/* Nombre de tranches en theta */ 
/*!    Return number of theta-slices on the sphere */
  virtual uint_4 NbThetaSlices() const;

/*! Nombre de pixels en phi de la tranche d'indice kt 
    \warning  DEPRECATED - use getRingNbPixels()   */ 
  inline int_4 NPhi(int_4 kt) const
  { return getRingNbPixels(kt); }

/*! Renvoie dans t1,t2 les valeurs respectives de theta min et theta max  
   de la tranche d'indice kt  
    \warning  DEPRECATED - use getRingBoundary()   */ 
  inline void Theta(int_4 kt, double& t1, double& t2) const
  { getRingBoundary(kt, t1, t2); return; }

/*! Renvoie dans p1,p2 les valeurs phimin et phimax du pixel d'indice jp  
   dans la tranche d'indice kt  
   \warning  DEPRECATED - use getThetaPhiBoundary()   */ 
  void Phi(int_4 kt, int_4 jp, double& p1, double& p2) const
  { double t1, t2;  return getThetaPhiBoundary(kt, jp, t1, t2, p1, p2); }

/*!   Return pixel index  with sequence index jp in the slice kt 
   \warning  DEPRECATED - use getIndex()   */ 
  int_4 Index(int_4 kt, int_4 jp) const
  { return getIndex(kt, jp); }

/*! Indice kt de la tranche et indice jp du pixel d'indice idx  
   \warning  DEPRECATED - use getThetaPhiIndex()   */ 
  void ThetaPhiIndex(int_4 idx,int_4& kt,int_4& jp)
  { return getThetaPhiIndex(idx, kt, jp); }

/*! return the theta of slice \b kt 
   \warning  DEPRECATED - use getRingTheta()   */ 
  virtual r_8  ThetaOfSlice(int_4 kt) const;
  virtual int_4 GetSymThetaSliceIndex(int_4 kt) const;
  virtual bool  HasSymThetaSlice() const;

  virtual void GetThetaSlice(int_4 index,r_8& theta,TVector<r_8>& phi,TVector<T>& value) const; 
  virtual void GetThetaSlice(int_4 index, r_8& theta, r_8& phi0,TVector<int_4>& pixelIndices, TVector<T>& value) const ;
  virtual T*   GetThetaSliceDataPtr(int_4 sliceIndex);


//! ASCII dump (print) of the pixel map on stream \b os
  void print(std::ostream& os) const;
//! ASCII dump (print) of the pixel map 
  inline void Print(std::ostream& os) const { print(os); }
//! ASCII dump (print) of the pixel map on cout
  inline void Print() const { print(std::cout); }

//---------- operator overload to access pixel values 

/*! \brief operator [] overloading : access to pixel \b idx 
  \warning no check is performed , 0 <= idx < NbPixels()  */
  inline T& operator[](int_4 idx) { return pixels_(idx); }
/*! \brief operator [] overloading : access to pixel \b idx (const version)
  \warning no check is performed , 0 <= idx < NbPixels()  */
  inline T  operator[](int_4 idx) const { return pixels_(idx); }

/*! \brief operator () overloading : access to the pixel in the direction defined by \b ll */
  inline T& operator()(LongitudeLatitude const& ll) { return pixels_(getIndex(ll)); };
/*! \brief operator () overloading (const version) : access to the pixel in the direction defined by \b ll */
  inline T  operator()(LongitudeLatitude const& ll) const { return pixels_(getIndex(ll)); };
/*! \brief operator () overloading : access to the pixel in the direction defined by \b (theta,phi)  */
  inline T& operator()(double theta,double phi) { return pixels_(getIndex(LongitudeLatitude(theta,phi))); };
/*! \brief operator () overloading (const version) : access to the pixel in the direction defined by \b (theta,phi)  */
  inline T  operator()(double theta,double phi) const { return pixels_(getIndex(LongitudeLatitude(theta,phi))); };

//---------------------------------------------------------------------
// A += -= *= /= x (ajoute, soustrait, ... x a tous les elements)

//! Fill a SphereThetaPhi map pixels with a constant value \b a
  virtual T SetPixels(T a); 
//! Operator notation for SetT(T a)
  inline  SphereThetaPhi<T>& operator = (T a) { SetPixels(a); return (*this); }

//! Add the constant \b a to all map pixels
  virtual SphereThetaPhi<T>& AddCst(T a); 
//! Add the constant \b a to all map pixels (operator +=  method AddCst() )
  inline  SphereThetaPhi<T>&  operator += (T x)  { return AddCst(x); }
//! Substract the constant \b a  from all map pixels
  virtual SphereThetaPhi<T>& SubCst(T a, bool fginv=false); 
//! Substract the constant \b a from all map pixels (operator -=  method SubCst() )
  inline   SphereThetaPhi<T>&  operator -= (T x)  { return SubCst(x); }
//! Multiply all map pixels by the constant \b a
  virtual SphereThetaPhi<T>& MulCst(T a); 
//! Multiply all map pixels by the constant \b a (operator *=  method MulCst() )
  inline  SphereThetaPhi<T>&  operator *= (T x)  { return MulCst(x); }
//! Divide all map pixels by the constant \b a
  virtual SphereThetaPhi<T>& DivCst(T a, bool fginv=false); 
//! Divide all map pixels by the constant \b a (operator /=  method DivCst() )
  inline  SphereThetaPhi<T>&  operator /= (T x)  { return DivCst(x); }

//--------  (*this) += -=  *= /= (map)  : pixel by pixel operations 
  //! In place addition (pixel wise) of two SphereThetaPhi (*this) += a
  virtual SphereThetaPhi<T>&  AddElt(const SphereThetaPhi<T>& a);
  //! Pixel wise in place addition operator of two SphereThetaPhi (*this) += a  (method AddElt() )
  inline  SphereThetaPhi<T>&  operator += (const SphereThetaPhi<T>& a)  { return AddElt(a); }
  //! In place subtraction (pixel wise) of two SphereThetaPhi (*this) -= a
  virtual SphereThetaPhi<T>&  SubElt(const SphereThetaPhi<T>& a, bool fginv=false);
  //! Pixel wise in place subtraction operator of two SphereThetaPhi (*this) -= a (method SubElt() )
  inline  SphereThetaPhi<T>&  operator -= (const SphereThetaPhi<T>& a)  { return SubElt(a); }
  //! In place product (pixel wise) of two SphereThetaPhi (*this) *= a
  virtual SphereThetaPhi<T>&  MulElt(const SphereThetaPhi<T>& a);
  //! Pixel wise in place multiplication operator of two SphereThetaPhi (*this) *= a  (method MulElt() )
  inline  SphereThetaPhi<T>&  operator *= (const SphereThetaPhi<T>& a)  { return MulElt(a); }
  //! In place division (pixel wise) of two SphereThetaPhi (*this) /= a
  virtual SphereThetaPhi<T>&  DivElt(const SphereThetaPhi<T>& a, bool fginv=false);
  //! Pixel wise in place multiplication operator of two SphereThetaPhi (*this) /= a  (method DivElt() )
  inline  SphereThetaPhi<T>&  operator /= (const SphereThetaPhi<T>& a)  { return DivElt(a); }

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
  inline       NDataBlock<T>& DataBlock()       { return pixels_;}
//! Acces to the map pixels data NDataBlock<T> object (const version)
  inline const NDataBlock<T>& DataBlock() const { return pixels_;}

  void CloneOrShare(const SphereThetaPhi<T>& a);
  void Share(const SphereThetaPhi<T>& a);

  SphereThetaPhi<T>& CopyElt(const SphereThetaPhi<T>& a);

//! assign a new object Id (or DataRef Id) - useful for PPF write operations
// Reza 02/2007 : Est-il suffisant de faire l'operation sur pixels_ ?
  inline void RenewObjId() { pixels_.RenewObjId(); }


  // Temporaire?
  inline  bool IsTemp(void) const 
  {  return pixels_.IsTemp(); }
/*! Setting blockdata to temporary (see ndatablock documentation) */
  inline  void SetTemp(bool temp=false) const 
  {  pixels_.SetTemp(temp); }
  
 // friend declaration for classes which handle persistence and FITS IO
  friend class FIO_SphereThetaPhi<T>;
  friend class FITS_SphereThetaPhi<T>;

protected :
// ------------- variables membres ---------------------
 NDataBlock<T> pixels_;  //!< map contents (array of pixels) 
};

////////////////////////////////////////////////////////////////
// Surcharge d'operateurs A (+,-,*,/) (T) x
/*! \ingroup SkyMap \fn operator+(const SphereThetaPhi<T>&,T)
  \brief Operator SphereThetaPhi = SphereThetaPhi + constant */
template <class T> inline SphereThetaPhi<T> operator + (const SphereThetaPhi<T>& a, T b)
    {SphereThetaPhi<T> result; result.CloneOrShare(a); result.SetTemp(true); 
    result.AddCst(b); return result;}
/*! \ingroup SkyMap \fn operator+(T,const SphereThetaPhi<T>&)
  \brief Operator SphereThetaPhi = constant + SphereThetaPhi */
template <class T> inline SphereThetaPhi<T> operator + (T b,const SphereThetaPhi<T>& a)
    {SphereThetaPhi<T> result; result.CloneOrShare(a); result.SetTemp(true); 
    result.AddCst(b); return result;}


/*! \ingroup SphereThetaPhi\fn operator-(const SphereThetaPhi<T>&,T)
  \brief Operator SphereThetaPhi = SphereThetaPhi - constant */
template <class T> inline SphereThetaPhi<T> operator - (const SphereThetaPhi<T>& a, T b)
    {SphereThetaPhi<T> result; result.CloneOrShare(a); result.SetTemp(true); 
    result.SubCst(b); return result;}

/*! \ingroup  \fn operator-(T,const SphereThetaPhi<T>&)
  \brief Operator SphereThetaPhi = constant - SphereThetaPhi */
template <class T> inline SphereThetaPhi<T> operator - (T b,const SphereThetaPhi<T>& a)
    {SphereThetaPhi<T> result; result.CloneOrShare(a); result.SetTemp(true); 
    result.SubCst(b,true); return result;}

/*! \ingroup SkyMap \fn operator*(const SphereThetaPhi<T>&,T)
  \brief Operator SphereThetaPhi = SphereThetaPhi * constant */
template <class T> inline SphereThetaPhi<T> operator * (const SphereThetaPhi<T>& a, T b)
    {SphereThetaPhi<T> result; result.CloneOrShare(a); result.SetTemp(true); 
    result.MulCst(b); return result;}

/*! \ingroup SkyMap \fn operator*(T,const SphereThetaPhi<T>&)
  \brief Operator SphereThetaPhi = constant * SphereThetaPhi */
template <class T> inline SphereThetaPhi<T> operator * (T b,const SphereThetaPhi<T>& a)
    {SphereThetaPhi<T> result; result.CloneOrShare(a); result.SetTemp(true); 
    result.MulCst(b); return result;}

/*! \ingroup SkyMap \fn operator/(const SphereThetaPhi<T>&,T)
  \brief Operator SphereThetaPhi = SphereThetaPhi / constant */
template <class T> inline SphereThetaPhi<T> operator / (const SphereThetaPhi<T>& a, T b)
    {SphereThetaPhi<T> result; result.CloneOrShare(a); result.SetTemp(true); 
    result.DivCst(b); return result;}

/*! \ingroup SkyMap \fn operator/(T,const SphereThetaPhi<T>&)
  \brief Operator SphereThetaPhi = constant / SphereThetaPhi  */
template <class T> inline SphereThetaPhi<T> operator / (T b, const SphereThetaPhi<T>& a)
    {SphereThetaPhi<T> result; result.CloneOrShare(a); result.SetTemp(true); 
    result.DivCst(b, true); return result;}

////////////////////////////////////////////////////////////////
// Surcharge d'operateurs C = A (+,-) B

/*! \ingroup SkyMap \fn operator+(const SphereThetaPhi<T>&,const SphereThetaPhi<T>&)
  \brief Operator SphereThetaPhi = SphereThetaPhi + SphereThetaPhi */
template <class T>
inline SphereThetaPhi<T> operator + (const SphereThetaPhi<T>& a,const SphereThetaPhi<T>& b)
    { SphereThetaPhi<T> result; result.SetTemp(true); 
    if (b.IsTemp())  { result.Share(b); result.AddElt(a); }
    else { result.CloneOrShare(a); result.AddElt(b); }
    return result; }

/*! \ingroup SkyMap \fn operator-(const SphereThetaPhi<T>&,const SphereThetaPhi<T>&)
  \brief Operator SphereThetaPhi = SphereThetaPhi - SphereThetaPhi */
template <class T>
inline SphereThetaPhi<T> operator - (const SphereThetaPhi<T>& a,const SphereThetaPhi<T>& b)
    { SphereThetaPhi<T> result; result.SetTemp(true); 
    result.CloneOrShare(a); result.SubElt(b); 
    return result; }

////////////////////////////////////////////////////////////////
// Surcharge d'operateurs C = A (*,/) B

/*! \ingroup SkyMap \fn operator*(const SphereThetaPhi<T>&,const SphereThetaPhi<T>&)
  \brief Operator SphereThetaPhi = SphereThetaPhi * SphereThetaPhi (pixel by pixel multiply)*/
template <class T>
inline SphereThetaPhi<T> operator * (const SphereThetaPhi<T>& a,const SphereThetaPhi<T>& b)
    { SphereThetaPhi<T> result; result.SetTemp(true); 
    if (b.IsTemp())  { result.Share(b); result.MulElt(a); }
    else { result.CloneOrShare(a); result.MulElt(b); }
    return result; }

/*! \ingroup SkyMap \fn operator/(const SphereThetaPhi<T>&,const SphereThetaPhi<T>&)
  \brief Operator SphereThetaPhi = SphereThetaPhi / SphereThetaPhi (pixel by pixel divide) */
template <class T>
inline SphereThetaPhi<T> operator / (const SphereThetaPhi<T>& a,const SphereThetaPhi<T>& b)
    { SphereThetaPhi<T> result; result.SetTemp(true); 
    result.CloneOrShare(a); result.DivElt(b); 
    return result; }


//--------- extern template declarations (if needed) -----------
#if defined ( NEED_EXT_DECL_TEMP ) && !defined( SPHERETHETAPHI_CC_BFILE )
extern template class SphereThetaPhi<uint_1>;
extern template class SphereThetaPhi<uint_2>;
extern template class SphereThetaPhi<int_1>;
extern template class SphereThetaPhi<int_2>;
extern template class SphereThetaPhi<int_4>;
extern template class SphereThetaPhi<int_8>;
extern template class SphereThetaPhi<r_4>;
extern template class SphereThetaPhi<r_8>;
extern template class SphereThetaPhi< std::complex<r_4> >;
extern template class SphereThetaPhi< std::complex<r_8> >;
#ifdef SO_LDBLE128
extern template class SphereThetaPhi<r_16>;
extern template class SphereThetaPhi< std::complex<r_16> >;
#endif

#endif  // Fin de if defined ( NEED_EXT_DECL_TEMP )

} // Fin du namespace

#endif
