// This may look like C code, but it is really -*- C++ -*-
// Classe pour la permettre la persistance PPF des vecteurs STL
//                         R. Ansari - Avril 2005
// LAL (Orsay) / IN2P3-CNRS  DAPNIA/SPP (Saclay) / CEA

#ifndef PPFWRAPSTLV_H
#define PPFWRAPSTLV_H

#include "machdefs.h"
#include "anydataobj.h"
#include "ppersist.h"
#include "pexceptions.h"
#include <vector>
#include <iostream>

/*!
   \class SOPHYA::PPFWrapperSTLVector
   \ingroup BaseTools
   Class for managing PPF persistence of standard (STL) vectors.
   Only the overloaded operators I/O operators on POutPersist
   and PInPersist streams are intended to be used in user programs.
   When applied to vectors of user defined types and classes (\b T), the 
   c++ I/O operators POutPersist& << and PInPersist& >> should be 
   defined for the type \b T and the resulting handler must be registered
   using the macro \b PPRegister(PPFWrapperSTLVector<T>). However, automatic 
   registration of PPFWrapperSTLVector<T> have been implemented in february 2012,
   durng the first call to Read() or Write().
   
*/

namespace SOPHYA {

   
template <class T>
class PPFWrapperSTLVector : public  PPersist , public AnyDataObj {
public:
  PPFWrapperSTLVector()
  {
    dobj = new std::vector<T> ;
    ownobj=true;    
  }

  //   PPFWrapperSTLVector(string const & filename);
  PPFWrapperSTLVector(std::vector<T> & obj)
  {
    dobj = &obj;
    ownobj = false;
  }

  PPFWrapperSTLVector(std::vector<T> * obj)
  {
    if (obj == NULL)  
      throw ParmError("PPFWrapperSTLVector<T>::PPFWrapperSTLVector(* obj) obj=NULL (ppfwrapstlv.h)");
    dobj = obj;
    ownobj = false;
  }
  virtual ~PPFWrapperSTLVector()
  {
    if (ownobj && dobj) delete dobj;
  }
  virtual AnyDataObj* DataObj() { return this; }
  virtual void        SetDataObj(AnyDataObj & o)
  {
    throw NotAvailableOperation("PPFWrapperSTLVector<T>::SetDataObj() Not permitted ! (ppfwrapstlv.h)");
  }
  inline operator std::vector<T>()  { return(*dobj); }

  virtual void       Write(POutPersist& os) const
  {
    if (!PIOPersist::checkPPClassId(*this)) {  // on enregistre la classe 
      PPRegister(PPFWrapperSTLVector< T >);
      DObjRegister(PPFWrapperSTLVector< T >, std::vector< T >);
    }
    PPersist::Write(os);
  }
  virtual void       Read(PInPersist& os) 
  {
    if (!PIOPersist::checkPPClassId(*this)) {  // on enregistre la classe 
      PPRegister(PPFWrapperSTLVector< T >);
      DObjRegister(PPFWrapperSTLVector< T >, std::vector< T >);
    }
    PPersist::Read(os);
  }

protected :
  virtual void       ReadSelf(PInPersist& is)
  {
    if (dobj == NULL) 
      throw ParmError("PPFWrapperSTLVector<T>::ReadSelf() dobj=NULL (ppfwrapstlv.h)");
    // On lit les 3 premiers uint_8
    uint_8 itab[3];
    is.Get(itab, 3);
    // On efface le contenu du vecteur si necessaire - (on peut faire plus efficacement ...)
    if ( dobj->size() > 0) dobj->erase(dobj->begin(), dobj->end());
    for(uint_8 k=0; k<itab[1]; k++) { 
      T el;
      is >> el;
      dobj->push_back(el);
    }
  }         
  virtual void       WriteSelf(POutPersist& os) const
  {
    if (dobj == NULL) 
      throw ParmError("PPFWrapperSTLVector<T>::WriteSelf() dobj=NULL (ppfwrapstlv.h)");
    //  On ecrit 3 uint_8 
    //  0 : Numero de version = 1 : Taille,  2  reserve a l
    uint_8 itab[3];
    itab[0] = 1;
    itab[1] = dobj->size();
    itab[2] = 0;
    os.Put(itab, 3);
    // On ecrit le vecteur de donnees 
    for(uint_8 k=0; k<itab[1]; k++) os << (*dobj)[k];
  }

  // Variables membres 
  std::vector<T> * dobj;  // Le vecteur de la STL 
  bool ownobj;  // true -> objet cree par le wrapper 
};

/*! Writes the STL vector object in the POutPersist stream \b os */
template <class T>
inline POutPersist& operator << (POutPersist& os, std::vector<T> & obj)
{ PPFWrapperSTLVector<T> fio(&obj);  fio.Write(os);  return(os); }
/*! Reads in and initializes the STL vector object from the PInPersist stream \b is */
template <class T>
inline PInPersist& operator >> (PInPersist& is, std::vector<T> & obj)
{ PPFWrapperSTLVector<T> fio(&obj); is.SkipToNextObject(); fio.Read(is); return(is); }

} // Fin du namespace

#endif
