/* ------------------------------------------------------------------------------------
   SOPHYA class library - (C)  UPS+LAL IN2P3/CNRS  , (C) CEA-Irfu-SPP 
   PPF handler for SphericalMap with Theta-Phi (IGLOO) pixelisation 
   R. Ansari 2000-2015
   ------------------------------------------------------------------------------------  */

#include "fiospherethetaphi.h"
#include "pexceptions.h"
#include "fiondblock.h"
#include "datatype.h"
#include <typeinfo>

///////////////////////////////////////////////////////////
// --------------------------------------------------------
//   Les objets delegues pour la gestion de persistance de SphereThetaPhi<T>
// --------------------------------------------------------
//////////////////////////////////////////////////////////

using namespace std;

namespace SOPHYA {

/*!
  \class FIO_SphereThetaPh
  \ingroup SkyMap

  \brief PPF handler for SphereThetaPhi<T> Spherical maps 

  \sa SOPHYA::SphereThetaPhi
*/

template <class T>
FIO_SphereThetaPhi<T>::FIO_SphereThetaPhi()
{
  dobj= NULL;
  ownobj= true;
}

template <class T>
FIO_SphereThetaPhi<T>::FIO_SphereThetaPhi(const SphereThetaPhi<T>& obj)
{
  dobj= new SphereThetaPhi<T>(obj, true);
  ownobj= true;
}

template <class T>
FIO_SphereThetaPhi<T>::FIO_SphereThetaPhi(SphereThetaPhi<T>* obj)
{
  dobj= obj;
  ownobj= false;
}

template <class T>
FIO_SphereThetaPhi<T>::~FIO_SphereThetaPhi()
{
  if (ownobj && dobj) delete dobj;
}

template <class T>
AnyDataObj* FIO_SphereThetaPhi<T>::DataObj()
{
  return(dobj);
}


template <class T>
void FIO_SphereThetaPhi<T>::SetDataObj(AnyDataObj & o)
{
  SphereThetaPhi<T> * po = dynamic_cast< SphereThetaPhi<T> * >(&o);
  if (po == NULL) {
    char buff[160];
    sprintf(buff,"FIO_SphereThetaPhi<%s>::SetDataObj(%s) - Object type  error ! ",
	    DataTypeInfo<T>::getTypeName().c_str(), typeid(o).name());
    throw TypeMismatchExc(PExcLongMessage(buff));    
  }

  if (ownobj && dobj) delete dobj;
  dobj = po; ownobj = false;
} 

template <class T>
void FIO_SphereThetaPhi<T>::ReadSelf(PInPersist& is)
{
//  On lit 3 uint_8 
//  0 : Numero de version,  1: flags,  2: nTheta (number of rings/slices in theta)
//  itab[1]  bit 0 true -> FgBy3_ , bit 1 true -> Has Info
// On lit les 3 premiers uint_8
  uint_8 itab[3];
  is.Get(itab, 3);
  if (itab[0]<2)  
    throw FileFormatExc("FIO_SphereThetaPhi<T>::ReadSelf() - Old V=1 PPF object version reading NOT supported");
  bool fgby3=false;
  bool hasinfo=false;
  if ((itab[1]&1)==1)  fgby3=true;
  if ((itab[1]&2)==2)  hasinfo=true;
  if (dobj == NULL) { dobj = new SphereThetaPhi<T>(itab[2], fgby3);  ownobj = true; }
  else dobj->setNbRings(itab[2], fgby3);
  // Let's Read the SphereCoordSys object  
  SphereCoordSys scs;
  FIO_SphereCoordSys fio_scs(&scs);
  fio_scs.Read(is);
  dobj->SetCoordSys(scs);
  FIO_NDataBlock<T> fio_nd(&(dobj->DataBlock()));
  fio_nd.Read(is);
  if (hasinfo) {
    DVList& dvlinfo=dobj->Info();
    is >> dvlinfo;
  }
}

template <class T>
void FIO_SphereThetaPhi<T>::WriteSelf(POutPersist& os) const
{
  if(dobj == NULL) 
    throw NullPtrError("FIO_SphereThetaPhi<T>::WriteSelf() null dobj pointer");
//  On ecrit 3 uint_8 
//  0 : Numero de version,  1: flags,  2  reserve
//  itab[0] = 2    (NumVer=2 a partir de Juin 2015)
//  itab[1] (flags)  bit 0 true (2^0=1) -> FgBy3_ , bit 1 true (2^1=2) -> Has Info
//  itab[2] = NbThetaSlices = NbRings 
  uint_8 itab[3];
  itab[0] = 2;  // On passe a 2 en Juin 2015 - les PPF contenant une SphereThetaPhi avec V=1 ne sont plus lisibles 
  itab[1] = 0;
  if (dobj->IsRingNPixMultipleOf3() )  itab[1]|=1;
  if (dobj->HasInfoObject())  itab[1]|=2;
  itab[2] = dobj->getNbThetaSlices();
  os.Put(itab, 3);
  // Let's write the SphereCoordSys object
  SphereCoordSys scs(dobj->GetCoordSys());
  FIO_SphereCoordSys fio_scs(&scs);
  fio_scs.Write(os);
  // write the map pixel data 
  FIO_NDataBlock<T> fio_nd(&(dobj->DataBlock()));
  fio_nd.Write(os);
  // and the DVList info object 
  if (dobj->HasInfoObject()) {
    DVList& dvlinfo=dobj->Info();
    os << dvlinfo;
  }
}

#ifdef __CXX_PRAGMA_TEMPLATES__
#pragma define_template FIO_SphereThetaPhi<int_4>
#pragma define_template FIO_SphereThetaPhi<r_8>
#pragma define_template FIO_SphereThetaPhi<r_4>
#pragma define_template FIO_SphereThetaPhi< complex<r_4> >
#pragma define_template FIO_SphereThetaPhi< complex<r_8> >
#endif
#if defined(ANSI_TEMPLATES) || defined(GNU_TEMPLATES)
template class FIO_SphereThetaPhi<uint_1>;
template class FIO_SphereThetaPhi<uint_2>;
template class FIO_SphereThetaPhi<int_1>;
template class FIO_SphereThetaPhi<int_2>;
template class FIO_SphereThetaPhi<int_4>;
template class FIO_SphereThetaPhi<int_8>;
template class FIO_SphereThetaPhi<r_4>;
template class FIO_SphereThetaPhi<r_8>;
template class FIO_SphereThetaPhi< complex<r_4> >;
template class FIO_SphereThetaPhi< complex<r_8> >;
#ifdef SO_LDBLE128
template class FIO_SphereThetaPhi<r_16>;
template class FIO_SphereThetaPhi< complex<r_16> >;
#endif
#endif

} // FIN namespace SOPHYA 
