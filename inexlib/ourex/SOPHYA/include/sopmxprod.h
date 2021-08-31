//-----------------------------------------------------------------
// Class  SOP_MatrixProduct<T> : Optimized matrix multiplication
// SOPHYA class library - (C)  UPS+LAL , IN2P3/CNRS , CEA-Irfu 
// R. Ansari (UPS/LAL) - C. Magneville (CEA-IRFU) - Sep 2014
//-----------------------------------------------------------------

#ifndef SOPMXPROD_H_SEEN
#define SOPMXPROD_H_SEEN


#include "tmatrix.h"

namespace SOPHYA {

//---------------------------------------------------------------------------------------------------
//-------- Declaration et Implementation de SOP_MatrixProduct<T> 
//---------------------------------------------------------------------------------------------------
template <class T>
class SOP_MatrixProduct {
public:
  //! if lev<0 , do not change the print level - return the print level 
  static int  SetPrintLevel(int lev=0); 
  //! optimized multi-threaded matrix product. Computes C = A*B
  static TMatrix<T>& MatrixMultiply(TMatrix<T> const& A, TMatrix<T> const& B, TMatrix<T> &C, int nthr=1);
  //! less optimized single thread matrix product. Computes C=A*B
  static TMatrix<T>& MatrixMultiplyNoOpt(TMatrix<T> const& A, TMatrix<T> const& B, TMatrix<T> &C);
  //! dot product of two vectors : res= Sum_[i=0...n] x[i*stepx]*y[i*stepy]
  static T dotproduct(size_t n, const T * x, size_t stepx, const T * y, size_t stepy);
protected:
};

//! Computes and return the matrix product C = A*B, through a call to SOP_MatrixProduct<T>::MatrixMultiply(A,B,C,nthr) 
template <class T>
inline TMatrix<T> SOP_Multiply(TMatrix<T> const& A, TMatrix<T> const& B, int nthr=1) 
  {
    TMatrix<T> C;
    SOP_MatrixProduct<T>::MatrixMultiply(A,B,C,nthr);
    return C;
  }



} // end of namespace SOPHYA 


#endif

