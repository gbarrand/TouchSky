//-----------------------------------------------------------
// Classe TMatrix<T>
//    Matrix class, inheriting from TArray<T>  
// SOPHYA class library - (C)  UPS+LAL IN2P3/CNRS  , CEA-Irfu 
// C.Magneville       1999          
//-----------------------------------------------------------

//                         C.Magneville          04/99

#ifndef TMatrix_SEEN
#define TMatrix_SEEN

#include "machdefs.h"
#include "tarray.h"

namespace SOPHYA {

//! Class of matrices
template <class T>
class TMatrix : public TArray<T> {
public:

#include "tmatrix_tsnl.h"  /* For two level name look-up gcc >= 3.4 */

  // Creation / destruction 
  TMatrix();
  TMatrix(sa_size_t r,sa_size_t c, short mm=BaseArray::AutoMemoryMapping, bool fzero=true);
  TMatrix(const TMatrix<T>& a);
  TMatrix(const TMatrix<T>& a, bool share);
  TMatrix(const TArray<T>& a, bool share=true);
  TMatrix(const BaseArray& a, bool pack=true); 

  virtual ~TMatrix();

  //  Pour verifiez la compatibilite de dimensions lors de l'affectation
  virtual TArray<T>& Set(const TArray<T>& a);
  //! Operator = between matrices
  /*! \warning Datas are copied (cloned) from \b a.
      \sa NDataBlock::operator=(const NDataBlock<T>&) */
  inline  TMatrix<T>& operator = (const TMatrix<T>& a)
                     { Set(a);  return(*this); }
  //! Operator = between a matrix and an array 
  inline  TMatrix<T>& operator = (const TArray<T>& a)
                     { Set(a);  return(*this); }

  virtual TArray<T>& SetBA(const BaseArray& a);
  //! Operator = between matrices with different types
  inline  TMatrix<T>& operator = (const BaseArray& a)
                     { SetBA(a);  return(*this); }

  // Size - Changing the Size 
  //! return number of rows
  inline sa_size_t NRows() const {return size_[marowi_]; }
  //! return number of columns
  inline sa_size_t NCols() const {return size_[macoli_]; }
  //! return number of columns
  inline sa_size_t NCol()  const {return size_[macoli_]; } // back-compat Peida

  void ReSize(sa_size_t r,sa_size_t c, short mm=BaseArray::SameMemoryMapping, bool fzero=true);
  //! a synonym (alias) for method ReSize(sa_size_t, sa_size_t, short)
  inline void SetSize(sa_size_t r,sa_size_t c, short mm=BaseArray::SameMemoryMapping, bool fzero=true)
                 { ReSize(r, c, mm, fzero); }
  // Reallocation de place
  void Realloc(sa_size_t r,sa_size_t c, short mm=BaseArray::SameMemoryMapping, bool force=false);

  // Sub-matrix extraction $CHECK$ Reza 03/2000  Doit-on declarer ces methode const ?
  TMatrix<T> SubMatrix(Range rline, Range rcol) const ;
  //! () : Return submatrix define by \b Range \b rline and \b rcol
  inline TMatrix<T> operator () (Range rline, Range rcol) const 
                    { return SubMatrix(rline, rcol); }
  // Lignes et colonnes de la matrice
  //! Return the sub-matrix corresponding to the row \b ir (row vector)
  inline TMatrix<T> Row(sa_size_t ir) const
                    { return SubMatrix(Range(ir,ir), Range(0,NCols()-1)); }
  //! Return the sub-matrix corresponding to the column \b ic (column vector)
  inline TMatrix<T> Column(sa_size_t ic) const
                    { return SubMatrix(Range(0,NRows()-1), Range(ic,ic)); }
  // a copy of the sub-matrix defined by the row and col ranges, as a packed matrix  
  TMatrix<T> PackAndCopySubMatrix(Range rline, Range rcol, short mm=BaseArray::SameMemoryMapping) const ;
  //! Return a copy of the row \b ir as a packed matrix 
  inline TMatrix<T> CopyRow(sa_size_t ir, short mm=BaseArray::SameMemoryMapping) const
                    { return PackAndCopySubMatrix(Range(ir,ir), Range(0,NCols()-1), mm); }
  //! Return a copy of the column \b ic as a packed matrix 
  inline TMatrix<T> CopyColumn(sa_size_t ic, short mm=BaseArray::SameMemoryMapping) const
                    { return PackAndCopySubMatrix(Range(0,NRows()-1), Range(ic,ic)); }

  // Inline element acces methods 
  inline T const& operator()(sa_size_t r,sa_size_t c) const;
  inline T&       operator()(sa_size_t r,sa_size_t c);

  // Operations matricielles
  TMatrix<T>& TransposeSelf();
  TMatrix<T>  Transpose() const;
  //mm = SameMemoryMapping or CMemoryMapping or FortranMemoryMapping 
  TMatrix<T>  Transpose(short mm) const ; 
  // Rearranging Matrix Elements
  TMatrix<T>  Rearrange(short mm) const;

  // Operateur d'affectation
  // A = x (matrice diagonale Identite)
  virtual TMatrix<T>& SetIdentity(IdentityMatrix imx);
  // = : fill matrix with an identity matrix \b imx
  inline  TMatrix<T>& operator = (IdentityMatrix imx) { return SetIdentity(imx); }

  // = : fill matrix with a Sequence \b seq
  inline  TMatrix<T>&  operator = (Sequence const & seq)    { SetSeq(seq); return(*this); }

  // Operations diverses  avec une constante
  //! = : fill matrix with constant value \b x
  inline  TMatrix<T>&  operator = (T x)             { SetT(x); return(*this); }
  //! += : add constant value \b x to matrix
  inline  TMatrix<T>&  operator += (T x)            { AddCst(x,*this); return(*this); }
  //! -= : substract constant value \b x to matrix
  inline  TMatrix<T>&  operator -= (T x)            { SubCst(x,*this); return(*this); }
  //! *= : multiply matrix by constant value \b x
  inline  TMatrix<T>&  operator *= (T x)            { MulCst(x,*this); return(*this); }
  //! /= : divide matrix by constant value \b x
  inline  TMatrix<T>&  operator /= (T x)            { DivCst(x,*this); return(*this); }

  //  operations avec matrices 
  //! += : add a matrix
  inline  TMatrix<T>&  operator += (const TMatrix<T>& a)  { AddElt(a,*this); return(*this); }
  //! -= : substract a matrix
  inline  TMatrix<T>&  operator -= (const TMatrix<T>& a)  { SubElt(a,*this); return(*this); }

  TMatrix<T>  Multiply(const TMatrix<T>& b, short mm=BaseArray::SameMemoryMapping) const;
  //A supprimer ? Reza Juillet 2004 ! *= : matrix product : C = (*this)*B
  //  inline  TMatrix<T>&  operator *= (const TMatrix<T>& b)
  //       { this->Set(Multiply(b));  return(*this); }

  // I/O print, ...
  virtual std::string InfoString() const;
  virtual std::ostream&  Print(std::ostream& os, sa_size_t maxprt=-1, 
			       bool si=false, bool ascd=false) const ;

protected:
};

//  ---- inline acces methods ------
 //! () : return element for line \b r and column \b c
template <class T>
inline T const& TMatrix<T>::operator()(sa_size_t r, sa_size_t c) const
{
#ifdef SO_BOUNDCHECKING
  if (marowi_ == 0)   CheckBound(r, c, 0, 0, 0, 4);
  else   CheckBound(c, r, 0, 0, 0, 4);
#endif
  return ( *( mNDBlock.Begin()+ offset_+ 
	      r*step_[marowi_] + c*step_[macoli_] ) );
}

//! () : return element for line \b r and column \b c
template <class T>
inline T & TMatrix<T>::operator()(sa_size_t r, sa_size_t c) 
{
#ifdef SO_BOUNDCHECKING
  if (marowi_ == 0)   CheckBound(r, c, 0, 0, 0, 4);
  else   CheckBound(c, r, 0, 0, 0, 4);
#endif
  return ( *( mNDBlock.Begin()+ offset_+ 
	      r*step_[marowi_] + c*step_[macoli_] ) );
}
////////////////////////////////////////////////////////////////
// Surcharge d'operateurs A (+,-,*,/) (T) x

/*! \ingroup TMatrix \fn operator+(const TMatrix<T>&,T)
  \brief Operator TMatrix = TMatrix + constant */
template <class T> inline TMatrix<T> operator + (const TMatrix<T>& a, T b)
    {TMatrix<T> result; result.SetTemp(true); 
    a.AddCst(b,result); return result;}

/*! \ingroup TMatrix \fn operator+(T,const TMatrix<T>&)
  \brief Operator TMatrix = constant + TMatrix */
template <class T> inline TMatrix<T> operator + (T b,const TMatrix<T>& a)
    {TMatrix<T> result; result.SetTemp(true); 
    a.AddCst(b,result); return result;}

/*! \ingroup TMatrix \fn operator-(const TMatrix<T>&,T)
  \brief Operator TMatrix = TMatrix - constant */
template <class T> inline TMatrix<T> operator - (const TMatrix<T>& a, T b)
    {TMatrix<T> result; result.SetTemp(true); 
    a.SubCst(b,result); return result;}

/*! \ingroup TMatrix \fn operator-(T,const TMatrix<T>&)
  \brief Operator TMatrix = constant - TMatrix */
template <class T> inline TMatrix<T> operator - (T b,const TMatrix<T>& a)
    {TMatrix<T> result; result.SetTemp(true); 
    a.SubCst(b,result,true); return result;}

/*! \ingroup TMatrix \fn operator*(const TMatrix<T>&,T)
  \brief Operator TMatrix = TMatrix * constant */
template <class T> inline TMatrix<T> operator * (const TMatrix<T>& a, T b)
    {TMatrix<T> result; result.SetTemp(true); 
    a.MulCst(b,result); return result;}

/*! \ingroup TMatrix \fn operator*(T,const TMatrix<T>&)
  \brief Operator TMatrix = constant * TMatrix */
template <class T> inline TMatrix<T> operator * (T b,const TMatrix<T>& a)
    {TMatrix<T> result; result.SetTemp(true); 
    a.MulCst(b,result); return result;}

/*! \ingroup TMatrix \fn operator/(const TMatrix<T>&,T)
  \brief Operator TMatrix = TMatrix / constant */
template <class T> inline TMatrix<T> operator / (const TMatrix<T>& a, T b)
    {TMatrix<T> result; result.SetTemp(true); 
    a.DivCst(b,result); return result;}

/*! \ingroup TMatrix \fn operator/(T,const TMatrix<T>&)
  \brief Operator TMatrix = constant / TMatrix  */
template <class T> inline TMatrix<T> operator / (T b, const TMatrix<T>& a)
    {TMatrix<T> result; result.SetTemp(true); 
    a.Div(b,result,true); return result;}

////////////////////////////////////////////////////////////////
// Surcharge d'operateurs B = -A

/*! \ingroup TMatrix \fn operator - (const TMatrix<T>&)
  \brief Operator - Returns a matrix with elements equal to the opposite of
  the original matrix elements.  */
template <class T> inline TMatrix<T> operator - (const TMatrix<T>& a)
    {TMatrix<T> result; result.SetTemp(true); 
    a.NegateElt(result); return result;}


// Surcharge d'operateurs C = A (+,-) B
// $CHECK$ Reza 3/4/2000 Pas necessaire  de redefinir les operateurs
// Defini au niveau de TArray<T> - Pour ameliorer l'efficacite
// Doit-on le faire aussi pour les constantes ? - Fin de $CHECK$ Reza 3/4/2000

/*! \ingroup TArray \fn operator+(const TMatrix<T>&,const TMatrix<T>&)
  \brief return a+b , a matrix equal to the  addition of the two matrices \b a and \b b  (element wise operation) */
template <class T>
inline TMatrix<T> operator + (const TMatrix<T>& a,const TMatrix<T>& b)
    {TMatrix<T> result; result.SetTemp(true); 
    a.AddElt(b, result);     return result; }


/*! \ingroup TArray \fn operator-(const TMatrix<T>&,const TMatrix<T>&)
  \brief return a-b , a matrix equal to the  subtraction of the two matrices \b a and \b b  (element wise operation) */
template <class T>
inline TMatrix<T> operator - (const TMatrix<T>& a,const TMatrix<T>& b)
    {TMatrix<T> result; result.SetTemp(true); 
    a.SubElt(b, result);     return result; }

/*! \ingroup TArray \fn operator&&(const TMatrix<T>&,const TMatrix<T>&)
  \brief return element wise a*b , a matrix equal to the element wise product of the two matrices \b a and \b b  
  
  \warning Notice the usage of the operator && (usually a logical operator) to distinguish element wise operation from matrix product
 */
template <class T>
inline TMatrix<T> operator && (const TMatrix<T>& a,const TMatrix<T>& b)
    {TMatrix<T> result; result.SetTemp(true); 
    a.MulElt(b, result);     return result; }
    

// Surcharge d'operateurs C = A * B
/*! \ingroup TArray \fn operator*(const TMatrix<T>&,const TMatrix<T>&)
  \brief Matrix multiplication operator, returns a matrix equal to the matrix product of the two  matrices \b a and \b b  ( a*b) */
template <class T> inline TMatrix<T> operator * (const TMatrix<T>& a, const TMatrix<T>& b)
   { return(a.Multiply(b)); }

// Typedef pour simplifier et compatibilite Peida
/*! \ingroup TArray
  \typedef Matrix
  \brief To simplify the most common usage of TMatrix<T> class, corresponding to TMatrix<r_8> (Matrix of double) 
*/
typedef TMatrix<r_8> Matrix;

//--------- extern template declarations (if needed) -----------
#if defined ( NEED_EXT_DECL_TEMP ) && !defined( TMATRIX_CC_BFILE )
extern template class TMatrix<uint_1>;
extern template class TMatrix<uint_2>;
extern template class TMatrix<uint_4>;
extern template class TMatrix<uint_8>;
extern template class TMatrix<int_1>;
extern template class TMatrix<int_2>;
extern template class TMatrix<int_4>;
extern template class TMatrix<int_8>;
extern template class TMatrix<r_4>;
extern template class TMatrix<r_8>;
extern template class TMatrix< std::complex<r_4> >;
extern template class TMatrix< std::complex<r_8> >;
#ifdef SO_LDBLE128
extern template class TMatrix<r_16>;
extern template class TMatrix< std::complex<r_16> >;
#endif
#endif  // Fin de if defined ( NEED_EXT_DECL_TEMP )


} // Fin du namespace

#endif
