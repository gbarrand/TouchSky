//-----------------------------------------------------------
// class SpecialSquareMatrix<T>
// --- Special Square matrices base class 
// SOPHYA class library - (C)  UPS+LAL IN2P3/CNRS  , CEA-Irfu 
//  (C) Univ. Paris-Sud   (C) LAL-IN2P3/CNRS   (C) IRFU-CEA
//  (C) R. Ansari, C.Magneville    2009-2010  
//-----------------------------------------------------------

#ifndef SPESQMTX_H_SEEN
#define SPESQMTX_H_SEEN

#include "ndatablock.h"
#include "pexceptions.h"
#include "utilarr.h"
#include "tmatrix.h"


namespace SOPHYA {

//--- enum definissant le type de matrice carre 
//! enum definition for the differents special square matrix types (or flavours)
enum SpSqMtxType {
  C_UndefinedMatrix = 0,        //!< Base type 
  C_DiagonalMatrix = 1,         //!< Diagonal Matrix 
  C_SymmetricMatrix = 4,        //!< Symmetric Matrix
  C_LowerTriangularMatrix = 8,  //!< Lower Triangular Matrix
  C_UpperTriangularMatrix = 9   //!< Upper Triangular Matrix
};

//-- forward declaration for the PPF handler 
template <class T> class FIO_SpecialSquareMatrix;
  
template <class T>
class SpecialSquareMatrix : public AnyDataObj {
public :


// Constructor - size 0, SetSize() should be called before the object is used
explicit SpecialSquareMatrix(SpSqMtxType typ=C_UndefinedMatrix);

// Constructeur avec specification du nombre de lignes 
explicit SpecialSquareMatrix(sa_size_t rowSize, SpSqMtxType typ);

// Constructor de copy (avec partage de reference de NDataBlock) 
SpecialSquareMatrix(SpecialSquareMatrix<T> const & a,  bool share=true);  

// Allocation / changement de taille - Doit etre redefinie par les classes derivees
virtual void SetSize(sa_size_t rowSize);

//! Alias for SetSize() 
inline void ReSize(sa_size_t rowSize) 
{
  return SetSize(rowSize);
}

//! Alias for SetSize() 
inline void ReSizeRow(sa_size_t rowSize) 
{
  return SetSize(rowSize);
}

// Retourne l'objet sous forme d'une TMatrix<T> 
virtual TMatrix<T> ConvertToStdMatrix() const;

// Clone : duplique l'objet et ses donnees 
virtual void Clone(SpecialSquareMatrix<T> const & a);

// Share : recopie l'objet a avec partage des donnees des elements  
virtual void Share(SpecialSquareMatrix<T> const & a);

// Operation de Share() si objet temporaire, Clone() sinon
virtual void CloneOrShare(SpecialSquareMatrix<T> const & a);

//! Flags the associated NDataBlock as temporary if \b fgtmp==true
inline void SetTemp(bool fgtmp=false) const  { mElems.SetTemp(fgtmp); } 

//! Return the matrix type (Diagonal, Triangular ...) 
inline SpSqMtxType MtxType() const { return mType; }
//! Return number of rows
inline  sa_size_t NRows() const {return (int_4)mNrows;}
//! Return number of colums ( = NRows())
inline  sa_size_t NCols() const {return (int_4)mNrows;}

//! Return the total number of non zero elements 
inline sa_size_t Size() const { return mElems.Size();}

//! Return reference to datablock containg matrix elements NDataBlock
inline NDataBlock<T>& DataBlock()   { return mElems; }
//! Return reference to datablock containg matrix elements NDataBlock
inline const NDataBlock<T>& DataBlock() const  { return mElems; }

//! Return a reference to the associated DVList object 
inline DVList& Info()
  { if (mInfo==NULL)  mInfo = new DVList;  return (*mInfo); }

// Pour mettre tous les elements a la valeur a 
virtual SpecialSquareMatrix<T>& SetCst(T a);

//! Alias for SetT(a) - Set all matrix elements to the value \b a
inline SpecialSquareMatrix<T>& SetT(T a)
  { return SetCst(a); }

//  Egalite/Recopie element par element 
virtual SpecialSquareMatrix<T>& Set(SpecialSquareMatrix<T> const & a);

//  Remplissage avec une sequence de valeurs
virtual SpecialSquareMatrix<T>& SetSeq(Sequence const & seq);

// Ajout de la constante "b: a  tous les elements
virtual SpecialSquareMatrix<T>& AddCst(T b);

// Soustrait "b" a  tous les elements - fginv==true -> elem = b-elem
virtual SpecialSquareMatrix<T>& SubCst(T b, bool fginv=false);

// Multiplie "b" a tous les elements 
virtual SpecialSquareMatrix<T>& MulCst(T b);

// Divise tous les elements par "b". fginv==true elem = b / elem 
virtual SpecialSquareMatrix<T>& DivCst(T b, bool fginv=false);

// Addition element par element:  elem(l,c) = elem(l,c) + b.elem(l,c)
virtual SpecialSquareMatrix<T>& AddElt(SpecialSquareMatrix<T> const& b);

// Soustraction element par element : elem(l,c) = elem(l,c) - b.elem(l,c)
virtual SpecialSquareMatrix<T>& SubElt(SpecialSquareMatrix<T> const& b);

// Multiplication element par element : elem(l,c) = elem(l,c) * b.elem(l,c)
virtual SpecialSquareMatrix<T>& MulElt(SpecialSquareMatrix<T> const& b);

// Division element par element : elem(l,c) = elem(l,c) / b.elem(l,c)
virtual SpecialSquareMatrix<T>& DivElt(SpecialSquareMatrix<T> const& b);

//--- Operateurs = (T b) , = (SpecialSquareMatrix<T> b)
//! operator = a , to set all elements to the value \b a
inline SpecialSquareMatrix<T>& operator = (T a)
  {  return SetCst(a);  }
//! operator = SpecialSquareMatrix<T> a , element by element copy operator
inline SpecialSquareMatrix<T>& operator = (SpecialSquareMatrix<T> const & a)
  {  return Set(a); }
//! operator = Sequence seq , 
inline SpecialSquareMatrix<T>& operator = (Sequence const & seq)
  {  return SetSeq(seq); }

//--- Operateur += -= *= /= (T b)
//! operator += b , Add a constant value \b b to all elements
inline SpecialSquareMatrix<T>& operator += (T b)
  { return AddCst(b); }
//! operator -= b , subtract a constant value \b b from all elements
inline SpecialSquareMatrix<T>& operator -= (T b)
  { return SubCst(b); }
//! operator *= b , multiply all elements by  a constant value \b b 
inline SpecialSquareMatrix<T>& operator *= (T b)
  { return MulCst(b); }
//! operator /= b , divide all elements by  a constant value \b b 
inline SpecialSquareMatrix<T>& operator /= (T b)
  { return DivCst(b); }

//! Element access operator (R/W): access to the element at position \b index - No bound checking
inline T& operator[](sa_size_t index) 
  { return  mElems(index); }
//! Element access operator (RO): access to the element at position \b index - No bound checking
inline T operator[](sa_size_t index) const
  { return  mElems(index); }

/*! 
  \brief return the element (r,c) of the matrix
  should be implemented in derived classes 
*/
virtual T getElem(sa_size_t r,sa_size_t c) const;

// Valeur min et max des elements (sauf tableaux complexes -> exception)
virtual void MinMax(T& min, T& max) const ;

// Impression / representation texte sur le flot os
virtual std::ostream& Show(std::ostream& os) const;

//! ASCII dump/print of the triangular matrix object (set nbLignes=-1 for dumping the complete matrix)
std::ostream& Print(std::ostream& os, sa_size_t nbLignes=0) const;

friend class FIO_SpecialSquareMatrix<T>;
//..............
protected: 
SpSqMtxType mType;        //!< Type of the square matrix (diagonal, symmetric, triangular)
sa_size_t mNrows;         //!< size of the square matrix
NDataBlock<T> mElems;     //!< Data block
DVList* mInfo;            //!< Associated DVList structure
};

//! print (ascii representation) operator
template <class T>
inline std::ostream& operator << (std::ostream& os, const SpecialSquareMatrix<T>& a)
  { a.Show(os);    return(os);    }

////////////////////////////////////////////////////////////////
// Surcharge d'operateurs A (+,-,*,/) (T) x

/*! \ingroup TArray \fn operator+(const SpecialSquareMatrix<T>&,T)
  \brief Operator SpecialSquareMatrix = SpecialSquareMatrix + constant */
template <class T> inline SpecialSquareMatrix<T> operator + (const SpecialSquareMatrix<T>& a, T b)
  { SpecialSquareMatrix<T> result; result.CloneOrShare(a); 
    result.SetTemp(true);  result.AddCst(b); return result; }

/*! \ingroup TArray \fn operator+(T,const SpecialSquareMatrix<T>&)
  \brief Operator SpecialSquareMatrix = constant + SpecialSquareMatrix */
template <class T> inline SpecialSquareMatrix<T> operator + (T b,const SpecialSquareMatrix<T>& a)
  { SpecialSquareMatrix<T> result; result.CloneOrShare(a); 
    result.SetTemp(true);  result.AddCst(b); return result; }

/*! \ingroup TArray \fn operator-(const SpecialSquareMatrix<T>&,T)
  \brief Operator SpecialSquareMatrix = SpecialSquareMatrix - constant */
template <class T> inline SpecialSquareMatrix<T> operator - (const SpecialSquareMatrix<T>& a, T b)
  { SpecialSquareMatrix<T> result; result.CloneOrShare(a); 
    result.SetTemp(true);  result.SubCst(b); return result; }

/*! \ingroup TArray \fn operator-(T,const SpecialSquareMatrix<T>&)
  \brief Operator SpecialSquareMatrix = constant - SpecialSquareMatrix */
template <class T> inline SpecialSquareMatrix<T> operator - (T b,const SpecialSquareMatrix<T>& a)
  { SpecialSquareMatrix<T> result; result.CloneOrShare(a); 
    result.SetTemp(true);  result.SubCst(b, true); return result; }

/*! \ingroup TArray \fn operator*(const SpecialSquareMatrix<T>&,T)
  \brief Operator SpecialSquareMatrix = SpecialSquareMatrix * constant */
template <class T> inline SpecialSquareMatrix<T> operator * (const SpecialSquareMatrix<T>& a, T b)
  { SpecialSquareMatrix<T> result; result.CloneOrShare(a); 
    result.SetTemp(true);  result.MulCst(b); return result; }

/*! \ingroup TArray \fn operator*(T,const SpecialSquareMatrix<T>&)
  \brief Operator SpecialSquareMatrix = constant * SpecialSquareMatrix */
template <class T> inline SpecialSquareMatrix<T> operator * (T b,const SpecialSquareMatrix<T>& a)
  { SpecialSquareMatrix<T> result; result.CloneOrShare(a); 
    result.SetTemp(true);  result.MulCst(b); return result; }

/*! \ingroup TArray \fn operator/(const SpecialSquareMatrix<T>&,T)
  \brief Operator SpecialSquareMatrix = SpecialSquareMatrix / constant */
template <class T> inline SpecialSquareMatrix<T> operator / (const SpecialSquareMatrix<T>& a, T b)
  { SpecialSquareMatrix<T> result; result.CloneOrShare(a); 
    result.SetTemp(true);  result.DivCst(b); return result; }

/*! \ingroup TArray \fn operator/(T,const SpecialSquareMatrix<T>&)
  \brief Operator SpecialSquareMatrix = constant / SpecialSquareMatrix  */
template <class T> inline SpecialSquareMatrix<T> operator / (T b, const SpecialSquareMatrix<T>& a)
  { SpecialSquareMatrix<T> result; result.CloneOrShare(a); 
    result.SetTemp(true);  result.DivCst(b, true); return result; }


////////////////////////////////////////////////////////////////
// Surcharge d'operateurs C = A (+,-,&&,/) B

/*! \ingroup TArray \fn operator+(const SpecialSquareMatrix<T>&,const SpecialSquareMatrix<T>&)
  \brief Operator SpecialSquareMatrix = SpecialSquareMatrix + SpecialSquareMatrix (element by element addition) */
template <class T>
inline SpecialSquareMatrix<T> operator + (const SpecialSquareMatrix<T>& a,const SpecialSquareMatrix<T>& b)
  { SpecialSquareMatrix<T> result; result.CloneOrShare(a); 
    result.SetTemp(true);  result.AddElt(b); return result; }

/*! \ingroup TArray \fn operator-(const SpecialSquareMatrix<T>&,const SpecialSquareMatrix<T>&)
  \brief Operator SpecialSquareMatrix = SpecialSquareMatrix - SpecialSquareMatrix (element by element subtraction) */
template <class T>
inline SpecialSquareMatrix<T> operator - (const SpecialSquareMatrix<T>& a,const SpecialSquareMatrix<T>& b)
  { SpecialSquareMatrix<T> result; result.CloneOrShare(a); 
    result.SetTemp(true);  result.SubElt(b); return result; }

/*! \ingroup TArray \fn operator && (const SpecialSquareMatrix<T>&,const SpecialSquareMatrix<T>&)
  \brief Element by element multiplication of two arrays SpecialSquareMatrix = SpecialSquareMatrix * SpecialSquareMatrix 

  \warning Notice the usage of the operator && (usually a logical operator) to distinguish element wise operation from matrix product,
  as to TMatrix objects 
*/

template <class T>
inline SpecialSquareMatrix<T> operator && (const SpecialSquareMatrix<T>& a,const SpecialSquareMatrix<T>& b)
  { SpecialSquareMatrix<T> result; result.CloneOrShare(a); 
    result.SetTemp(true);  result.MulElt(b); return result; }

/*! \ingroup TArray \fn operator / (const SpecialSquareMatrix<T>&,const SpecialSquareMatrix<T>&)
  \brief Element by element division of two arrays SpecialSquareMatrix = SpecialSquareMatrix / SpecialSquareMatrix */
template <class T>
inline SpecialSquareMatrix<T> operator / (const SpecialSquareMatrix<T>& a,const SpecialSquareMatrix<T>& b)
  { SpecialSquareMatrix<T> result; result.CloneOrShare(a); 
    result.SetTemp(true);  result.DivElt(b); return result; }

//--------- extern template declarations (if needed) -----------
#if defined ( NEED_EXT_DECL_TEMP ) && !defined( SPESQMTX_CC_BFILE )
extern template class SpecialSquareMatrix<uint_1>;
extern template class SpecialSquareMatrix<uint_2>;
extern template class SpecialSquareMatrix<uint_4>;
extern template class SpecialSquareMatrix<uint_8>;
extern template class SpecialSquareMatrix<int_1>;
extern template class SpecialSquareMatrix<int_2>;
extern template class SpecialSquareMatrix<int_4>;
extern template class SpecialSquareMatrix<int_8>;
extern template class SpecialSquareMatrix<r_4>;
extern template class SpecialSquareMatrix<r_8>;
extern template class SpecialSquareMatrix< std::complex<r_4> >;
extern template class SpecialSquareMatrix< std::complex<r_8> >;
#ifdef SO_LDBLE128
extern template class SpecialSquareMatrix<r_16>;
extern template class SpecialSquareMatrix< std::complex<r_16> >;
#endif
#endif  // Fin de if defined ( NEED_EXT_DECL_TEMP )
  
}   // namespace SOPHYA

#endif
