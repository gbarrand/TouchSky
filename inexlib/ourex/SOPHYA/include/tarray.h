//-----------------------------------------------------------
// Classe TArray<T>
//    Main template class for numerical arrays 
// SOPHYA class library - (C)  UPS+LAL IN2P3/CNRS  , CEA-Irfu 
// R. Ansari , C.Magneville       03/2000         
//-----------------------------------------------------------

//      template array class for numerical types
//                     R. Ansari, C.Magneville   03/2000

#ifndef TArray_SEEN
#define TArray_SEEN

#include "machdefs.h"
#include <math.h>
#include <iostream>
#include "basarr.h"
#include "ndatablock.h"
#include <complex>
#include "utilarr.h"


namespace SOPHYA {

// Forward declaration
template <class T> class FIO_TArray;

//   --------------------------- classe template Array -----------------------
//  ( See BaseArray class for data organisation in  memory and related methods )

//! Class for template arrays
template <class T>
class TArray : public BaseArray {
public:
  // Creation / destruction 
  TArray();
  TArray(int_4 ndim, const sa_size_t * siz, sa_size_t step =1, bool fzero=true);
  TArray(sa_size_t nx, sa_size_t ny=0, sa_size_t nz=0, sa_size_t nt=0, sa_size_t nu=0, bool fzero=true);
  TArray(int_4 ndim, const sa_size_t * siz, const NDataBlock<T> & db, bool share=false, sa_size_t step=1, sa_size_t offset=0);
  TArray(int_4 ndim, const sa_size_t * siz, T* values, sa_size_t step=1, sa_size_t offset=0, Bridge* br=NULL);
  TArray(const TArray<T>& a);
  TArray(const TArray<T>& a, bool share);
  TArray(const BaseArray& a, bool pack=true);

  virtual ~TArray();

  // A = B
  //! = operator between TArray
  /*! \warning Datas are copied (cloned) from \b a.
      \sa Set \sa NDataBlock::operator=(const NDataBlock<T>&) */
  inline  TArray<T>& operator = (const TArray<T>& a) { return Set(a); }
  virtual TArray<T>& Set(const TArray<T>& a);

  //! = operator between  TArray 's with different types - Elements are converted.
  inline  TArray<T>& operator = (const BaseArray& a) { return SetBA(a); }
  virtual TArray<T>& SetBA(const BaseArray& a);

  // Gestion taille/Remplissage
  virtual void Clone(const TArray<T>& a); 
  // partage les donnees si "a" temporaire, clone sinon.
  void CloneOrShare(const TArray<T>& a);
  // Share: partage les donnees de "a"
  void Share(const TArray<T>& a);

  void ReSize(int_4 ndim, sa_size_t * siz, sa_size_t step=1, bool fzero=true);
  void ReSize(const BaseArray& a, bool pack=false, bool fzero=true);
  //! a synonym (alias) for method ReSize(int_4, ...)
  inline void SetSize(int_4 ndim, sa_size_t * siz, sa_size_t step=1, bool fzero=true)
		{ ReSize(ndim, siz, step, fzero); }
  //! a synonym (alias) for method ReSize(const BaseArray&)
  inline void SetSize(const BaseArray& a, bool pack=false, bool fzero=true)
		{ ReSize(a, pack, fzero); }
  void Realloc(int_4 ndim, sa_size_t * siz, sa_size_t step=1, bool force=false);
  
  //! To resize the array, creating a one dimensional packed array ( nx  ) 
  void SetSize1D(sa_size_t nx, bool fzero=true);
  //! To resize the array, creating a two dimensional packed array ( nx * ny ) 
  void SetSize2D(sa_size_t nx, sa_size_t ny, bool fzero=true);
  //! To resize the array, creating a three dimensional packed array ( nx * ny * nz ) 
  void SetSize3D(sa_size_t nx, sa_size_t ny, sa_size_t nz, bool fzero=true);
  //! To resize the array, creating a four dimensional packed array ( nx * ny * nz * nt ) 
  void SetSize4D(sa_size_t nx, sa_size_t ny, sa_size_t nz, sa_size_t nt, bool fzero=true);
  //! To resize the array, creating a five dimensional packed array ( nx * ny * nz * nt * nu ) 
  void SetSize5D(sa_size_t nx, sa_size_t ny, sa_size_t nz, sa_size_t nt, sa_size_t nu, bool fzero=true);

  //! To clear the array sizes - corresponding to an unallocated array.
  virtual TArray<T>& ZeroSize();

  // Compacts size=1 array dimensions
  virtual TArray<T>& CompactAllDimensions();
  virtual TArray<T>& CompactTrailingDimensions();

  // Packing array elements in memory
  virtual TArray<T> PackElements(bool force=false) const ;

  // Return a reshaped array 
  virtual TArray<T> ReShape(sa_size_t nx, sa_size_t ny=0, sa_size_t nz=0, sa_size_t nt=0, sa_size_t nu=0, bool share=true);

  //--- Changement et acces a l'unite physique associee
  //! Define or change the physical unit of the data. Call the corresponding NDataBlock method. 
  inline void SetUnits(const Units& un)  
    { mNDBlock.SetUnits(un); } 
  /*!
    \brief Return the physical unit of the data. 
    Flag sdone is set to true if SetUnits() has been called. Call the corresponding NDataBlock method. 
  */
  inline Units GetUnits(bool& sdone) const
    { return mNDBlock.GetUnits(sdone); } 
  //! Return the physical unit of the data 
  inline Units GetUnits() const  
    {bool fg; return mNDBlock.GetUnits(fg); }

  // SubArrays - $CHECK$ Reza 03/2000 je ne sais pas s'il faut declarer ca const ??
  TArray<T> SubArray(Range rx, Range ry, Range rz, Range rt, Range ru, bool compact=true) const ;

  //! Extract the first 3D subarray specified by rx, ry, rz. (see SubArray() )
  inline TArray<T> operator () (Range rx, Range ry, Range rz) const
                   { return  SubArray(rx, ry, rz, Range::first(), Range::first()); }
  //! Extract the first 4D subarray specified by rx, ry, rz. (see SubArray() )
  inline TArray<T> operator () (Range rx, Range ry, Range rz, Range rt) const
                   { return  SubArray(rx, ry, rz, rt, Range::first()); }
  //! Extract the subarray specified by rx, ry, rz. (see SubArray() )
  inline TArray<T> operator () (Range rx, Range ry, Range rz, Range rt, Range ru) const
                   { return  SubArray(rx, ry, rz, rt, ru); }

  // Return a packed array, equal to the sub-array defined by the ranges - no data sharing 
  TArray<T> PackAndCopySubArray(Range rx, Range ry, Range rz=Range::first(), Range rt=Range::first(), Range ru=Range::first()) const;

  // ---- Access to data
  // Definition of virtual element acces method inherited from BaseArray class 
  virtual MuTyV & ValueAtPosition(sa_size_t ip) const;
  virtual MuTyV & ValueAtPositionDB(sa_size_t ip) const;

  // Data Access: operator overloaded inline acces methods 
  inline T const& operator()(sa_size_t ix) const ;
  inline T&       operator()(sa_size_t ix);
  inline T const& operator()(sa_size_t ix, sa_size_t iy) const ;
  inline T&       operator()(sa_size_t ix, sa_size_t iy);
  inline T const& operator()(sa_size_t ix, sa_size_t iy, sa_size_t iz) const ;
  inline T&       operator()(sa_size_t ix, sa_size_t iy, sa_size_t iz);
  inline T const& operator()(sa_size_t ix, sa_size_t iy, sa_size_t iz, sa_size_t it, sa_size_t iu=0) const ;
  inline T&       operator()(sa_size_t ix, sa_size_t iy, sa_size_t iz, sa_size_t it, sa_size_t iu=0);
  inline T const& operator[](sa_size_t ip) const ;
  inline T&       operator[](sa_size_t ip);

  inline T const& Elem(sa_size_t ix, sa_size_t iy, sa_size_t iz, sa_size_t it=0, sa_size_t iu=0) const ;
  inline T&       Elem(sa_size_t ix, sa_size_t iy, sa_size_t iz, sa_size_t it=0, sa_size_t iu=0);
  inline T const& ElemCheckBound(sa_size_t ix, sa_size_t iy, sa_size_t iz, sa_size_t it=0, sa_size_t iu=0) const ;
  inline T&       ElemCheckBound(sa_size_t ix, sa_size_t iy, sa_size_t iz, sa_size_t it=0, sa_size_t iu=0);

  //! Return pointer to first element adress
  inline        T* Data()       {return mNDBlock.Begin()+offset_;}
  //! Return pointer to first element adress
  inline const  T* Data() const {return mNDBlock.Begin()+offset_;}
  //! Return reference to datablock NDataBlock
  inline        NDataBlock<T>& DataBlock()       {return mNDBlock;}
  //! Return reference to datablock NDataBlock
  inline const  NDataBlock<T>& DataBlock() const {return mNDBlock;}

  // Temporaire?
  //! Are the array temporay ?
  inline bool   IsTemp(void) const {return mNDBlock.IsTemp();}
  //! Set the array as temporay
  inline void   SetTemp(bool temp=false) const {mNDBlock.SetTemp(temp);}

// Operations diverses  = , +=, ...
// Conversion en type T, if Size() == 1
  inline T toScalar();
// Met les elements a une suite de valeurs
  virtual TArray<T>&  SetSeq(Sequence const & seq);
  //! Fill TArray with Sequence \b seq
  inline  TArray<T>&  operator = (Sequence const & seq)    { return SetSeq(seq); }

// A = x (tous les elements a x)
  virtual TArray<T>&  SetCst(T x);
  //! Fill an array with a constant value \b x ( alias for \b SetCst() method )
  inline  TArray<T>&  SetT(T x) { return SetCst(x); }
  //! Fill TArray with all elements equal to \b x
  inline  TArray<T>&  operator = (T x)             { return SetT(x); }

// addition et soustraction de constante 
  virtual TArray<T>&  AddCst(T x, TArray<T>& res) const ;
  virtual TArray<T>&  SubCst(T x, TArray<T>& res, bool fginv=false) const ;
// Multiplication et division par une  constante 
  virtual TArray<T>&  MulCst(T x, TArray<T>& res) const ;
  virtual TArray<T>&  DivCst(T x, TArray<T>& res, bool fginv=false) const ;

// A += -= *= /= x (ajoute, soustrait, ... x a tous les elements)
  // Methodes Add/Sub/Mul/Div() sont la pour compatibilite avant V=2 (1.818)
  // Faut-il les garder ? Reza, Juillet 2004
  inline  TArray<T>&  Add(T x)                     { return AddCst(x, *this); }
  inline  TArray<T>&  Sub(T x, bool fginv=false)   { return SubCst(x, *this, fginv); }
  inline  TArray<T>&  Mul(T x)                     { return MulCst(x, *this); }
  inline  TArray<T>&  Div(T x, bool fginv=false)   { return DivCst(x, *this, fginv); }

  //! Add \b x to all elements
  inline  TArray<T>&  operator += (T x)            { return AddCst(x, *this); }
  //! Substract \b x to all elements
  inline  TArray<T>&  operator -= (T x)            { return SubCst(x, *this); }
  //! Multiply all elements by \b x
  inline  TArray<T>&  operator *= (T x)            { return MulCst(x, *this); }
  //! Divide all elements by \b x
  inline  TArray<T>&  operator /= (T x)            { return DivCst(x, *this); }

// applique le signe moins a tous les elements
  virtual TArray<T>&  NegateElt(TArray<T>& res) const ;
//! Replace array elements values by their opposite ( (*this)(i) -> -(*this)(i) )
  virtual TArray<T>&  NegateEltSelf();  

//! return a new array, with contents corresponding to the square of the original array elements 
  virtual TArray<T>   SquareElt(bool pack=true) const ;
//! Replace array elements values by their square ( (*this)(i) -> (*this)(i)^2 )
  virtual TArray<T>&  SquareEltSelf();

// A += -=  (ajoute, soustrait element par element les deux tableaux )
  virtual TArray<T>&  AddElt(const TArray<T>& a, TArray<T>& res) const ;
  virtual TArray<T>&  SubElt(const TArray<T>& a, TArray<T>& res, bool fginv=false) const ;
// Multiplication, division element par element les deux tableaux 
  virtual TArray<T>&  MulElt(const TArray<T>& a, TArray<T>& res) const ;
  virtual TArray<T>&  DivElt(const TArray<T>& a, TArray<T>& res, bool fginv=false, bool divzero=false) const ;

  //! Operator TArray += TArray (element by element addition in place)
  inline  TArray<T>&  operator += (const TArray<T>& a)  { return AddElt(a, *this); }
  //! Operator TArray -= TArray (element by element subtraction in place)
  inline  TArray<T>&  operator -= (const TArray<T>& a)  { return SubElt(a, *this); }

  // Doit-on definir les operateur *= /= TArray ?  Reza, Juillet 2004
  //! Element by element multiplication in place TArray *= TArray (element by element)
  inline  TArray<T>&  Mul(const TArray<T>& a)           { return MulElt(a, *this); }
  //! Element by element division in place TArray *= TArray (element by element)
  inline  TArray<T>&  Div(const TArray<T>& a, bool divzero=false)   
                                                        { return DivElt(a, *this, false, divzero); }

// Recopie des valeurs, element par element
  virtual TArray<T>&  CopyElt(const TArray<T>& a);
// Recopie des valeurs avec conversion prealable, element par element
  virtual TArray<T>&  ConvertAndCopyElt(const BaseArray& a);

// Calcul du produit scalaire ( Somme_i (*this)(i)*a(i) )
  virtual T ScalarProduct(const TArray<T>& a) const ;
//! return Sum_i (*this)(i)*a(i) - alias for ScalarProduct
  inline  T DotProduct(const TArray<T>& a) const 
  { return  ScalarProduct(a); }
// Somme et produit des elements
  virtual T Sum() const ;
  virtual T Product() const ;
// Somme du carre des elements
  virtual T SumSq() const;
//  Norme^2 , identique a SumSq pour tableaux reels/integer , sum[el * conj(el)] pour complexe
  virtual T Norm2() const; 
// Valeur min et max des elements (sauf tableaux complexes -> exception)
  virtual void MinMax(T& min, T& max) const ;

// Impression, I/O, ...
  virtual std::string InfoString() const;   
  virtual std::ostream&  Print(std::ostream& os, sa_size_t maxprt=-1, 
			       bool si=false, bool ascd=false) const ;

// Lecture,Ecriture sur fichier ASCII
  virtual sa_size_t ReadASCII(std::istream& is, sa_size_t & nr, sa_size_t & nc,
			      char clm='#', const char* sep=" \t");
  virtual void   WriteASCII(std::ostream& os) const;

/*!
  \brief assign a new object Id (or DataRef Id) - useful for PPF write operations 

  This method should be called when a given array is written to a PPF file several times,
  after its content has been changed.  By calling this method before a write to PPF stream, 
  the DataBlock object is written to the stream, regardless of it being already saved to 
  the PPF stream or not. 
*/
  inline void RenewObjId() { mNDBlock.RenewObjId(); }
//  Pour la gestion de persistance
  friend class  FIO_TArray<T>;

protected:
  //! A non virtual, faster, non protected version of Offset(sa_size_t ip), used by the [] element access operator 
  inline size_t FastOffset(sa_size_t ip)  const 
  {   
    if (moystep_ == 1)  return (offset_+ip); 
    sa_size_t off = offset_;
    for(int k=0; k<ndim_; k++) { off += (ip%size_[k])*step_[k];  ip /= size_[k]; }
    return (off);
  }
  NDataBlock<T> mNDBlock; //!< Block for datas
  mutable MuTyV my_mtv;   //!< for use by ValueAtPosition()
};

////////////////////////////////////////////////////////////////
//   Surcharge d'operateur << 
//! Print TArray \b a on stream \b os
template <class T>
inline std::ostream& operator << (std::ostream& os, const TArray<T>& a)
                            { a.Print(os);    return(os);    }

//   Surcharge d'operateur >> 
//! Decodes the ASCII input stream \b is , filling TArray \b a elements
template <class T>
inline std::istream& operator >> (std::istream& is, TArray<T>& a)
                            { sa_size_t nr, nc; 
			      a.ReadASCII(is, nr, nc);    return(is);    }


////////////////////////////////////////////////////////////////
// Surcharge d'operateurs A (+,-,*,/) (T) x

/*! \ingroup TArray \fn operator+(const TArray<T>&,T)
  \brief Operator TArray = TArray + constant */
template <class T> inline TArray<T> operator + (const TArray<T>& a, T b)
    {TArray<T> result; result.SetTemp(true); 
    a.AddCst(b, result); return result;}

/*! \ingroup TArray \fn operator+(T,const TArray<T>&)
  \brief Operator TArray = constant + TArray */
template <class T> inline TArray<T> operator + (T b,const TArray<T>& a)
    {TArray<T> result; result.SetTemp(true); 
    a.AddCst(b, result); return result;}

/*! \ingroup TArray \fn operator-(const TArray<T>&,T)
  \brief Operator TArray = TArray - constant */
template <class T> inline TArray<T> operator - (const TArray<T>& a, T b)
    {TArray<T> result; result.SetTemp(true); 
    a.SubCst(b,result); return result;}

/*! \ingroup TArray \fn operator-(T,const TArray<T>&)
  \brief Operator TArray = constant - TArray */
template <class T> inline TArray<T> operator - (T b,const TArray<T>& a)
    {TArray<T> result; result.SetTemp(true); 
    a.SubCst(b,result,true); return result;}

/*! \ingroup TArray \fn operator*(const TArray<T>&,T)
  \brief Operator TArray = TArray * constant */
template <class T> inline TArray<T> operator * (const TArray<T>& a, T b)
    {TArray<T> result; result.SetTemp(true); 
    a.MulCst(b, result); return result;}

/*! \ingroup TArray \fn operator*(T,const TArray<T>&)
  \brief Operator TArray = constant * TArray */
template <class T> inline TArray<T> operator * (T b,const TArray<T>& a)
    {TArray<T> result; result.SetTemp(true); 
    a.MulCst(b,result); return result;}

/*! \ingroup TArray \fn operator/(const TArray<T>&,T)
  \brief Operator TArray = TArray / constant */
template <class T> inline TArray<T> operator / (const TArray<T>& a, T b)
    {TArray<T> result; result.SetTemp(true); 
    a.DivCst(b,result); return result;}

/*! \ingroup TArray \fn operator/(T,const TArray<T>&)
  \brief Operator TArray = constant / TArray  */
template <class T> inline TArray<T> operator / (T b, const TArray<T>& a)
    {TArray<T> result; result.SetTemp(true); 
    a.DivCst(b, result, true); return result;}

////////////////////////////////////////////////////////////////
// Surcharge d'operateurs B = -A

/*! \ingroup TArray \fn operator - (const TArray<T>&)
  \brief Operator - Returns an array with elements equal to the opposite of
  the original array elements.  */
template <class T> inline TArray<T> operator - (const TArray<T>& a)
    {TArray<T> result; result.SetTemp(true); 
    a.NegateElt(result); return result;}

////////////////////////////////////////////////////////////////
// Surcharge d'operateurs C = A (+,-,*,/) B

/*! \ingroup TArray \fn operator+(const TArray<T>&,const TArray<T>&)
  \brief Operator TArray = TArray + TArray (element by element addition) */
template <class T>
inline TArray<T> operator + (const TArray<T>& a,const TArray<T>& b)
    { TArray<T> result; result.SetTemp(true); 
    a.AddElt(b, result);    return result; }

/*! \ingroup TArray \fn operator-(const TArray<T>&,const TArray<T>&)
  \brief Operator TArray = TArray - TArray (element by element subtraction) */
template <class T>
inline TArray<T> operator - (const TArray<T>& a,const TArray<T>& b)
    { TArray<T> result; result.SetTemp(true); 
    a.SubElt(b, result);    return result; }

/*! \ingroup TArray \fn operator * (const TArray<T>&,const TArray<T>&)
  \brief Element by element multiplication of two arrays TArray = TArray * TArray */

template <class T>
inline TArray<T> operator * (const TArray<T>& a,const TArray<T>& b)
    { TArray<T> result; result.SetTemp(true); 
    a.MulElt(b, result);    return result; }

/*! \ingroup TArray \fn operator / (const TArray<T>&,const TArray<T>&)
  \brief Element by element division of two arrays TArray = TArray / TArray */
template <class T>
inline TArray<T> operator / (const TArray<T>& a,const TArray<T>& b)
    { TArray<T> result; result.SetTemp(true); 
    a.DivElt(b, result, false, false);    return result; }

// --------------------------------------------------
//        inline element acces methods
// --------------------------------------------------

//! Return element (ix,iy,iz,it,iu) value
template <class T>
inline T const& TArray<T>::Elem(sa_size_t ix, sa_size_t iy, sa_size_t iz, sa_size_t it, sa_size_t iu) const
{
  return ( *( mNDBlock.Begin()+ offset_+ 
	      ix*step_[0] + iy*step_[1] + iz*step_[2] +
	      it*step_[3] + iu*step_[4]) );
}

//! Return element (ix,iy,iz,it,iu) value
template <class T>
inline T & TArray<T>::Elem(sa_size_t ix, sa_size_t iy, sa_size_t iz, sa_size_t it, sa_size_t iu)
{
  return ( *( mNDBlock.Begin()+ offset_+ 
	      ix*step_[0] + iy*step_[1] + iz*step_[2] +
	      it*step_[3] + iu*step_[4]) );
}

//! Return element (ix,iy,iz,it,iu) value with Check of indexes bound first
template <class T>
inline T const& TArray<T>::ElemCheckBound(sa_size_t ix, sa_size_t iy, sa_size_t iz, sa_size_t it, sa_size_t iu) const
{
  CheckBound(ix, iy, iz, it, iu, 4);
  return(Elem(ix, iy, iz, it, iu));
}

//! Return element (ix,iy,iz,it,iu) value with Check of indexes bound first
template <class T>
inline T & TArray<T>::ElemCheckBound(sa_size_t ix, sa_size_t iy, sa_size_t iz, sa_size_t it, sa_size_t iu)
{
  CheckBound(ix, iy, iz, it, iu, 4);
  return(Elem(ix, iy, iz, it, iu));
}
  
//=======>  a(ix)  access operators  
//! Return the value of the element (ix) - suitable for 1D arrays 
template <class T>
inline T const& TArray<T>::operator()(sa_size_t ix) const
{
#ifdef SO_BOUNDCHECKING
  CheckBound(ix, 0, 0, 0, 0, 4);
#endif
  return ( *( mNDBlock.Begin()+ offset_+ ix*step_[0] ) );
}

//! Return a reference to the element the element (ix) - suitable for 1D arrays 
template <class T>
inline T & TArray<T>::operator()(sa_size_t ix) 
{
#ifdef SO_BOUNDCHECKING
  CheckBound(ix, 0, 0, 0, 0, 4);
#endif
  return ( *( mNDBlock.Begin()+ offset_+ ix*step_[0] ) );
}

//=======>  a(ix,iy) OR a(ix) accees operators  
//! Return the value of the element (ix,iy) - suitable for 2D arrays 
template <class T>
inline T const& TArray<T>::operator()(sa_size_t ix, sa_size_t iy) const
{
#ifdef SO_BOUNDCHECKING
  CheckBound(ix, iy, 0, 0, 0, 4);
#endif
  return ( *( mNDBlock.Begin()+ offset_+ 
	      ix*step_[0] + iy*step_[1] ) );
}

//! Return a reference to the element (ix,iy) - suitable for 2D arrays 
template <class T>
inline T & TArray<T>::operator()(sa_size_t ix, sa_size_t iy) 
{
#ifdef SO_BOUNDCHECKING
  CheckBound(ix, iy, 0, 0, 0, 4);
#endif
  return ( *( mNDBlock.Begin()+ offset_+ 
	      ix*step_[0] + iy*step_[1] ) );
}

//=======>  a(ix,iy,iz) accees operators  
//! Return the value of the element (ix,iy,iz) - suitable for 3D arrays 
template <class T>
inline T const& TArray<T>::operator()(sa_size_t ix, sa_size_t iy, sa_size_t iz) const
{
#ifdef SO_BOUNDCHECKING
  CheckBound(ix, iy, iz, 0, 0, 4);
#endif
  return ( *( mNDBlock.Begin()+ offset_+ 
	      ix*step_[0] + iy*step_[1] + iz*step_[2]) );
}

//! Return a reference to the element (ix,iy,iz) - suitable for 3D arrays 
template <class T>
inline T & TArray<T>::operator()(sa_size_t ix, sa_size_t iy, sa_size_t iz) 
{
#ifdef SO_BOUNDCHECKING
  CheckBound(ix, iy, iz, 0, 0, 4);
#endif
  return ( *( mNDBlock.Begin()+ offset_+ 
	      ix*step_[0] + iy*step_[1] + iz*step_[2]) );
}

//=======>  a(ix,iy,iz,it,iu) or a(ix,iy,iz,it) accees operators  
/*! 
  \brief Return the value of the element  (ix,iy,iz,it,iu) - suitable for 4D or 5D arrays 

  Specify iu = 0 for 4D arrays 
*/
template <class T>
inline T const& TArray<T>::operator()(sa_size_t ix, sa_size_t iy, sa_size_t iz, sa_size_t it, sa_size_t iu) const
{
#ifdef SO_BOUNDCHECKING
  CheckBound(ix, iy, iz, it, iu, 4);
#endif
  return ( *( mNDBlock.Begin()+ offset_+ 
	      ix*step_[0] + iy*step_[1] + iz*step_[2] +
	      it*step_[3] + iu*step_[4]) );
}

/*! 
  \brief Return a reference to the element  (ix,iy,iz,it,iu) - suitable for 4D or 5D arrays 

  Specify iu = 0 for 4D arrays 
*/

template <class T>
inline T & TArray<T>::operator()(sa_size_t ix, sa_size_t iy, sa_size_t iz, sa_size_t it, sa_size_t iu)
{
#ifdef SO_BOUNDCHECKING
  CheckBound(ix, iy, iz, it, iu, 4);
#endif
  return ( *( mNDBlock.Begin()+ offset_+ 
	      ix*step_[0] + iy*step_[1] + iz*step_[2] +
	      it*step_[3] + iu*step_[4]) );
}


//! Operator [] : return element at positon ip
template <class T>
inline T const& TArray<T>::operator[](sa_size_t ip) const
{
#ifdef SO_BOUNDCHECKING
  return *(mNDBlock.Begin()+Offset(ip));
#else 
return *(mNDBlock.Begin()+FastOffset(ip));
#endif
}

//! Operator [] : return element at positon ip
template <class T>
inline T & TArray<T>::operator[](sa_size_t ip) 
{
#ifdef SO_BOUNDCHECKING
  return *(mNDBlock.Begin()+Offset(ip));
#else 
  return *(mNDBlock.Begin()+FastOffset(ip));
#endif
}


//! Converts to a scalar (value of first element) if the array size is equal to 1 
template <class T>
inline T TArray<T>::toScalar()
{ 
  if (Size() != 1) throw(SzMismatchError("TArray<T>::operator T() Size() != 1")) ;
  return ( (*this)[0] );
}

// Typedef pour simplifier
/*! \ingroup TArray
  \typedef Array
  \brief To simplify the most common usage of TArray<T> class, corresponding to TArray<r_8> (Array of double) 
*/
typedef TArray<r_8> Array;

//--------- extern template declarations (if needed) -----------
#if defined ( NEED_EXT_DECL_TEMP ) && !defined( TARRAY_CC_BFILE )
extern template class TArray<uint_1>; 
extern template class TArray<uint_2>;
extern template class TArray<uint_4>;
extern template class TArray<uint_8>;
extern template class TArray<int_1>; 
extern template class TArray<int_2>;  
extern template class TArray<int_4>;
extern template class TArray<int_8>;
extern template class TArray<r_4>;
extern template class TArray<r_8>;
extern template class TArray< std::complex<r_4> >;
extern template class TArray< std::complex<r_8> >;
#ifdef SO_LDBLE128
extern template class TArray<r_16>;
extern template class TArray< std::complex<r_16> >;
#endif
#endif  // Fin de if defined ( NEED_EXT_DECL_TEMP )

} // Fin du namespace

#endif
