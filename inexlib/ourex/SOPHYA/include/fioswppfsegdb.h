// This may look like C code, but it is really -*- C++ -*-
// Classe pour la gestion de persistance PPF des SwPPFSegDataBlock 
//                         R. Ansari - Fevrier 2013
// (C) LAL (Orsay) / IN2P3-CNRS  IRFU/SPP (Saclay) / CEA

#ifndef FIOSWPPFSEGDATABLOCK_H
#define FIOSWPPFSEGDATABLOCK_H

#include "machdefs.h"
#include "ppersist.h"
#include "swppfsegdb.h"
#include <typeinfo> 
#include <iostream> 

/*!
   \class SOPHYA::FIO_SwPPFSegDataBlock
   \ingroup BaseTools
   Class implementing PPF persistence (PPersist handler) for segmented data structures
   using PPF streams as swap space (class SwPPFSegDataBlock).
   \sa PPersist
*/

namespace SOPHYA {

   
template <class T>
class FIO_SwPPFSegDataBlock : public  PPersist  {
public:
  FIO_SwPPFSegDataBlock()
  {
    dobj = NULL ;
    ownobj=true;    
  }

  //   FIO_SwPPFSegDataBlock(string const & filename);
  FIO_SwPPFSegDataBlock(SwPPFSegDataBlock<T> & obj)
  {
    dobj = &obj;
    ownobj = false;
  }

  FIO_SwPPFSegDataBlock(SwPPFSegDataBlock<T> * obj)
  {
    if (obj == NULL)  
      throw ParmError("FIO_SwPPFSegDataBlock<T>::FIO_SwPPFSegDataBlock(* obj) obj=NULL (ppfwrapstlv.h)");
    dobj = obj;
    ownobj = false;
  }
  virtual ~FIO_SwPPFSegDataBlock()
  {
    if (ownobj && dobj)    delete dobj;
  }
  virtual AnyDataObj* DataObj() { return dobj; }
  virtual void        SetDataObj(AnyDataObj & o)
  {
    SwPPFSegDataBlock<T> * po = dynamic_cast< SwPPFSegDataBlock<T> * >(&o);
    if (po == NULL) {
      char buff[160];
      sprintf(buff,"FIO_SwPPFSegDataBlock<%s>::SetDataObj(%s) - Object type  error ! ",
	      typeid(T).name(), typeid(o).name());
      throw TypeMismatchExc(PExcLongMessage(buff));    
    }
    if (ownobj && dobj) delete dobj;
    dobj = po; ownobj = false;
  } 

  inline operator SwPPFSegDataBlock<T>()  { return(*dobj); }

  // Les methodes suivantes implementent les fonctionalites necessaires
  // au  partage de reference ds les fichiers PPF 
  uint_8 getMemOId() const
  {
    return ( (dobj) ? dobj->DRefId() : 0 );
  }
  void ShareDataReference(PPersist & pp)
  {
    FIO_SwPPFSegDataBlock<T> *ppo = dynamic_cast< FIO_SwPPFSegDataBlock<T> * >(&pp);
    if (ppo == NULL) throw TypeMismatchExc("FIO_SwPPFSegDataBlock<T>::ShareDataReference() - Type Mismatch Error");
    if (ppo->dobj) {
      if (dobj) (*dobj)=*(ppo->dobj); //  dobj->Share(*(ppo->dobj));
      else { dobj = new SwPPFSegDataBlock<T>(*(ppo->dobj)); ownobj = true; } 
    }
  }
  PPersist* CloneSharedReference()
  {
    FIO_SwPPFSegDataBlock<T> * ppo = new FIO_SwPPFSegDataBlock<T>;
    ppo->dobj = new SwPPFSegDataBlock<T>(*(dobj));
    ppo->ownobj = true;
    return(ppo); 
  }

protected :
  virtual void       ReadSelf(PInPersist& is)
  {
    // On lit les 4 premiers uint_8
    // 0: Numero de version (16 bits, 0..15) + bits d'etat (48 bits 16..63)  
    // 1: SegmentSize  2: NbSegments  3: NbItems
    uint_8 itab[4];
    is.Get(itab, 4);
    if (dobj == NULL) {
      dobj = new SwPPFSegDataBlock<T>(); 
      ownobj = true;
    }
    dobj->SetSize(itab[1], itab[2], itab[3]);
    //DBG cout << " ReadSelf()*DBG dobj->SetSize(" << itab[1] << "," << itab[2] << "," << itab[3] << ")" << endl;
    // lecture de l'unite si applicable 
    Units un;
    bool sdone=false;
    if (((itab[0]&0xFFFF)>=1)&&(itab[0]&(1<<16)))  { is >> un;  sdone=true; }
    if (sdone) dobj->SetUnits(un);
    // nom du fichier de swap et flag indiquant si fichier de swap = ce fichier 
    bool sameswf=false;
    std::string swpfilename;
    if (((itab[0]&0xFFFF)>=1)&&(itab[0]&(1<<17)))  sameswf=true;
    // On lit le nom du fichier de swap 
    is.Get(swpfilename);
    // On defint le swapper s'il n'y en a pas deja 
    DataSwapperInterface<T> * dsw=dobj->GetSwapper();
    if (!dsw)  {
      if (sameswf) {
	PPFDataSwapper<T> swp(is);
	dobj->SetSwapper( swp );
      }
      else {
	PPFDataSwapper<T> swp(swpfilename, std::ios::in );
	dobj->SetSwapper( swp );
      }
    }
    // On lit la table des tags de positionnement ...
    std::vector<int_8> postags;
    is.GetPosTagTable(postags);
    /*DBG
    cout << " ReadSelf()*DBG*B postags.size()=" << postags.size();
    if (postags.size()>3) 
      cout << " postags[0..2]: : " << postags[0] << "," << postags[1] << "," << postags[2] << endl;
    else cout << endl;
    */
    dobj->SetSwapPosTagTable(postags);
  }
         
  virtual void       WriteSelf(POutPersist& os) const
  {
    if (dobj == NULL) 
      throw ParmError("FIO_SwPPFSegDataBlock<T>::WriteSelf() dobj=NULL (fioswppfsegdb.h)");
    // On ecrit les 4  uint_8
    // 0: Numero de version (16 bits 0..15) + bits d'etat (48 bits 16..63)  
    // 1: SegmentSize  2: NbSegments  3: NbItems
    //  Numero de version 1 - introduction de la classe en Fev 2013 
    //      Bit 16 de itab[0] a 1 si  mSRef->unit_p non nul 
    //      Bit 17 de itab[0] a 1 si  si fichier PPF swapper different de celui ds lequel on ecrit  
    uint_8 itab[4];
    itab[0] = 1;
    itab[1] = dobj->SegmentSize();
    itab[2] = dobj->NbSegments();
    itab[3] = dobj->NbItems();
    bool sdone;
    Units un=dobj->GetUnits(sdone);
    if (sdone)  itab[0] |= (1 << 16);
    bool sameswf=false;
    std::string swpfilename;
    DataSwapperInterface<T> * dsw=dobj->GetSwapper();
    PPFDataSwapper<T> * dswp=NULL;
    if (dsw) dswp = dynamic_cast< PPFDataSwapper<T> * >(dsw);
    if (dswp)  {
      if (dswp->OutStream() == &os) sameswf=true;
      else sameswf=false;
      if (dswp->OutStream()) swpfilename=dswp->OutStream()->FileName();
    }
    if (sameswf)  itab[0] |= (1 << 17);
    os.Put(itab, 4);
    if (sdone)  os<<un;   // ecriture de l'unite si definie
    // On ecrit le nom du fichier de swap 
    os.Put(swpfilename);
    // On ecrit la table des tags de positionnement 
    os.PutPosTagTable(dobj->GetSwapPosTagTable());
  }

  // Variables membres 
  SwPPFSegDataBlock<T> * dobj;  // Le vecteur de la STL 
  bool ownobj;  // true -> objet cree par le wrapper 
};

/*! Writes the SwPPFSegDataBlock<T> \b obj in the POutPersist stream \b os */
template <class T>
inline POutPersist& operator << (POutPersist& os, SwPPFSegDataBlock<T> & obj)
{ obj.SwapOutBuffer(); FIO_SwPPFSegDataBlock<T> fio(&obj);  fio.Write(os);  return(os); }
/*! Initializes the SwPPFSegDataBlock<T> \b obj from the PInPersist stream \b is */
template <class T>
inline PInPersist& operator >> (PInPersist& is, SwPPFSegDataBlock<T> & obj)
{ FIO_SwPPFSegDataBlock<T> fio(&obj); is.SkipToNextObject(); fio.Read(is); return(is); }

} // Fin du namespace

#endif
