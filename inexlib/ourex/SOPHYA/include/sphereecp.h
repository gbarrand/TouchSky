//------------------------------------------------------------------------------------
// class SphereECP<T>
//   Classe de carte spherique avec decoupage theta-phi 
//    (projection cylindique: Equatorial Cylindrical Projection)   
// SOPHYA class library - (C)  UPS+LAL IN2P3/CNRS , (C) CEA-Irfu-SPP
//      R. Ansari  , Univ. Paris-Sud, LAL-IN2P3/CNRS       2004 
//------------------------------------------------------------------------------------

#ifndef SPHEREECP_SEEN
#define SPHEREECP_SEEN

#include "sphericalmap.h"
#include "sphecpbase.h"
#include "tarray.h"


/* Classe de carte spherique avec decoupage theta-phi 
  (projection cylindique: Equatorial Cylindrical Projection) 
  avec possibilite de couverture partielle, limitees en theta,phi
  R. Ansari  - Septembre 2004 
 */

namespace SOPHYA {
template <class T>
class FIO_SphereECP;  

template <class T> 
class SphereECP : public SphericalMap<T> , public SphECPBase
{
public :

//! return the size index value corresponding to resolution res (in radian)
static inline int_4 ResolToSizeIndex(double res)
  { return (int_4)((M_PI/res)+0.5); }
//! return the pixel resolution (in radian) for the size index \b m 
static inline double  SizeIndexToResol(int_4 m) 
  { return  (M_PI/(double)m); }

  //! default constructor : not allocated map 
  SphereECP();
  //! Constructor: full (4 pi) coverage, m slices in theta over pi (rings) -  2m in phi over 2 pi
  SphereECP(int m);
  //! Constructor: full (4 pi) coverage, ntet slices in theta over pi (rings), nphi in phi (delta phi = 2Pi)
  SphereECP(int ntet, int nphi);
  // Constructeur , couverture partielle, ntet tranches en theta , nphi en phi 
  //! Constructor: partial coverage
  SphereECP(r_8 tet1, r_8 tet2, int ntet, r_8 phi1, r_8 phi2, int nphi);
  //! Copy constructor: share=true -> sharing of the pixel array, clone otherwise
  SphereECP(const SphereECP<T>& a, bool share);
  //! Copy constructor: shares the pixel array 
  SphereECP(const SphereECP<T>& a);
  //! destructor 
  virtual  ~SphereECP();

  // Gestion taille/Remplissage
  virtual void Clone(const SphereECP<T>& a); 
  // partage les donnees si "a" temporaire, clone sinon.
  void CloneOrShare(const SphereECP<T>& a);
  // Share: partage les donnees de "a"
  void Share(const SphereECP<T>& a);

  //--- Copy operation 
  //! Copy SphereECP content from object \b a 
  virtual SphereECP<T>& Set(const SphereECP<T>& a); 
  //! Copy/assignment: operator notation for Set(const SphereECP<T>& a)
  inline  SphereECP<T>& operator = (const SphereECP<T>& a) { return Set(a); }

  // Extraction de carte partielle d'une carte ECP complete
  virtual SphereECP<T> ExtractPartial(r_8 tet1, r_8 tet2, r_8 phi1, r_8 phi2);


// Renvoie Type ECP 
  virtual std::string	TypeOfMap() const;  


  inline int_4 Index(int_4 ktheta, int_4 jphi)  { return( ktheta*_pixels.SizeX()+jphi);  }
// Interface de Sphericalmap
/*!    Return total number of pixels  */
  virtual int_4		NbPixels() const;

/* retourne la valeur du pixel d'indice k */ 
/*!    Return value of pixel with index k */
  virtual T&		PixVal(int_4 k);
  virtual T     	PixVal(int_4 k) const;

/* Return true if teta,phi in map  */
  virtual bool		ContainsSph(double theta, double phi) const;
/* retourne l'indice du pixel a (theta,phi) */ 
/*    Return index of the pixel corresponding to direction (theta, phi). */
  virtual int_4		PixIndexSph(double theta, double phi) const;

/* retourne les coordonnees Spheriques du centre du pixel d'indice k */ 
/*!   Return (theta,phi) coordinates of middle of  pixel with  index k */
  virtual void		PixThetaPhi(int_4 k, double& theta, double& phi) const;

/* retourne l'angle Solide de Pixel   (steradians) */ 
//! return the solid angle covered by pixel \b k , in steradians 
  virtual double	PixSolAngle(int_4 k=0) const;


/* retourne/fixe la valeur du parametre de decoupage m */ 
  virtual int_4		SizeIndex() const ;

// Interface de SphericalMap 
// index characterizing the size pixelization : m for SphereThetaPhi
// nside for Gorski sphere...
  virtual void		Resize(int_4 m);
  virtual uint_4	NbThetaSlices() const;

  // Nb de tranches en Phi
  inline  uint_4	NbPhiSlices() const { return _pixels.SizeX(); }

  virtual r_8           ThetaOfSlice(int_4 index) const;
  virtual void          GetThetaSliceInfo(int_4 index, r_8& theta, r_8& phi0, int_4& nphi, T* & ptr);

  virtual void		GetThetaSlice(int_4 index,r_8& theta, 
				      TVector<r_8>& phi, TVector<T>& value) const ; 
  virtual void		GetThetaSlice(int_4 sliceIndex, r_8& theta, r_8& phi0, 
				      TVector<int_4>& pixelIndices,TVector<T>& value) const ;
  virtual T*            GetThetaSliceDataPtr(int_4 index);

  // Valeur de pixel hors carte
  inline void		SetOutOfMapValue(T v) { _outofmappix = _outofmapval = v; }
  inline T		GetOutOfMapValue(T v) { return _outofmapval; }

  // Impression 
  virtual void          Show(std::ostream& os) const;
  inline void		Show() const { Show(std::cout); }
  virtual void          Print(std::ostream& os) const;
  inline void		print(std::ostream& os) const { Print(os); }
  inline void		Print() const { Print(std::cout); }

//---------- operator overload to access pixel values 

/*! \brief operator [] overloading : access to pixel \b idx 
  \warning no check is performed , 0 <= idx < NbPixels()  */
  inline T& operator[](int_4 idx) { return _pixels.DataBlock()(idx); }
/*! \brief operator [] overloading : access to pixel \b idx (const version)
  \warning no check is performed , 0 <= idx < NbPixels()  */
  inline T  operator[](int_4 idx) const { return _pixels.DataBlock()(idx); }

/*! \brief operator () overloading : access to the pixel in the direction defined by \b ll  */
  inline T& operator()(LongitudeLatitude const& ll) { return _pixels.DataBlock()(getIndex(ll)); };
/*! \brief operator () overloading (const version) : access to the pixel in the direction defined by \b ll  */
  inline T  operator()(LongitudeLatitude const& ll) const { return _pixels.DataBlock()(getIndex(ll)); };
/*! \brief operator () overloading : access to the pixel in the direction defined by \b (theta,phi)  */
  inline T& operator()(double theta,double phi) { return _pixels.DataBlock()(getIndex(LongitudeLatitude(theta,phi))); };
/*! \brief operator () overloading (const version) : access to the pixel in the direction defined by \b (theta,phi)  */
  inline T  operator()(double theta,double phi) const { return _pixels.DataBlock()(getIndex(LongitudeLatitude(theta,phi))); };

  // ---- les operations =, +, - , *
  //! Fill a SphereThetaPhi map pixels with a constant value \b a
  virtual SphereECP<T>& SetCst(T a); 
  //! Operator notation for SetCst(T a)
  inline  SphereECP<T>& operator = (T a) { return SetCst(a); }
  //! Alias for SetCst() : Fill a SphereECP map pixels with a constant value \b a
  inline SphereECP<T>& Set(T a) { return SetCst(a); } 
  
  //! Add the constant \b a to all map pixels
  virtual SphereECP<T>& AddCst(T a); 
  //! Add the constant \b a to all map pixels (operator += )
  inline  SphereECP<T>&  operator += (T x)  { return AddCst(x); }
  //! Substract the constant \b a  from all map pixels
  virtual SphereECP<T>& SubCst(T a, bool fginv=false); 
  //! Substract the constant \b a from all map pixels (operator -= )
  inline   SphereECP<T>&  operator -= (T x)  { return SubCst(x); }
  //! Multiply all map pixels by the constant \b a
  virtual SphereECP<T>& MulCst(T a); 
  //! Multiply all map pixels by the constant \b a (operator *= )
  inline  SphereECP<T>&  operator *= (T x)  { return MulCst(x); }
  //! Divide all map pixels by the constant \b a
  virtual SphereECP<T>& DivCst(T a, bool fginv=false); 
  //! Divide all map pixels by the constant \b a (operator *= )
  inline  SphereECP<T>&  operator /= (T x)  { return DivCst(x); }
  
  // A += -=  (ajoute, soustrait element par element les deux spheres )
  
  //! In place addition (pixel wise) of two SphereECP (*this) += a
  virtual SphereECP<T>&  AddElt(const SphereECP<T>& a);
  //! Pixel wise in place addition operator of two SphereECP (*this) += a  (method AddElt() )
  inline  SphereECP<T>&  operator += (const SphereECP<T>& a)  { return AddElt(a); }
  //! In place subtraction (pixel wise) of two SphereECP (*this) -= a
  virtual SphereECP<T>&  SubElt(const SphereECP<T>& a);
  //! Pixel wise in place subtraction operator of two SphereECP (*this) -= a (method SubElt() )
  inline  SphereECP<T>&  operator -= (const SphereECP<T>& a)  { return SubElt(a); }
  //! In place product (pixel wise) of two SphereECP (*this) *= a
  virtual SphereECP<T>&  MulElt(const SphereECP<T>& a);
  //! Pixel wise in place multiplication operator of two SphereECP (*this) *= a  (method MulElt())
  inline  SphereECP<T>&  operator *= (const SphereECP<T>& a)  { return MulElt(a); }
  //! In place division (pixel wise) of two SphereECP (*this) /= a
  virtual SphereECP<T>&  DivElt(const SphereECP<T>& a);
  //! Pixel wise in place multiplication operator of two SphereECP (*this) *= a  (method DivElt())
  inline  SphereECP<T>&  operator /= (const SphereECP<T>& a)  { return DivElt(a); }
  
//! return the sum of all map pixel values 
  virtual T Sum() const  
  { return _pixels.Sum(); }
//! return the product of all map pixel values 
  virtual T Product() const 
  { return _pixels.Product(); }
//! return the product of all map pixel (p) values-squared (Sum[ p^2 ])  
  virtual T SumSq() const 
  { return _pixels.SumSq(); }
//! return the minumum and maximum of all map pixel values (can not be applied to maps with complex data) 
  virtual void MinMax(T& min, T& max) const
  { return _pixels.MinMax(min,max); }

  // Acces au tableau des pixels
  inline TArray<T>&	GetPixelArray() { return _pixels; }
  inline TArray<T>	GetPixelArray() const { return _pixels; }

  //--- Changement et acces a l'unite physique associee
  virtual void SetUnits(const Units& un)    { _pixels.SetUnits(un); } 
  virtual Units GetUnits(bool& sdone) const  { return _pixels.GetUnits(sdone); } 

  //! assign a new object Id (or DataRef Id) - useful for PPF write operations
  inline void RenewObjId() { _pixels.RenewObjId(); }

  friend class  FIO_SphereECP<T>;   // Gestion de persistance PPF

  //! temporary map management - used to avoid non necessary allocation when using arithmetic operators 
  inline void SetTemp(bool fgtemp=false) const  // A supprimer 
  { _pixels.SetTemp(fgtemp);  }
  //! temporary map management - used to avoid non necessary allocation when using arithmetic operators 
  inline bool   IsTemp(void) const { return _pixels.IsTemp(); }
protected:
  //! copy pixelisation geometry and Info object 
  inline void  CopyGeometry(const SphereECP<T>& a)
  { 
    SphECPBase::copyFrom(a);  
    _outofmapidx=a._outofmapidx;   _outofmapnphi=a._outofmapnphi;    _outofmapntet=a._outofmapntet;
    _outofmappix=a._outofmappix;   _outofmapval=a._outofmapval;
    SphericalMap<T>::UpdateInfo(a);  //JEC inheritance of Template method
  }

  //----- data members 
  TArray<T> _pixels;
  int_4 _outofmapidx;
  int_4 _outofmapnphi;
  int_4 _outofmapntet;
  T _outofmappix;
  T _outofmapval;
};

// Surcharge d'operateurs A (+,-,*,/) (T) x
/*! \ingroup SkyMap \fn operator+(const SphereECP<T>&,T)
  \brief Operator SphereECP = SphereECP + constant */
template <class T> inline SphereECP<T> operator + (const SphereECP<T>& a, T b)
    {SphereECP<T> result; result.CloneOrShare(a); result.SetTemp(true); 
    result.AddCst(b); return result;}
/*! \ingroup SkyMap \fn operator+(T,const SphereECP<T>&)
  \brief Operator SphereECP = constant + SphereECP */
template <class T> inline SphereECP<T> operator + (T b,const SphereECP<T>& a)
    {SphereECP<T> result; result.CloneOrShare(a); result.SetTemp(true); 
    result.AddCst(b); return result;}


/*! \ingroup SphereECP\fn operator-(const SphereECP<T>&,T)
  \brief Operator SphereECP = SphereECP - constant */
template <class T> inline SphereECP<T> operator - (const SphereECP<T>& a, T b)
    {SphereECP<T> result; result.CloneOrShare(a); result.SetTemp(true); 
    result.SubCst(b); return result;}

/*! \ingroup  \fn operator-(T,const SphereECP<T>&)
  \brief Operator SphereECP = constant - SphereECP */
template <class T> inline SphereECP<T> operator - (T b,const SphereECP<T>& a)
    {SphereECP<T> result; result.CloneOrShare(a); result.SetTemp(true); 
    result.SubCst(b,true); return result;}

/*! \ingroup SkyMap \fn operator*(const SphereECP<T>&,T)
  \brief Operator SphereECP = SphereECP * constant */
template <class T> inline SphereECP<T> operator * (const SphereECP<T>& a, T b)
    {SphereECP<T> result; result.CloneOrShare(a); result.SetTemp(true); 
    result.MulCst(b); return result;}

/*! \ingroup SkyMap \fn operator*(T,const SphereECP<T>&)
  \brief Operator SphereECP = constant * SphereECP */
template <class T> inline SphereECP<T> operator * (T b,const SphereECP<T>& a)
    {SphereECP<T> result; result.CloneOrShare(a); result.SetTemp(true); 
    result.MulCst(b); return result;}

/*! \ingroup SkyMap \fn operator/(const SphereECP<T>&,T)
  \brief Operator SphereECP = SphereECP / constant */
template <class T> inline SphereECP<T> operator / (const SphereECP<T>& a, T b)
    {SphereECP<T> result; result.CloneOrShare(a); result.SetTemp(true); 
    result.DivCst(b); return result;}

/*! \ingroup SkyMap \fn operator/(T,const SphereECP<T>&)
  \brief Operator SphereECP = constant / SphereECP  */
template <class T> inline SphereECP<T> operator / (T b, const SphereECP<T>& a)
    {SphereECP<T> result; result.CloneOrShare(a); result.SetTemp(true); 
    result.DivCst(b, true); return result;}

////////////////////////////////////////////////////////////////
// Surcharge d'operateurs C = A (+,-) B

/*! \ingroup SkyMap \fn operator+(const SphereECP<T>&,const SphereECP<T>&)
  \brief Operator SphereECP = SphereECP + SphereECP */
template <class T>
inline SphereECP<T> operator + (const SphereECP<T>& a,const SphereECP<T>& b)
    { SphereECP<T> result; result.SetTemp(true); 
    if (b.IsTemp())  { result.Share(b); result.AddElt(a); }
    else { result.CloneOrShare(a); result.AddElt(b); }
    return result; }

/*! \ingroup SkyMap \fn operator-(const SphereECP<T>&,const SphereECP<T>&)
  \brief Operator SphereECP = SphereECP - SphereECP */
template <class T>
inline SphereECP<T> operator - (const SphereECP<T>& a,const SphereECP<T>& b)
    { SphereECP<T> result; result.SetTemp(true); 
    result.CloneOrShare(a); result.SubElt(b); 
    return result; }

////////////////////////////////////////////////////////////////
// Surcharge d'operateurs C = A (*,/) B

/*! \ingroup SkyMap \fn operator*(const SphereECP<T>&,const SphereECP<T>&)
  \brief Operator SphereECP = SphereECP * SphereECP (pixel by pixel multiply)*/
template <class T>
inline SphereECP<T> operator * (const SphereECP<T>& a,const SphereECP<T>& b)
    { SphereECP<T> result; result.SetTemp(true); 
    if (b.IsTemp())  { result.Share(b); result.MulElt(a); }
    else { result.CloneOrShare(a); result.MulElt(b); }
    return result; }

/*! \ingroup SkyMap \fn operator/(const SphereECP<T>&,const SphereECP<T>&)
  \brief Operator SphereECP = SphereECP / SphereECP (pixel by pixel divide) */
template <class T>
inline SphereECP<T> operator / (const SphereECP<T>& a,const SphereECP<T>& b)
    { SphereECP<T> result; result.SetTemp(true); 
    result.CloneOrShare(a); result.DivElt(b); 
    return result; }

//--------- extern template declarations (if needed) -----------
#if defined ( NEED_EXT_DECL_TEMP ) && !defined( SPHEREECP_CC_BFILE )
extern template class SphereECP<uint_1>;
extern template class SphereECP<uint_2>;
extern template class SphereECP<int_1>;
extern template class SphereECP<int_2>;
extern template class SphereECP<int_4>;
extern template class SphereECP<int_8>;
extern template class SphereECP<r_4>;
extern template class SphereECP<r_8>;
extern template class SphereECP< std::complex<r_4> >;
extern template class SphereECP< std::complex<r_8> >;
#ifdef SO_LDBLE128
extern template class SphereECP<r_16>;
extern template class SphereECP< std::complex<r_16> >;
#endif
#endif  // Fin de if defined ( NEED_EXT_DECL_TEMP )


}// Fin du namespace

#endif
