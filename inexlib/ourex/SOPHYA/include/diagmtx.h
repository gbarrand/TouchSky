// This may look like C code, but it is really -*- C++ -*-
// This code is part of the SOPHYA library
//  (C) Univ. Paris-Sud   (C) LAL-IN2P3/CNRS   (C) IRFU-CEA
//  (C) R. Ansari, C.Magneville    2009-2010  

#ifndef DIAGMTX_H_SEEN
#define DIAGMTX_H_SEEN

#include "spesqmtx.h"
#include "tvector.h"
#include <vector> 

namespace SOPHYA {

/*!
  \class DiagonalMatrix
  \ingroup TArray
  \brief Class representing a diagonal matrix. 

  This class offers similar functionalities to the TArray<T> / TMatrix<T> classes, like 
  reference sharing and counting, arithmetic operators ... However, this class has no 
  sub matrix extraction method.
*/

template <class T>
class DiagonalMatrix : public SpecialSquareMatrix<T> {
public :

#include "spesqmtx_tsnl.h"

//! Default constructor - TriangMatrix of size 0, SetSize() should be called before the object is used
explicit DiagonalMatrix()
  : SpecialSquareMatrix<T>(C_DiagonalMatrix) 
{ 
  mOffDiag = T(0);
}

//! Instanciate a diagonal nxn matrix, setting diagonal elements to dv  (n must be > 0)
explicit DiagonalMatrix(sa_size_t n, T dv=T(0))
  : SpecialSquareMatrix<T>(n, C_DiagonalMatrix)
{
  if (n < 1) 
    throw ParmError("DiagonalMatrix<T>::DiagonalMatrix(n) n <= 0");
  mElems.ReSize(n,false); 
  mElems = dv;
  mInfo = NULL;
  mOffDiag = T(0);
}

//! Copy constructor (possibility of sharing datas)
DiagonalMatrix(DiagonalMatrix<T> const & a,  bool share=false)  
  : SpecialSquareMatrix<T>(a, share) 
{
  mOffDiag = T(0);
}

//! Copy constructor (possibility of sharing datas)
DiagonalMatrix(SpecialSquareMatrix<T> const & a,  bool share=false)  
  : SpecialSquareMatrix<T>(a, share) 
{
  if (a.MtxType() != C_DiagonalMatrix) 
    throw TypeMismatchExc("DiagonalMatrix(a) a NOT a DiagonalMatrix");
  mOffDiag = T(0);
}

/*!
 \brief Create a diagonal matrix from the TVector<T> \vd defining the diagonal elements
*/
explicit DiagonalMatrix(TVector<T> const & vd)  
  : SpecialSquareMatrix<T>(vd.Size(), C_DiagonalMatrix)
{
  if (vd.Size() < 1) 
    throw ParmError("DiagonalMatrix<T>::(TVector<T> const & vd) vd not allocated");
  mElems.ReSize(vd.Size(),false); 
  for(sa_size_t l=0; l<NRows(); l++) (*this)(l,l) = vd(l);
  mOffDiag = T(0);
}

/*!
 \brief Create a diagonal matrix from the std::vector<T> \vd defining the diagonal elements
*/
explicit DiagonalMatrix(std::vector<T> const & vd)  
  : SpecialSquareMatrix<T>(vd.size(), C_DiagonalMatrix)
{
  if (vd.size() < 1) 
    throw ParmError("DiagonalMatrix<T>::(vector<T> const & vd) vd has zero size");
  mElems.ReSize(vd.size(),false); 
  for(sa_size_t l=0; l<NRows(); l++) (*this)(l,l) = vd[l];
  mOffDiag = T(0);
}

/*!
 \brief Create a diagonal matrix from a square matrix. 
 Off diagonal elements are ignored.
*/
explicit DiagonalMatrix(TMatrix<T> const & mx)  
  : SpecialSquareMatrix<T>(mx.NRows(), C_DiagonalMatrix)
{
  if ((mx.NRows() != mx.NCols()) || (mx.NRows() < 1)) 
    throw ParmError("DiagonalMatrix<T>::(TMatrix<T> const & mx) mx not allocated OR NOT a square matrix");
  mElems.ReSize(mx.NRows(),false); 
  for(sa_size_t l=0; l<NRows(); l++) (*this)(l,l) = mx(l,l);
  mOffDiag = T(0);
}

//! Sets or change the diagonal matrix size, specifying the new number of rows
virtual void SetSize(sa_size_t rowSize) 
{
  if (rowSize < 1) 
    throw ParmError("DiagonalMatrix<T>::SetSize(rsz) rsz <= 0");
  if (rowSize == mNrows)  return;
  mNrows=rowSize;
  mElems.ReSize(mNrows);  
}


//! Return the object (diagonal matrix) as a standard (TMatrix<T>) square matrix
virtual TMatrix<T> ConvertToStdMatrix() const
{
  if (mNrows < 1)
    throw SzMismatchError("DiagonalMatrix<T>::ConvertToStdMatrix() (this) not allocated !");
  TMatrix<T> mx(NRows(), NRows());
  for(sa_size_t l=0; l<NRows(); l++) mx(l,l) = (*this)(l,l);
  return mx;
}
 

//--- Operateurs = (T b) , = (DiagonalMatrix<T> b)
//! operator = a , to set all elements to the value \b a
inline DiagonalMatrix<T>& operator = (T a)
  {  SetCst(a);  return (*this);  }
//! operator = DiagonalMatrix<T> a , element by element copy operator
inline DiagonalMatrix<T>& operator = (DiagonalMatrix<T> const & a)
  {  Set(a); return (*this); }
//! operator = Sequence seq  
inline DiagonalMatrix<T>& operator = (Sequence const & seq)
  {  SetSeq(seq); return (*this); }
//! operator = Sequence seq  
inline DiagonalMatrix<T>& operator = (IdentityMatrix & idmx)
  {  SetCst((T)(idmx.Diag())); return (*this); }

//--- Operateurs d'acces aux elements 
//! Element access operator (R/W): access to elements row \b r and column \b c
inline T& operator()(sa_size_t r, sa_size_t c) 
{ 
  if ((r<0)||(r>=mNrows))  
    throw RangeCheckError("DiagonalMatrix<T>::operator()(r,c) (r<0)||(r>=NRows())");
  if (r!=c) { mOffDiag = T(0); return mOffDiag; }
  return mElems(r);
}
//! Element access operator (RO): access to elements row \b r and column \b c
inline T operator()(sa_size_t r, sa_size_t c) const
{
  if ((r<0)||(r>=mNrows))  
    throw RangeCheckError("DiagonalMatrix<T>::operator()(r,c) (r<0)||(r>=NRows())");
  if (r!=c) { mOffDiag = T(0); return mOffDiag; }
  return mElems(r);
}

//! return the element (r,c) row \b r and column \b c
virtual T getElem(sa_size_t r,sa_size_t c) const
{
  if ((r<0)||(r>=mNrows))  
    throw RangeCheckError("DiagonalMatrix<T>::operator()(r,c) (r<0)||(r>=NRows())");
  if (r!=c) { mOffDiag = T(0); return mOffDiag; }
  return mElems(r);
}

//! Diagonal Matrix product (multiplication) : ret_matrix = (*this) * dmx
DiagonalMatrix<T> Multiply(DiagonalMatrix<T> const & dmx) const
{
  if (NRows() != dmx.NRows()) 
    throw SzMismatchError("DiagonalMatrix<T>::Multiply(DiagonalMatrix<T> dmx): different sizes");
  DiagonalMatrix<T> ret(NRows());
  for(size_t k=0; k<mElems.Size(); k++) 
    ret.mElems(k) = mElems(k)*dmx.mElems(k);
  ret.SetTemp(true);
  return ret;
}

//! Matrix product (multiplication) : ret_matrix = (*this) * mx
TMatrix<T> MultiplyDG(TMatrix<T> const & mx) const 
{
  if (NCols() != mx.NRows())
    throw SzMismatchError("DiagonalMatrix<T>::MultiplyDG(TMatrix<T> mx): NCols()!=mx.NRows()");
    
  TMatrix<T> ret(mx, false);
  for(sa_size_t r=0; r<NRows(); r++) 
    ret.Row(r) *= (*this)(r,r);
  ret.SetTemp(true);
  return ret;
}

//! Matrix product (multiplication) : ret_matrix = mx * (*this)
TMatrix<T> MultiplyGD(TMatrix<T> const & mx) const
{
  if (NRows() != mx.NCols())
    throw SzMismatchError("DiagonalMatrix<T>::MultiplyGD(TMatrix<T> mx): NRows()!=mx.NCols()");
    
  TMatrix<T> ret(mx, false);
  for(sa_size_t c=0; c<NRows(); c++) 
    ret.Column(c) *= (*this)(c,c);
  ret.SetTemp(true);
  return ret;
}


//! ASCII dump/print of the diagonal element (nprt=-1 for printing all diagonal elements)
std::ostream& PrintDiag(std::ostream& os, sa_size_t nprt=-1) const
{
  os << "DiagonalMatrix< " << typeid(T).name() 
     << " > NRow=" << mNrows << " NbElem<>0 : " << Size() << std::endl;
  for(sa_size_t k=0; k<Size(); k+=8) {
    if (k%32==0) os << "DiagonalElements: [ " << k << " ..." << k+31 <<" ] :" << std::endl;
    sa_size_t jmx=k+8;
    if (jmx>Size()) jmx = Size();
    for(sa_size_t j=k; j<jmx; j++)  os << mElems(j) << " , ";
    os << std::endl; 
    if (k >= nprt)  break;
  } 
  return os;
}

protected: 
mutable T mOffDiag;
};

//----- Surcharge d'operateurs C = A * B (multiplication matricielle)
/*! \ingroup TArray \fn operator*(const DiagonalMatrix<T>&,const DiagonalMatrix<T>&)
  \brief * : DiagonalMatrix multiplication \b a and \b b */
template <class T> 
inline DiagonalMatrix<T> operator * (const DiagonalMatrix<T>& a, const DiagonalMatrix<T>& b)
   { return(a.Multiply(b)); }

/*! \ingroup TArray \fn operator*(const DiagonalMatrix<T>&,const TMatrix<T>&)
  \brief * : Matrix multiplication DiagonalMatrix (\b a ) *  TMatrix<T> ( \b b ) */
template <class T> 
inline TMatrix<T> operator * (const DiagonalMatrix<T>& a, const TMatrix<T>& b)
   { return(a.MultiplyDG(b)); }

/*! \ingroup TArray \fn operator*(const TMatrix<T>&,const DiagonalMatrix<T>&)
  \brief * : Matrix multiplication TMatrix (\b a ) *  DiagonalMatrix<T> ( \b b ) */
template <class T> 
inline TMatrix<T> operator * (const TMatrix<T>& a, const DiagonalMatrix<T>& b)
   { return(b.MultiplyGD(a)); }

  
}   // namespace SOPHYA

#endif
