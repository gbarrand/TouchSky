// This may look like C code, but it is really -*- C++ -*-
// Gestion de block de donnees avec partage de references
//                         C.Magneville          04/99
// LAL (Orsay) / IN2P3-CNRS  DAPNIA/SPP (Saclay) / CEA
#ifndef NDATABLOCK_H
#define NDATABLOCK_H

#include "machdefs.h"
#include <stdlib.h>        /* pour que size_t soit defini */
#include "anydataobj.h"
#include "sunitpcst.h"
#include <complex>
#include <iostream>

namespace SOPHYA {

////////////////////////////////////////////////////////////////
//// ------------------- Class Bridge ----------------------- //
////////////////////////////////////////////////////////////////
/*!
  \class Bridge
  \ingroup BaseTools
  This class is use by NDataBlock. It allows sharing of datas
  with external structures : by example, if you want to connect
  a Blitz data structure with a NDataBlock or a TMatrix or ...
  \sa NDataBlock
*/

// Classe pour permettre de partager des donnees avec 
// un autre systeme de gestion de references (ex avec Blitz)
//! Empty class which allows data sharing with external structures (for NDataBlock)
class Bridge {
public:
  Bridge() { } 
  virtual ~Bridge() { }
};


class ThSafeOp; //forward class declaration for ThreadSafe operations (Ref.Count/Share) 

////////////////////////////////////////////////////////////////
//// ----------------- Class NDataBlock --------------------- //
////////////////////////////////////////////////////////////////
// classe de container avec partage de reference
//! Container of data with reference sharing
template <class T>
class NDataBlock : public AnyDataObj {

public:

  // Methodes statiques pour debug.
  static void SetPrintDebug(int prtdbglevel=1);
  static void ResetDebug(size_t nallocdata=0, size_t nallocsref=0);
  static void PrintDebug();

  // Creation / destruction 
  NDataBlock(size_t n, bool fzero=true);
  NDataBlock(size_t n, T* data, Bridge* br=NULL);
  NDataBlock();
  NDataBlock(const NDataBlock<T>& a);
  NDataBlock(const NDataBlock<T>& a,bool share);
  virtual ~NDataBlock();

  // Temporaire?
  //! Return true if data block is temporay
  inline bool IsTemp(void) const {return mIsTemp;}
  //! Set temporary caracter of data block - method made INACTIVE (IsTemp()=false always) in Aug. 2019
  inline void SetTemp(bool temp=false) const {mIsTemp = false;} // {mIsTemp = temp;}
  // Depuis que le createur par copie partage les donnees
  // la seule utilisation de SetTemp et pour faire les operations du type:
  // NDataBlock = NDataBlock + NDataBlock + NDataBlock ...
  // --- Aout 2019/ Reza : methode rendue inactive suite probleme dans les operations sur les tableaux 
  // le constructeur de copie ne semble plus appele et on se trouve alors avec des tableaux ou NDataBlock
  // qui ont le flag IsTemp() a true, par exemple  C ds NDataBlock C = A+B  
   

  // Gestion taille/Remplissage
  void Clone(const NDataBlock<T>& a);
  void CloneOrShare(const NDataBlock<T>& a);
  void Share(const NDataBlock<T>& a);
  void FillFrom(size_t n,T* data);
  //! Re-set all data values to \b v
  inline void Reset(T v=0)
         {if(mSz==0) return; T *p=Begin(),*pe=End(); while(p<pe) *p++=v;}

  // ReSize redimmensionne une structure pour "n" donnees.
  // Les donnees precedentes sont perdues (pour cette classe)
  // et le nouveau tableau mis a zero si fzero=true. La nouvelle structure de
  // donnees n'a qu'une reference (celle de cette classe).
  //! Re-size the data structure
  /*! Old datas are lost (for this class). The new values are set 
    to zero if \b fzero=true .
    The new data structure has only one reference (itself!). */
  inline void ReSize(size_t n, bool fzero=true) {Alloc(n,NULL,NULL,fzero);}

  void Realloc(size_t nnew,bool force=false);
  
  //  Public, thread safe interface to Delete() (set the size to zero)
  void Dealloc();

  // Informations pointeur/data
  //! Return pointer on data structure.
  inline T* Data()
         {if(mSRef) return mSRef->data; else return NULL;}
  //! Return pointer on data structure.
  inline T* Data() const
         {if(mSRef) return mSRef->data; else return NULL;}
  //! Return the size of the data structure (alias for Size())
  inline size_t size() const    {return mSz;}
  //! Return the size of the data structure (alias for size())
  inline size_t Size() const    {return mSz;}
  //! operator[] overloading: Return the \b i th element of  the data structure as reference 
  inline T& operator[](size_t i)       {return *(mSRef->data+i);}
  //! operator[] overloading (const version): Return the \b i th element of the data structure as a value
  inline T  operator[](size_t i) const {return *(mSRef->data+i);}
  //! Return the \b i th element of  the data structure
  inline T& operator()(size_t i)       {return *(mSRef->data+i);}
  //! Return the \b i th element of  the data structure
  inline T  operator()(size_t i) const {return *(mSRef->data+i);}

  //! Return pointer to the beginning of the data structure.
  inline T*        begin()        {return mSRef->data;}
  //! Return pointer to the beginning of the data structure.
  inline T const*  begin() const  {return mSRef->data;} 
  //! Return pointer to the beginning of the data structure (alias for Begin()).
  inline T*        Begin()        {return mSRef->data;}
  //! Return pointer to the beginning of the data structure (alias for Begin()).
  inline T const*  Begin() const  {return mSRef->data;} 

  //! Return pointer to the end of the data structure. 
  inline T*        end()          {return mSRef->data+mSz;}
  //! Return pointer to the end of the data structure.
  inline T const*  end() const    {return mSRef->data+mSz;}
  //! Return pointer to the end of the data structure (alias for end()).
  inline T*        End()          {return mSRef->data+mSz;}
  //! Return pointer to the end of the data structure (alias for end()).
  inline T const*  End() const    {return mSRef->data+mSz;}

  //! Return the number of references to the data structure
  inline size_t NRef() const {if(mSRef) return mSRef->nref;  else return 0; }

  //! Define or change the physical unit of the data 
  void SetUnits(const Units& un);
  //! Return the physical unit of the data. Flag sdone is set to true if SetUnits() has been called.
  Units GetUnits(bool& sdone) const;
  //! Return the physical unit of the data 
  inline Units GetUnits() const  {bool fg; return GetUnits(fg); }

  // Impression
  void Print(std::ostream& os, size_t i1=0,size_t n=10) const;
  //! print infos and datas (from \b i1 to \b i2) on stdout.
  inline void Print(size_t i1=0,size_t n=0) const {Print(std::cout,i1,n);}

  // retourne la somme de n elements, commencant a l'element i1 (defaut:tous les elements) 
  T Sum(size_t i1=0,size_t n=0) const;
  // retourne la somme de n elements, commencant a l'element i1 (defaut:tous les elements)
  T Product(size_t i1=0,size_t n=0) const;
  // retourne la somme des carres de n elements, commencant a l'element i1 (defaut:tous les elements)
  T SumSq(size_t i1=0,size_t n=0) const;
// Valeur min et max de de n elements, commencant a l'element i1 (defaut:tous les elements)
  void MinMax(T& min, T& max, size_t i1=0,size_t n=0) const ;

  // Surcharge d'operateurs INPLACE: A = x , A = B , A @= x , A @= B
  NDataBlock<T>& operator = (const NDataBlock<T>& a);
  NDataBlock<T>& operator = (T v);

  //---- In place arithmetic operations with a constant 
  //! Add the constant b to all array elements 
  NDataBlock<T>& AddCst(T b); 
  //! In place constant addition operator: Add the constant b to all array elements (ND += b)
  inline NDataBlock<T>& operator += (T b)  { return AddCst(b); }
  //! subtract the constant b from all array elements (or if fginv==true ND = b - ND) 
  NDataBlock<T>& SubCst(T b,bool fginv=false); 
  //! In place constant subtraction operator: Subtract the constant b to all array elements (ND -= b)
  inline NDataBlock<T>& operator -= (T b)  { return SubCst(b,false); }
  //! Multiply all array elements by the constant b 
  NDataBlock<T>& MulCst(T b); 
  //! In place constant multiplication operator: multiply all array elements by b (ND *= b)
  inline NDataBlock<T>& operator *= (T b)  { return MulCst(b); }
  //! Divide all array elements by the constant b (or if fginv==true ND = b / ND) 
  NDataBlock<T>& DivCst(T b,bool fginv=false); 
  //! In place constant division operator: divide all array elements by b (ND /= b)
  NDataBlock<T>& operator /= (T b)  { return DivCst(b,false); }

  //---- In place arithmetic operations with another NDataBlocks (element wise)
  //! In place element wise addition: ND.elem[k] += a.elem[k] 
  NDataBlock<T>& AddElt(const NDataBlock<T>& a); 
  //! In place element wise addition operator (ND += a):  ND.elem[k] += a.elem[k] 
  inline NDataBlock<T>& operator += (const NDataBlock<T>& a)  { return AddElt(a); } 
  //! In place element wise subtraction: ND.elem[k] -= a.elem[k]  (or if fginv==true  ND = a-ND) 
  NDataBlock<T>& SubElt(const NDataBlock<T>& a, bool fginv=false); 
  //! In place element wise subtraction operator (ND -= a): ND.elem[k] -= a.elem[k] 
  inline NDataBlock<T>& operator -= (const NDataBlock<T>& a) { return SubElt(a,false); }
  //! In place element wise product (multiplication): ND.elem[k] *= a.elem[k] 
  NDataBlock<T>& MulElt(const NDataBlock<T>& a); 
  //! In place element wise product operator (ND *= a): ND.elem[k] *= a.elem[k] 
  inline NDataBlock<T>& operator *= (const NDataBlock<T>& a)  { return MulElt(a); } 
  //! In place element wise division: ND.elem[k] /= a.elem[k]  (or if fginv==true  ND = a/ND) 
  NDataBlock<T>& DivElt(const NDataBlock<T>& a, bool fginv=false); 
  //! In place element wise divide operator (ND /= a): ND.elem[k] /= a.elem[k] 
  inline NDataBlock<T>& operator /= (const NDataBlock<T>& a)  { return DivElt(a,false); }

  //---- cont version of the arithmetic operation methods - The source array (NDataBlock object) is not modified.
  // Surcharge d'operateurs: C = A @ x , C = A @ B
  NDataBlock<T> Add(T b) const;
  NDataBlock<T> Sub(T b,bool fginv=false) const;
  NDataBlock<T> Mul(T b) const;
  NDataBlock<T> Div(T b,bool fginv=false) const;

  NDataBlock<T> Add(const NDataBlock<T>& b) const;
  NDataBlock<T> Sub(const NDataBlock<T>& b) const;
  NDataBlock<T> Mul(const NDataBlock<T>& b) const;
  NDataBlock<T> Div(const NDataBlock<T>& b) const;

  //! Return thye associated object Id (or DataRef Id)
  inline uint_8 DRefId() { return ((mSRef)?mSRef->dsid:0); }
  //! assign a new object Id (or DataRef Id) - useful for PPF write operations 
  inline void RenewDRefId() { if (mSRef) mSRef->dsid = AnyDataObj::getUniqueId(); } 
  //! assign a new object Id (or DataRef Id) - useful for PPF write operations 
  inline void RenewObjId() { if (mSRef) mSRef->dsid = AnyDataObj::getUniqueId(); }
  

protected:
  //! NDREF structure for reference management
  typedef struct {
    size_t nref;      //!< Number of references to the data structure
    uint_8 dsid;      //!< Data structure Id - Used by FIO_NDataBlock
    T* data;          //!< Pointer to data structure itself
    Bridge* bridge;   //!< Pointer to a bridge for the data structure
    Units* unit_p;    //!<  Optional physical unit of the data 
  } NDREF;

  void Alloc(size_t n,T* data=NULL,Bridge* br=NULL, bool zero=true);
  void Delete(void);

  static int Debug_NDataBlock; //!< DEBUG: set debug level (all type<T> classes)
  static size_t NallocData; //!< DEBUG: number of allocations (all type<T> classes)
  static size_t NallocSRef; //!< DEBUG: number of references (all type<T> classes)
  static ThSafeOp* gThsop;  //!< Mutex For thread safe operation 

  size_t       mSz;      //!< size of data structure
  NDREF*       mSRef;    //!< NDREF structure for reference management
  mutable bool mIsTemp;  //!< true if class is temporary
};

//! Define operator \<\< for printing
template<class T>
inline std::ostream& operator << (std::ostream& os, const NDataBlock<T>& a)
                      {a.Print(os); return(os);}

//! Add a constant to datas and return NDataBlock : ND = NDa + b
template<class T>
inline NDataBlock<T> operator + (const NDataBlock<T>& a,T b)
                      {return a.Add(b);}
//! Add a constant to datas and return NDataBlock : ND = b + NDa
template<class T>
inline NDataBlock<T> operator + (T b,const NDataBlock<T>& a)
                      {return a.Add(b);}
//! Substract a constant to datas and return NDataBlock : ND = NDa - b
template<class T>
inline NDataBlock<T> operator - (const NDataBlock<T>& a,T b)
                      {return a.Sub(b);}
//! Substract a constant to datas and return NDataBlock : ND = b - NDa
template<class T>
inline NDataBlock<T> operator - (T b,const NDataBlock<T>& a)
                      {return a.Sub(b,true);}
//! Multiply datas by a constant and return NDataBlock : ND = NDa * b
template<class T>
inline NDataBlock<T> operator * (const NDataBlock<T>& a,T b)
                      {return a.Mul(b);}
//! Multiply datas by a constant and return NDataBlock : ND = b * NDa
template<class T>
inline NDataBlock<T> operator * (T b,const NDataBlock<T>& a)
                      {return a.Mul(b);}
//! Divide datas by a constant and return NDataBlock : ND = NDa / b
template<class T>
inline NDataBlock<T> operator / (const NDataBlock<T>& a,T b)
                      {return a.Div(b);}
//! Divide a constant by datas and return NDataBlock : ND = b / NDa
template<class T>
inline NDataBlock<T> operator / (T b,const NDataBlock<T>& a)
                      {return a.Div(b,true);}

//! Add datas of two data blocks and return NDataBlock : ND = NDa + NDb
template<class T>
inline NDataBlock<T> operator + (const NDataBlock<T>& a,const NDataBlock<T>& b)
                      {return a.Add(b);}
//! Substract datas of two data blocks and return NDataBlock : ND = NDa - NDb
template<class T>
inline NDataBlock<T> operator - (const NDataBlock<T>& a,const NDataBlock<T>& b)
                      {return a.Sub(b);}
//! Multiply datas of two data blocks and return NDataBlock : ND = NDa * NDb
template<class T>
inline NDataBlock<T> operator * (const NDataBlock<T>& a,const NDataBlock<T>& b)
                      {return a.Mul(b);}
//! Divide datas of two data blocks and return NDataBlock : ND = NDa / NDb
template<class T>
inline NDataBlock<T> operator / (const NDataBlock<T>& a,const NDataBlock<T>& b)
                      {return a.Div(b);}

//--------- extern template declarations (if needed) -----------
#if defined ( NEED_EXT_DECL_TEMP ) && !defined( NDATABLOCK_CC_BFILE )
extern template class NDataBlock<uint_1>;
extern template class NDataBlock<uint_2>;
extern template class NDataBlock<int_1>;
extern template class NDataBlock<int_2>;
extern template class NDataBlock<int_4>;
extern template class NDataBlock<int_8>;
extern template class NDataBlock<uint_4>;
extern template class NDataBlock<uint_8>;
extern template class NDataBlock<r_4>;
extern template class NDataBlock<r_8>;
extern template class NDataBlock< std::complex<r_4> >;
extern template class NDataBlock< std::complex<r_8> >;
#ifdef SO_LDBLE128
extern template class NDataBlock<r_16>;
extern template class NDataBlock< std::complex<r_16> >;
#endif

#endif  // Fin de if defined ( NEED_EXT_DECL_TEMP )

} // Fin du namespace

#endif
