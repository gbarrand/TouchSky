/* ------------------------------------------------------------------------------------
   SOPHYA class library - (C)  UPS+LAL IN2P3/CNRS  , (C) CEA-Irfu-SPP 
   PPF handler for SphericalMap with HEALPix pixelisation 
   Original version : G. Le Meur 04/2000
   R. Ansari 2000-2015
   ------------------------------------------------------------------------------------  */
// G. Le Meur 04/2000

#include "fiospherehealpix.h"
#include "pexceptions.h"
#include "fiondblock.h"
#include "datatype.h"
#include <typeinfo>

//*******************************************************************
// Class FIO_SphereHEALPix<T>
//  Les objets delegues pour la gestion de persistance SphereHEALPix<T>
//*******************************************************************

using namespace std;

namespace SOPHYA {

/*!
  \class FIO_SphereHEALPix
  \ingroup SkyMap

  \brief PPF handler for SphereHEALPix<T> Spherical maps 

  \sa SOPHYA::SphereHEALPix
*/

template <class T>
FIO_SphereHEALPix<T>::FIO_SphereHEALPix()
{
  dobj_= NULL;
  ownobj= true;
}

template <class T>
FIO_SphereHEALPix<T>::FIO_SphereHEALPix(const SphereHEALPix<T>& obj)
{
  dobj_= new SphereHEALPix<T>(obj, true);
  ownobj= true;
}

template <class T>
FIO_SphereHEALPix<T>::FIO_SphereHEALPix(SphereHEALPix<T>* obj)
{
  dobj_= obj;
  ownobj= false;
}

template <class T>
FIO_SphereHEALPix<T>::~FIO_SphereHEALPix()
{
  if (ownobj && dobj_) delete dobj_;
}

template <class T>
AnyDataObj* FIO_SphereHEALPix<T>::DataObj()
{
  return(dobj_);
}

template <class T>
void FIO_SphereHEALPix<T>::SetDataObj(AnyDataObj & o)
{
  SphereHEALPix<T> * po = dynamic_cast< SphereHEALPix<T> * >(&o);
  if (po == NULL) { 
    char buff[160];
    sprintf(buff,"FIO_SphereHEALPix<%s>::SetDataObj(%s) - Object type error ! ",
	    DataTypeInfo<T>::getTypeName().c_str(), typeid(o).name());
    throw TypeMismatchExc(PExcLongMessage(buff));
  }
  if (ownobj && dobj_) delete dobj_;
  dobj_ = po; ownobj = false;
} 

template <class T>
void FIO_SphereHEALPix<T>::ReadSelf(PInPersist& is)
{
  bool fgring = false;  
  bool hadinfo= false;
  SphereCoordSys scs;
// On lit les 3 premiers uint_8
  uint_8 itab[3];
  is.Get(itab, 3);
  if (itab[0] >= 3)  {  // lecture version PPF-healpix V>=3 
    if ((itab[2]&1)==1)  fgring=true;
    if ((itab[2]&2)==2)  {
      hadinfo=true;
    }
    if (dobj_ == NULL) { dobj_ = new SphereHEALPix<T>(itab[1], fgring);  ownobj = true; }
    else dobj_->setNSide(itab[1], fgring);
    // Let's Read the SphereCoordSys object  
    SphereCoordSys scs;
    FIO_SphereCoordSys fio_scs(&scs);
    fio_scs.Read(is);
    dobj_->SetCoordSys(scs);
    FIO_NDataBlock<T> fio_nd(&(dobj_->DataBlock()));
    fio_nd.Read(is);
    if (hadinfo) {
      DVList& dvlinfo=dobj_->Info();
      is >> dvlinfo;
    }
  }
  else {    // Code de lecture pour versions anciennes V <= 2 
    //  Si itab[2]==1 --> NESTED 
    fgring=true;
    if (itab[2] == 1) fgring = false;
    if (dobj_ == NULL) { dobj_ = new SphereHEALPix<T>(itab[1], fgring);  ownobj = true; }
    else dobj_->setNSide(itab[1], fgring);

    // Let's Read the SphereCoordSys object  -- ATTENTIOn - $CHECK$
    SphereCoordSys scs;
    FIO_SphereCoordSys fio_scs(scs);
    fio_scs.Read(is);
    // Pour savoir s'il y avait un DVList Infodobj->Info();  associe
    char strg[256];
    is.GetLine(strg, 255);
    if(strncmp(strg+strlen(strg)-7, "HasInfo", 7) == 0)  hadinfo= true;
    if(hadinfo)   is >> dobj_->Info();  // Lecture eventuelle du DVList Info
    int_4 nSide, nPix;
    double Omega;
    is.GetI4(nSide);    is.GetI4(nPix);   is.GetR8(Omega);
    // On lit les DataBlocks;
    FIO_NDataBlock<T> fio_nd(&dobj_->DataBlock());
    fio_nd.Read(is);
    NDataBlock<int_4> db1, db2;
    FIO_NDataBlock<int_4> fio_ndi1(&db1);
    fio_ndi1.Read(is);
    FIO_NDataBlock<int_4> fio_ndi2(&db2);
    fio_ndi2.Read(is);
  }    //---- fin lecture PPF healpex V<=2 
}
 
template <class T>
void FIO_SphereHEALPix<T>::WriteSelf(POutPersist& os) const
{
  if(dobj_ == NULL) 
    throw NullPtrError("FIO_SphereHEALPix<T>::WriteSelf() null dobj pointer");

//  On ecrit 3 uint_8 
//  0 : Numero de version,  1: SizeIndex=healpix_nside,  2: flags 
//  Juin 2006 : NumVer 1->2 avec mise en place de HEALPix NESTED
//  totalement back-compatible , on utilise itab[2] qu'on met a 1
//  pour NESTED
//  Juin 2015 : NumVer 3   itab[0]=3 
//     - on n'ecrit plus que le tableau des pixels
//     - on indique par un bit (bit 1->2^1) la presence d'un DVList
//     - compatibilite backward preserve 
//  itab[1] = HEALPix_NSide   (=SizeIndex())  
//  itab[2]  bit 0 true (2^0=1) -> RING (NESTED otherwise) , bit 1 true (2^1=2) -> Has Info
  uint_8 itab[3];
  itab[0] = 3;   // Passage a NumVer=3 en juin 2015 
  itab[1] = dobj_->getNSide() ;
  itab[2] = 0; 
  if (dobj_->IfRING()) itab[2]|=1;
  if (dobj_->HasInfoObject())  itab[2]|=2;
  os.Put(itab, 3);
// Let's write the SphereCoordSys object
  SphereCoordSys scs(dobj_->GetCoordSys());
  FIO_SphereCoordSys fio_scs(&scs);
  fio_scs.Write(os);
  // write the map pixel data 
  FIO_NDataBlock<T> fio_nd(&(dobj_->DataBlock()));
  fio_nd.Write(os);
  // and the DVList info object 
  if (dobj_->HasInfoObject()) {
    DVList& dvlinfo=dobj_->Info();
    os << dvlinfo;
  }
}

//*******************************************************************

#ifdef __CXX_PRAGMA_TEMPLATES__
#pragma define_template FIO_SphereHEALPix<uint_2>
#pragma define_template FIO_SphereHEALPix<int_4>
#pragma define_template FIO_SphereHEALPix<r_8>
#pragma define_template FIO_SphereHEALPix<r_4>
#pragma define_template FIO_SphereHEALPix< complex<r_4> >
#pragma define_template FIO_SphereHEALPix< complex<r_8> >
#endif
#if defined(ANSI_TEMPLATES) || defined(GNU_TEMPLATES)
template class FIO_SphereHEALPix<uint_1>;
template class FIO_SphereHEALPix<uint_2>;
template class FIO_SphereHEALPix<int_1>;
template class FIO_SphereHEALPix<int_2>;
template class FIO_SphereHEALPix<int_4>;
template class FIO_SphereHEALPix<int_8>;
template class FIO_SphereHEALPix<r_4>;
template class FIO_SphereHEALPix<r_8>;
template class FIO_SphereHEALPix< complex<r_4> >;
template class FIO_SphereHEALPix< complex<r_8> >;
#ifdef SO_LDBLE128
template class FIO_SphereHEALPix<r_16>;
template class FIO_SphereHEALPix< complex<r_16> >;
#endif

#endif

} // FIN namespace SOPHYA 
