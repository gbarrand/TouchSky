// This may look like C code, but it is really -*- C++ -*-
// This code is part of the SOPHYA library
//  (C) Univ. Paris-Sud   (C) LAL-IN2P3/CNRS   (C) IRFU-CEA
//  (C) R. Ansari, C.Magneville    2009-2010  

#ifndef SYMMTX_H_SEEN
#define SYMMTX_H_SEEN

#include "spesqmtx.h"

namespace SOPHYA {

/*!
  \class SymmetricMatrix
  \ingroup TArray
  \brief Class representing a symmetric matrix. 

  The symmetric matrix is represented in memory as column packed,
  corresponding to the lower triangular part, as illustrated below for a 5x5 matrix.
  \verbatim
  5x5 symmetric.Matrix, Size= 5*(5+1)/2 = 15 independent elements (0 ... 14) 
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
class SymmetricMatrix : public SpecialSquareMatrix<T> {
public :

#include "spesqmtx_tsnl.h"

//! Default constructor - TriangMatrix of size 0, SetSize() should be called before the object is used
explicit SymmetricMatrix()
  : SpecialSquareMatrix<T>(C_SymmetricMatrix)
{

}

//! Instanciate a triangular matrix from the number of rows (rowSize must be > 0)
explicit SymmetricMatrix(sa_size_t rowSize)
  : SpecialSquareMatrix<T>(rowSize, C_SymmetricMatrix)
{
  if (rowSize < 1) 
    throw ParmError("SymmetricMatrix<T>::SymmetricMatrix(rsz) rsz <= 0");
  mElems.ReSize((rowSize*(rowSize+1)/2) ); 
  mInfo = NULL;
}

//! Copy constructor (possibility of sharing datas)
SymmetricMatrix(SymmetricMatrix<T> const & a,  bool share=false)  
  : SpecialSquareMatrix<T>(a, share) 
{
}

//! Copy constructor (possibility of sharing datas)
SymmetricMatrix(SpecialSquareMatrix<T> const & a,  bool share=false)  
  : SpecialSquareMatrix<T>(a, share) 
{
  if (a.MtxType() != C_SymmetricMatrix) 
    throw TypeMismatchExc("SymmetricMatrix(a) a NOT a SymmetricMatrix");
}

/*!
 \brief Create a lower triangular matrix from a square matrix. 
 Elements above the diagonal are ignored.
*/
explicit SymmetricMatrix(TMatrix<T> const & mx)  
  : SpecialSquareMatrix<T>(C_SymmetricMatrix)
{
  if ((mx.NRows() != mx.NCols()) || (mx.NRows() < 1)) 
    throw ParmError("SymmetricMatrix<T>::(TMatrix<T> const & mx) mx not allocated OR NOT a square matrix");
  SetSize(mx.NRows());
  for(sa_size_t l=0; l<NRows(); l++) 
    for(sa_size_t m=0; m<=l; m++) (*this)(l,m) = mx(l,m);
}

//! Sets or change the triangular matrix size, specifying the new number of rows
virtual void SetSize(sa_size_t rowSize) 
{
  if (rowSize < 1) 
    throw ParmError("SymmetricMatrix<T>::SetSize(rsz) rsz <= 0");
  if (rowSize == mNrows)  return;
  mNrows=rowSize;
  mElems.ReSize(mNrows*(mNrows+1)/2);  
}

//! Return the object (triangular matrix) as a standard square matrix
virtual TMatrix<T> ConvertToStdMatrix() const
{
  if (mNrows < 1)
    throw SzMismatchError("SymmetricMatrix<T>::ConvertToStdMatrix() (this) not allocated !");
  SOPHYA::TMatrix<T> mx(NRows(), NRows());
  for(sa_size_t l=0; l<NRows(); l++) 
    for(sa_size_t m=0; m<=l; m++) mx(l,m) = mx(m,l) = (*this)(l,m);
  return mx;
}
 
//--- Operateurs = (T b) , = (SymmetricMatrix<T> b)
//! operator = a , to set all elements to the value \b a
inline SymmetricMatrix<T>& operator = (T a)
  {  SetCst(a);  return (*this);  }
//! operator = SymmetricMatrix<T> a , element by element copy operator
inline SymmetricMatrix<T>& operator = (SymmetricMatrix<T> const & a)
  {  Set(a); return (*this); }
//! operator = Sequence seq  
inline SymmetricMatrix<T>& operator = (Sequence const & seq)
  {  SetSeq(seq); return (*this); }


//--- Operateurs d'acces aux elements 
//! Element access operator (R/W): access to elements row \b r and column \b c
inline T& operator()(sa_size_t r, sa_size_t c) 
{ 
  if ((r<0)||(r>=mNrows))  
    throw RangeCheckError("DiagonalMatrix<T>::operator()(r,c) (r<0)||(r>=NRows())");
  if (c>r) { sa_size_t rc = r; r=c; c=rc; }
  // the inferior triangular part of the matrix is stored column by column
  return(mElems(r+ mNrows*c-c*(c+1)/2));
}
//! Element access operator (RO): access to elements row \b l and column \b m
inline T operator()(sa_size_t r, sa_size_t c) const
{ 
  if ((r<0)||(r>=mNrows))  
    throw RangeCheckError("DiagonalMatrix<T>::operator()(r,c) (r<0)||(r>=NRows())");
  if (c>r) { sa_size_t rc = r; r=c; c=rc; }
  // the inferior triangular part of the matrix is stored column by column
  return(mElems(r+ mNrows*c-c*(c+1)/2));
}
//! return the element (r,c) row \b r and column \b c
virtual T getElem(sa_size_t r,sa_size_t c) const
{
  if ((r<0)||(r>=mNrows))  
    throw RangeCheckError("DiagonalMatrix<T>::operator()(r,c) (r<0)||(r>=NRows())");
  if (c>r) { sa_size_t rc = r; r=c; c=rc; }
  // the inferior triangular part of the matrix is stored column by column
  return(mElems(r+ mNrows*c-c*(c+1)/2));
}

//! Return the pointer to the first non zero element in column \b j = &(tmmtx(j,j)) 
inline const T* columnData(sa_size_t j)  const {return mElems.Begin()+(mNrows*j-j*(j-1)/2) ;}

//! Return the pointer to the first non zero element in column \b j = &(tmmtx(j,j)) 
inline T* columnData(sa_size_t j) {return mElems.Begin()+(mNrows*j-j*(j-1)/2) ;}

//! compute the position of the element \b tm(i,j) relative to the first element
inline sa_size_t indexOfElement(sa_size_t i,sa_size_t j) const 
{
  //  return(i*(i+1)/2+j);
  // the (inferior triangular )matrix is stored column by column
  return(i+ mNrows*j-j*(j+1)/2);
}

//! Triangular Matrix product (multiplication) : ret_matrix = (*this) * tmx
TMatrix<T> Multiply(SymmetricMatrix<T> const & tmx) const
{
  if (NRows() != tmx.NRows()) 
    throw SzMismatchError("Matrix<T>::Multiply(SymmetricMatrix<T> tmx): different sizes");
// codage peu efficace : on utilise la multiplication de matrices generales ...
  TMatrix<T> a = ConvertToStdMatrix();
  TMatrix<T> b = tmx.ConvertToStdMatrix();
  return (a.Multiply(b));
}

//! Matrix product (multiplication) : ret_matrix = (*this) * mx
TMatrix<T> MultiplySG(TMatrix<T> const & mx) const 
{
  if (NCols() != mx.NRows())
    throw SzMismatchError("SymmetricMatrix<T>::MultiplySG(TMatrix<T> mx): NCols()!=mx.NRows()");
  TMatrix<T> a = ConvertToStdMatrix();
  return a.Multiply(mx); 
}

//! Matrix product (multiplication) : ret_matrix = mx * (*this)
TMatrix<T> MultiplyGS(TMatrix<T> const & mx) const 
{
  if (NRows() != mx.NCols())
    throw SzMismatchError("SymmetricMatrix<T>::MultiplyGS(TMatrix<T> mx): NRows()!=mx.NCols()");
  TMatrix<T> a = ConvertToStdMatrix();
  return mx.Multiply(a); 
}


protected: 
};

//----- Surcharge d'operateurs C = A * B (multiplication matricielle)
/*! \ingroup TArray \fn operator*(const SymmetricMatrix<T>&,const SymmetricMatrix<T>&)
  \brief * : SymmetricMatrix multiplication \b a and \b b */
template <class T> 
inline TMatrix<T> operator * (const SymmetricMatrix<T>& a, const SymmetricMatrix<T>& b)
   { return(a.Multiply(b)); }

/*! \ingroup TArray \fn operator*(const SymmetricMatrix<T>&,const TMatrix<T>&)
  \brief * : Matrix multiplication SymmetricMatrix (\b a ) *  TMatrix<T> ( \b b ) */
template <class T> 
inline TMatrix<T> operator * (const SymmetricMatrix<T>& a, const TMatrix<T>& b)
   { return(a.MultiplySG(b)); }

/*! \ingroup TArray \fn operator*(const TMatrix<T>&,const SymmetricMatrix<T>&)
  \brief * : Matrix multiplication TMatrix (\b a ) *  SymmetricMatrix<T> ( \b b ) */
template <class T> 
inline TMatrix<T> operator * (const TMatrix<T>& a, const SymmetricMatrix<T>& b)
   { return(b.MultiplyGS(a)); }

  
}   // namespace SOPHYA

#endif
