// This may look like C code, but it is really -*- C++ -*-
// This code is part of the SOPHYA library
//  (C) Univ. Paris-Sud   (C) LAL-IN2P3/CNRS   (C) IRFU-CEA
//  (C) R. Ansari, C.Magneville    2009-2014 

#ifndef TRNGMTX_H_SEEN
#define TRNGMTX_H_SEEN

#include "spesqmtx.h"

namespace SOPHYA {

//---------------------------------------------------------------------------------------------------------
//------------------------------------- PART 1 : LowerTriangularMatrix ------------------------------------
//---------------------------------------------------------------------------------------------------------

/*!
  \class LowerTriangularMatrix
  \ingroup TArray
  \brief Class representing a lower (inferior) triangular matrix. 

  The lower triangular matrix is mapped in memory with elements in columns packed in memory, 
  as illustrated below for a 5x5 triangular matrix.
  \verbatim
  Column-Packed 5x5 Inf.Triang.Matrix, Size= 5*(5+1)/2 = 15 elements (0 ... 14) 
  | 0                  |
  | 1   5              |
  | 2   6   9          |
  | 3   7   10  12     |
  | 4   8   11  13  14 |
  \endverbatim 
  
  This class offers similar functionalities to the TArray<T> / TMatrix<T> classes, like 
  reference sharing and counting, arithmetic operators ... However, this class has no 
  sub matrix extraction method.
*/
  
template <class T>
class LowerTriangularMatrix : public SpecialSquareMatrix<T> {
public :

#include "spesqmtx_tsnl.h"

//! Default constructor - TriangMatrix of size 0, SetSize() should be called before the object is used
explicit LowerTriangularMatrix()
  : SpecialSquareMatrix<T>(C_LowerTriangularMatrix)
{
  mZeros = T(0);
}

//! Instanciate a triangular matrix from the number of rows (rowSize must be > 0)
explicit LowerTriangularMatrix(sa_size_t rowSize)
  : SpecialSquareMatrix<T>(rowSize, C_LowerTriangularMatrix)
{
  if (rowSize < 1) 
    throw ParmError("LowerTriangularMatrix<T>::LowerTriangularMatrix(rsz) rsz <= 0");
  mElems.ReSize((rowSize*(rowSize+1)/2) ); 
  mZeros = T(0);
}

//! Copy constructor (possibility of sharing datas)
LowerTriangularMatrix(LowerTriangularMatrix<T> const & a,  bool share=false)  
  : SpecialSquareMatrix<T>(a, share) 
{
  mZeros = T(0);
}

//! Copy constructor (possibility of sharing datas)
LowerTriangularMatrix(SpecialSquareMatrix<T> const & a,  bool share=false)  
  : SpecialSquareMatrix<T>(a, share) 
{
  if (a.MtxType() != C_LowerTriangularMatrix) 
    throw TypeMismatchExc("LowerTriangularMatrix(a) a NOT a LowerTriangularMatrix");
  mZeros = T(0);
}

/*!
 \brief Create a lower triangular matrix from a square matrix. 
 Elements above the diagonal are ignored.
*/
explicit LowerTriangularMatrix(TMatrix<T> const & mx)  
  : SpecialSquareMatrix<T>(C_LowerTriangularMatrix)
{
  if ((mx.NRows() != mx.NCols()) || (mx.NRows() < 1)) 
    throw ParmError("LowerTriangularMatrix<T>::(TMatrix<T> const & mx) mx not allocated OR NOT a square matrix");
  SetSize(mx.NRows());
  for(sa_size_t l=0; l<NRows(); l++) 
    for(sa_size_t m=0; m<=l; m++) (*this)(l,m) = mx(l,m);
  mZeros = T(0);
}

//! Sets or change the triangular matrix size, specifying the new number of rows
virtual void SetSize(sa_size_t rowSize) 
{
  if (rowSize < 1) 
    throw ParmError("LowerTriangularMatrix<T>::SetSize(rsz) rsz <= 0");
  if (rowSize == mNrows)  return;
  mNrows=rowSize;
  mElems.ReSize(mNrows*(mNrows+1)/2);  
}

//! Return the object (triangular matrix) as a standard square matrix
virtual TMatrix<T> ConvertToStdMatrix() const
{
  if (mNrows < 1)
    throw SzMismatchError("LowerTriangularMatrix<T>::ConvertToStdMatrix() (this) not allocated !");
  SOPHYA::TMatrix<T> mx(NRows(), NRows());
  for(sa_size_t l=0; l<NRows(); l++) 
    for(sa_size_t m=0; m<=l; m++) mx(l,m) = (*this)(l,m);
  return mx;
}
 
//--- Operateurs = (T b) , = (LowerTriangularMatrix<T> b)
//! operator = a , to set all elements to the value \b a
inline LowerTriangularMatrix<T>& operator = (T a)
  {  SetCst(a);  return (*this);  }
//! operator = LowerTriangularMatrix<T> a , element by element copy operator
inline LowerTriangularMatrix<T>& operator = (LowerTriangularMatrix<T> const & a)
  {  Set(a); return (*this); }
//! operator = Sequence seq  
inline LowerTriangularMatrix<T>& operator = (Sequence const & seq)
  {  SetSeq(seq); return (*this); }


//--- Operateurs d'acces aux elements 
//! Element access operator (R/W): access to elements row \b r and column \b c
inline T& operator()(sa_size_t r, sa_size_t c) 
{ 
  if ((r<0)||(r>=mNrows))  
    throw RangeCheckError("LowerTriangularMatrix<T>::operator()(r,c) (r<0)||(r>=NRows())");
  if (c>r) { mZeros = T(0); return mZeros; }
  /*  if (mFgRowPacked)   // the (inferior triangular )matrix is stored row by row
      return(mElems(c + r*(r+1)/2)); */
  // the (inferior triangular )matrix is stored column by column
  return(mElems(r+ c*mNrows-c*(c+1)/2));
}
//! Element access operator (RO): access to elements row \b l and column \b m
inline T operator()(sa_size_t r, sa_size_t c) const
{ 
  if ((r<0)||(r>=mNrows))  
    throw RangeCheckError("LowerTriangularMatrix<T>::operator()(r,c) (r<0)||(r>=NRows())");
  if (c>r) { mZeros = T(0); return mZeros; }
  /*  if (mFgRowPacked)   // the (inferior triangular )matrix is stored row by row
      return(mElems(c + r*(r+1)/2)); */
  // the (inferior triangular )matrix is stored column by column
  return(mElems(r+ c*mNrows-c*(c+1)/2));
}
//! return the element (r,c) row \b r and column \b c
virtual T getElem(sa_size_t r,sa_size_t c) const
{
  if ((r<0)||(r>=mNrows))  
    throw RangeCheckError("LowerTriangularMatrix<T>::operator()(r,c) (r<0)||(r>=NRows())");
  if (c>r) { mZeros = T(0); return mZeros; }
  return(mElems(r+ c*mNrows-c*(c+1)/2));
}

//! Return the pointer to the first non zero element in column \b j = &(tmmtx(j,j)) 
inline const T* columnData(sa_size_t j)  const 
{
  /*  if (mFgRowPacked)   // the (inferior triangular )matrix is stored row by row
      return mElems.Begin()+j*(j+1)/2;  */
  // the (inferior triangular )matrix is stored column by column
  return mElems.Begin()+(mNrows*j-j*(j-1)/2) ;
}
//! Return the pointer to the first non zero element in column \b j = &(tmmtx(j,j)) 
inline T* columnData(sa_size_t j) 
{
  /*  if (mFgRowPacked)   // the (inferior triangular )matrix is stored row by row
      return mElems.Begin()+j*(j+1)/2;  */
  // the (inferior triangular )matrix is stored column by column
  return mElems.Begin()+(mNrows*j-j*(j-1)/2) ;
}

//! compute the position of the element \b tm(r,c) relative to the first element
inline sa_size_t indexOfElement(sa_size_t r,sa_size_t c) const 
{
  /* if (mFgRowPacked)  // the (inferior triangular )matrix is stored row by row
     return (c + r*(r+1)/2); */
  // the (inferior triangular )matrix is stored column by column
  return(r+ c*mNrows-c*(c+1)/2);
}

//! Triangular Matrix product (multiplication) : ret_matrix = (*this) * tmx
LowerTriangularMatrix<T> Multiply(LowerTriangularMatrix<T> const & tmx) const
{
  if (NRows() != tmx.NRows()) 
    throw SzMismatchError("Matrix<T>::Multiply(LowerTriangularMatrix<T> tmx): different sizes");
// codage peu efficace : on utilise la multiplication de matrices generales ...
  TMatrix<T> a = ConvertToStdMatrix();
  TMatrix<T> b = tmx.ConvertToStdMatrix();
  LowerTriangularMatrix<T> ret(a.Multiply(b));
  ret.SetTemp(true);
  return ret;
}

//! Matrix product (multiplication) : ret_matrix = (*this) * mx
TMatrix<T> MultiplyTG(TMatrix<T> const & mx) const 
{
  if (NCols() != mx.NRows())
    throw SzMismatchError("LowerTriangularMatrix<T>::MultiplyTG(TMatrix<T> mx): NCols()!=mx.NRows()");
  TMatrix<T> a = ConvertToStdMatrix();
  return a.Multiply(mx); 
}

//! Matrix product (multiplication) : ret_matrix = mx * (*this)
TMatrix<T> MultiplyGT(TMatrix<T> const & mx) const 
{
  if (NRows() != mx.NCols())
    throw SzMismatchError("LowerTriangularMatrix<T>::MultiplyGT(TMatrix<T> mx): NRows()!=mx.NCols()");
  TMatrix<T> a = ConvertToStdMatrix();
  return mx.Multiply(a); 
}


protected: 
mutable T mZeros; 
};

//----- Surcharge d'operateurs C = A * B (multiplication matricielle)
/*! \ingroup TArray \fn operator*(const LowerTriangularMatrix<T>&,const LowerTriangularMatrix<T>&)
  \brief * : LowerTriangularMatrix multiplication \b a and \b b */
template <class T> 
inline LowerTriangularMatrix<T> operator * (const LowerTriangularMatrix<T>& a, const LowerTriangularMatrix<T>& b)
   { return(a.Multiply(b)); }

/*! \ingroup TArray \fn operator*(const LowerTriangularMatrix<T>&,const TMatrix<T>&)
  \brief * : Matrix multiplication LowerTriangularMatrix (\b a ) *  TMatrix<T> ( \b b ) */
template <class T> 
inline TMatrix<T> operator * (const LowerTriangularMatrix<T>& a, const TMatrix<T>& b)
   { return(a.MultiplyTG(b)); }

/*! \ingroup TArray \fn operator*(const TMatrix<T>&,const LowerTriangularMatrix<T>&)
  \brief * : Matrix multiplication TMatrix (\b a ) *  LowerTriangularMatrix<T> ( \b b ) */
template <class T> 
inline TMatrix<T> operator * (const TMatrix<T>& a, const LowerTriangularMatrix<T>& b)
   { return(b.MultiplyGT(a)); }


//---------------------------------------------------------------------------------------------------------
//------------------------------------- PART 2 : UpperTriangularMatrix ------------------------------------
//---------------------------------------------------------------------------------------------------------

/*!
  \class UpperTriangularMatrix
  \ingroup TArray
  \brief Class representing an upper triangular matrix. 

  The upper triangular matrix is mapped in memory with elements in columns packed in memory, 
  as illustrated below for a 5x5 triangular matrix.
  \verbatim
  5x5 Upper Triang.Matrix, Size= 5*(5+1)/2 = 15 elements (0 ... 14) 
  | 0   1   3   6   10  |
  |     2   4   7   11  |
  |         5   8   12  |
  |             9   13  |
  |                 14  |
  \endverbatim 

  This class offers similar functionalities to the TArray<T> / TMatrix<T> classes, like 
  reference sharing and counting, arithmetic operators ... However, this class has no 
  sub matrix extraction method.
*/
  
template <class T>
class UpperTriangularMatrix : public SpecialSquareMatrix<T> {
public :

#include "spesqmtx_tsnl.h"

//! Default constructor - TriangMatrix of size 0, SetSize() should be called before the object is used
explicit UpperTriangularMatrix()
  : SpecialSquareMatrix<T>(C_UpperTriangularMatrix)
{
  mZeros = T(0);
}

//! Instanciate a triangular matrix from the number of rows (rowSize must be > 0)
  explicit UpperTriangularMatrix(sa_size_t rowSize)
    : SpecialSquareMatrix<T>(rowSize, C_UpperTriangularMatrix)
{
  if (rowSize < 1) 
    throw ParmError("UpperTriangularMatrix<T>::UpperTriangularMatrix(rsz) rsz <= 0");
  mElems.ReSize((rowSize*(rowSize+1)/2) ); 
  mZeros = T(0);
}

//! Copy constructor (possibility of sharing datas)
UpperTriangularMatrix(UpperTriangularMatrix<T> const & a,  bool share=false)  
  : SpecialSquareMatrix<T>(a, share) 
{
  mZeros = T(0);
}

//! Copy constructor (possibility of sharing datas)
UpperTriangularMatrix(SpecialSquareMatrix<T> const & a,  bool share=false)  
  : SpecialSquareMatrix<T>(a, share) 
{
  if (a.MtxType() != C_UpperTriangularMatrix) 
    throw TypeMismatchExc("UpperTriangularMatrix(a) a NOT a UpperTriangularMatrix");
  mZeros = T(0);
}

/*!
 \brief Create a lower triangular matrix from a square matrix. 
 Elements below the diagonal are ignored.
*/
explicit UpperTriangularMatrix(TMatrix<T> const & mx)  
  : SpecialSquareMatrix<T>(C_UpperTriangularMatrix)
{
  if ((mx.NRows() != mx.NCols()) || (mx.NRows() < 1)) 
    throw ParmError("UpperTriangularMatrix<T>::(TMatrix<T> const & mx) mx not allocated OR NOT a square matrix");
  SetSize(mx.NRows());
  for(sa_size_t l=0; l<NRows(); l++) 
    for(sa_size_t m=l; m<NRows(); m++) (*this)(l,m) = mx(l,m);
  mZeros = T(0);
}

//! Sets or change the triangular matrix size, specifying the new number of rows
virtual void SetSize(sa_size_t rowSize) 
{
  if (rowSize < 1) 
    throw ParmError("UpperTriangularMatrix<T>::SetSize(rsz) rsz <= 0");
  if (rowSize == mNrows)  return;
  mNrows=rowSize;
  mElems.ReSize(mNrows*(mNrows+1)/2);  
}

//! Return the object (triangular matrix) as a standard square matrix
virtual TMatrix<T> ConvertToStdMatrix() const
{
  if (mNrows < 1)
    throw SzMismatchError("UpperTriangularMatrix<T>::ConvertToStdMatrix() (this) not allocated !");
  SOPHYA::TMatrix<T> mx(NRows(), NRows());
  for(sa_size_t l=0; l<NRows(); l++) 
    for(sa_size_t m=l; m<NRows(); m++) mx(l,m) = (*this)(l,m);
  return mx;
}
 
//--- Operateurs = (T b) , = (UpperTriangularMatrix<T> b)
//! operator = a , to set all elements to the value \b a
inline UpperTriangularMatrix<T>& operator = (T a)
  {  SetCst(a);  return (*this);  }
//! operator = UpperTriangularMatrix<T> a , element by element copy operator
inline UpperTriangularMatrix<T>& operator = (UpperTriangularMatrix<T> const & a)
  {  Set(a); return (*this); }
//! operator = Sequence seq  
inline UpperTriangularMatrix<T>& operator = (Sequence const & seq)
  {  SetSeq(seq); return (*this); }


//--- Operateurs d'acces aux elements 
//! Element access operator (R/W): access to elements row \b r and column \b c
inline T& operator()(sa_size_t r, sa_size_t c) 
{ 
  if ((r<0)||(r>=mNrows))  
    throw RangeCheckError("UpperTriangularMatrix<T>::operator()(r,c) (r<0)||(r>=NRows())");
  if (r>c) { mZeros = T(0); return mZeros; }
  /*  if (mFgRowPacked)   // the (upper triangular )matrix is stored row by row
      return(mElems(c + r*mNrows-r*(r+1)/2)); */
  // the (upper triangular )matrix is stored column by column
  return(mElems(r+c*(c+1)/2));
}
//! Element access operator (RO): access to elements row \b l and column \b m
inline T operator()(sa_size_t r, sa_size_t c) const
{ 
  if ((r<0)||(r>=mNrows))  
    throw RangeCheckError("UpperTriangularMatrix<T>::operator()(r,c) (r<0)||(r>=NRows())");
  if (r>c) { mZeros = T(0); return mZeros; }
  /*  if (mFgRowPacked)   // the (upper triangular )matrix is stored row by row
      return(mElems(c + r*mNrows-r*(r+1)/2)); */
  // the (upper triangular )matrix is stored column by column
  return(mElems(r+c*(c+1)/2));
}
//! return the element (r,c) row \b r and column \b c
virtual T getElem(sa_size_t r,sa_size_t c) const
{
  if ((r<0)||(r>=mNrows))  
    throw RangeCheckError("UpperTriangularMatrix<T>::operator()(r,c) (r<0)||(r>=NRows())");
  if (r>c) { mZeros = T(0); return mZeros; }
  return(mElems(r+c*(c+1)/2));
}

//! Return the pointer to the first non zero element in row \b j = &(tmmtx(j,j)) 
inline const T* rowData(sa_size_t j)  const 
{
  /* if (mFgRowPacked)   // the (upper triangular )matrix is stored row by row
     return mElems.Begin()+(mNrows*j-j*(j-1)/2); */
  // the (inferior triangular )matrix is stored column by column
  return mElems.Begin()+(j*(j+1)/2);
}

//! Return the pointer to the first non zero element in row \b i = &(tmmtx(i,i)) 
inline T* rowData(sa_size_t j) 
{
  /* if (mFgRowPacked)   // the (upper triangular )matrix is stored row by row
     return mElems.Begin()+(mNrows*j-j*(j-1)/2); */
  // the (inferior triangular )matrix is stored column by column
  return mElems.Begin()+(j*(j+1)/2);
}
//! compute the position of the element \b tm(i,j) relative to the first element
virtual sa_size_t indexOfElement(sa_size_t r,sa_size_t c) const 
{
  /* if (mFgRowPacked)  // the (upper triangular )matrix is stored row by row
     return (c + r*mNrows-r*(r+1)/2);  */
  // the (upper triangular )matrix is stored column by column
  return(r + c*(c+1)/2);
}

//! Triangular Matrix product (multiplication) : ret_matrix = (*this) * tmx
UpperTriangularMatrix<T> Multiply(UpperTriangularMatrix<T> const & tmx) const
{
  if (NRows() != tmx.NRows()) 
    throw SzMismatchError("Matrix<T>::Multiply(UpperTriangularMatrix<T> tmx): different sizes");
// codage peu efficace : on utilise la multiplication de matrices generales ...
  TMatrix<T> a = ConvertToStdMatrix();
  TMatrix<T> b = tmx.ConvertToStdMatrix();
  UpperTriangularMatrix<T> ret(a.Multiply(b));
  ret.SetTemp(true);
  return ret;
}

//! Matrix product (multiplication) : ret_matrix = (*this) * mx
TMatrix<T> MultiplyTG(TMatrix<T> const & mx) const 
{
  if (NCols() != mx.NRows())
    throw SzMismatchError("UpperTriangularMatrix<T>::MultiplyTG(TMatrix<T> mx): NCols()!=mx.NRows()");
  TMatrix<T> a = ConvertToStdMatrix();
  return a.Multiply(mx); 
}

//! Matrix product (multiplication) : ret_matrix = mx * (*this)
TMatrix<T> MultiplyGT(TMatrix<T> const & mx) const 
{
  if (NRows() != mx.NCols())
    throw SzMismatchError("UpperTriangularMatrix<T>::MultiplyGT(TMatrix<T> mx): NRows()!=mx.NCols()");
  TMatrix<T> a = ConvertToStdMatrix();
  return mx.Multiply(a); 
}


protected: 
mutable T mZeros; 
};

//----- Surcharge d'operateurs C = A * B (multiplication matricielle)
/*! \ingroup TArray \fn operator*(const UpperTriangularMatrix<T>&,const UpperTriangularMatrix<T>&)
  \brief * : UpperTriangularMatrix multiplication \b a and \b b */
template <class T> 
inline UpperTriangularMatrix<T> operator * (const UpperTriangularMatrix<T>& a, const UpperTriangularMatrix<T>& b)
   { return(a.Multiply(b)); }

/*! \ingroup TArray \fn operator*(const UpperTriangularMatrix<T>&,const TMatrix<T>&)
  \brief * : Matrix multiplication UpperTriangularMatrix (\b a ) *  TMatrix<T> ( \b b ) */
template <class T> 
inline TMatrix<T> operator * (const UpperTriangularMatrix<T>& a, const TMatrix<T>& b)
   { return(a.MultiplyTG(b)); }

/*! \ingroup TArray \fn operator*(const TMatrix<T>&,const UpperTriangularMatrix<T>&)
  \brief * : Matrix multiplication TMatrix (\b a ) *  UpperTriangularMatrix<T> ( \b b ) */
template <class T> 
inline TMatrix<T> operator * (const TMatrix<T>& a, const UpperTriangularMatrix<T>& b)
   { return(b.MultiplyGT(a)); }


  
}   // namespace SOPHYA

#endif
