#include "sopnamsp.h"
#include "machdefs.h"

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <complex>

#include "sophyainit.h"

#include "pexceptions.h"

#include "ppersist.h"
#include "fiondblock.h"
#include "dvlist.h"

#include "randr48.h"
#include "randfmt.h"
#include "randtmt32.h"
#include "randtmt64.h"

#include "fiosegdb.h" 
#include "fioswppfsegdb.h" 
#include "ppfwrapstlv.h" 

#include "sunitpcst.h"

#include "sversion.h"

#include <iostream>
#include <map> 

// ---  Classe d'initialisation de SOPHYA, (PPersistMgr en particulier)
int SophyaInitiator::FgInit = 0;
// Module version number - 2.0 , Jul 2006
// Module version number - 2.02 , Fev07 Ajout NDataBlock::RenewObjId()
// Module version number - 2.1 ,  Avr07 
//  - Nettoyage machdefs_mkmf.h 
//  - Ajout classe ThSafeOp ---> NDataBlock<T> Sw/SegDataBlock<T> ThreadSafe
// Module version number - 2.15 ,  Oct08
//  - Exceptions sophya heritent de std::exception  (mars 09)
//  - Extension / ajout de classes generateur aleatoires (Mersenne-Twister ...avril 09)      
// Module version number - 2.20 ,  Avr09 
//  - Ajout type long double (r_16) et prise en chage des r_16 et complex<r_16> 
//    par les PPersist (fev 2010) 
// Module version number - 2.30 ,  Fev10 
//  - Fev 2012: Ajout classes PrimeNumbers, QNumber, Units, PhysQty 
// Module version number - 2.40 ,  Avr12  / Nov 12 
//  - Nov 2012:  Attribut Units dans NDataBlock (et SegDataBlock ...)
//     amelioratios diverses ds SegDataBlock et SwSegDataBlock        
//  - Oct 2014:  Gestion bool ds MuTyV 
//  - Mar 2015:  Ajout exception ZeroValExc
//  - Ete 2015: petites amelioration de NDataBlock
//  - Hiver 2018-Jan2019: petites amelioration de SegDataBlock et de Units
 
// Module version number - 2.55  (Jan 2019)
     
#define MOD_VERS   2.55

// Pour garder la liste des modules et leurs numeros de version
map<string, double>* ModListP = NULL;

/*!
   \namespace SOPHYA
   \brief This is the namespace for the whole Sophya class library 

   Check the SOPHYA web site http://www.sophya.org for more information.
*/

/*!
  \class SOPHYA::SophyaInitiator
  \ingroup BaseTools
  Each Sophya module may contain an initialiser class which should
  be a sub-class of SophyaInitiator.  
*/


SophyaInitiator::SophyaInitiator()
{
#if defined(Darwin)
  // Il semble y avoir un probleme sous MacOSX ...
  if (FgInit == 0) 
    cout << " SophyaInitiator::SophyaInitiator() BaseTools Init" << endl;
#endif
  FgInit++;
  if (FgInit > 1)  return;

  ModListP = new map<string, double>;

  // --- Les generateurs aleatoires ...
  DR48RandGen* grgp = new  ThSDR48RandGen;
  RandomGeneratorInterface::SetGlobalRandGenP(grgp); 

  // Initialisation des mecanismes PPF I/O 
  PIOPersist::Initialize();
  // Enregistrement des handlers PPF pour les NDataBlock<T> 
  PPRegister(FIO_NDataBlock<uint_1>);
  DObjRegister(FIO_NDataBlock<uint_1>, NDataBlock<uint_1>);
  PPRegister(FIO_NDataBlock<int_1>);
  DObjRegister(FIO_NDataBlock<int_1>, NDataBlock<int_1>);
  PPRegister(FIO_NDataBlock<uint_2>);
  DObjRegister(FIO_NDataBlock<uint_2>, NDataBlock<uint_2>);
  PPRegister(FIO_NDataBlock<int_2>);
  DObjRegister(FIO_NDataBlock<int_2>, NDataBlock<int_2>);
  PPRegister(FIO_NDataBlock<int_4>);
  DObjRegister(FIO_NDataBlock<int_4>, NDataBlock<int_4>);
  PPRegister(FIO_NDataBlock<int_8>);
  DObjRegister(FIO_NDataBlock<int_8>, NDataBlock<int_8>);
  PPRegister(FIO_NDataBlock<uint_4>);
  DObjRegister(FIO_NDataBlock<uint_4>, NDataBlock<uint_4>);
  PPRegister(FIO_NDataBlock<uint_8>);
  DObjRegister(FIO_NDataBlock<uint_8>, NDataBlock<uint_8>);
  PPRegister(FIO_NDataBlock<r_4>);
  DObjRegister(FIO_NDataBlock<r_4>, NDataBlock<r_4>);
  PPRegister(FIO_NDataBlock<r_8>);
  DObjRegister(FIO_NDataBlock<r_8>, NDataBlock<r_8>);
  PPRegister(FIO_NDataBlock< complex<r_4> >);
  DObjRegister(FIO_NDataBlock< complex<r_4> >, NDataBlock< complex<r_4> >);
  PPRegister(FIO_NDataBlock< complex<r_8> >);
  DObjRegister(FIO_NDataBlock< complex<r_8> >, NDataBlock< complex<r_8> >);
#ifdef SO_LDBLE128
  PPRegister(FIO_NDataBlock<r_16>);
  DObjRegister(FIO_NDataBlock<r_16>, NDataBlock<r_16>);
  PPRegister(FIO_NDataBlock< complex<r_16> >);
  DObjRegister(FIO_NDataBlock< complex<r_16> >, NDataBlock< complex<r_16> >);
#endif
  // Enregistrement des handlers PPF pour les TimeStamp
  PPRegister(ObjFileIO<TimeStamp>);
  DObjRegister(ObjFileIO<TimeStamp>, TimeStamp);

  // Enregistrement des handlers PPF pour les DVList
  PPRegister(ObjFileIO<DVList>);
  DObjRegister(ObjFileIO<DVList>, DVList);

  // Enregistrement des handlers PPF pour les RandomGenerator
  PPRegister(ObjFileIO<DR48RandGen>);
  DObjRegister(ObjFileIO<DR48RandGen>,DR48RandGen );
  PPRegister(ObjFileIO<ThSDR48RandGen>);
  DObjRegister(ObjFileIO<ThSDR48RandGen>, ThSDR48RandGen);
  PPRegister(ObjFileIO<FMTRandGen>);
  DObjRegister(ObjFileIO<FMTRandGen>, FMTRandGen);
  PPRegister(ObjFileIO<TinyMT32RandGen>);
  DObjRegister(ObjFileIO<TinyMT32RandGen>, TinyMT32RandGen);
  PPRegister(ObjFileIO<TinyMT64RandGen>);
  DObjRegister(ObjFileIO<TinyMT64RandGen>, TinyMT64RandGen);

  // Enregistrement des handlers PPF pour les SegDataBlock<T> 
  PPRegister(FIO_SegDataBlock<char>);
  DObjRegister(FIO_SegDataBlock<char>, SegDataBlock<char>);
  PPRegister(FIO_SegDataBlock<uint_2>);
  DObjRegister(FIO_SegDataBlock<uint_2>, SegDataBlock<uint_2>);
  PPRegister(FIO_SegDataBlock<int_2>);
  DObjRegister(FIO_SegDataBlock<int_2>, SegDataBlock<int_2>);
  PPRegister(FIO_SegDataBlock<int_4>);
  DObjRegister(FIO_SegDataBlock<int_4>, SegDataBlock<int_4>);
  PPRegister(FIO_SegDataBlock<int_8>);
  DObjRegister(FIO_SegDataBlock<int_8>, SegDataBlock<int_8>);
  PPRegister(FIO_SegDataBlock<uint_4>);
  DObjRegister(FIO_SegDataBlock<uint_4>, SegDataBlock<uint_4>);
  PPRegister(FIO_SegDataBlock<uint_8>);
  DObjRegister(FIO_SegDataBlock<uint_8>, SegDataBlock<uint_8>);
  PPRegister(FIO_SegDataBlock<r_4>);
  DObjRegister(FIO_SegDataBlock<r_4>, SegDataBlock<r_4>);
  PPRegister(FIO_SegDataBlock<r_8>);
  DObjRegister(FIO_SegDataBlock<r_8>, SegDataBlock<r_8>);
  PPRegister(FIO_SegDataBlock< complex<r_4> >);
  DObjRegister(FIO_SegDataBlock< complex<r_4> >, SegDataBlock< complex<r_4> >);
  PPRegister(FIO_SegDataBlock< complex<r_8> >);
  DObjRegister(FIO_SegDataBlock< complex<r_8> >, SegDataBlock< complex<r_8> >);
  PPRegister(FIO_SegDataBlock<string>);
  DObjRegister(FIO_SegDataBlock<string>, SegDataBlock<string>);
  PPRegister(FIO_SegDataBlock<TimeStamp>);
  DObjRegister(FIO_SegDataBlock<TimeStamp>, SegDataBlock<TimeStamp>);

#ifdef SO_LDBLE128
  PPRegister(FIO_SegDataBlock<r_16>);
  DObjRegister(FIO_SegDataBlock<r_16>, SegDataBlock<r_16>);
  PPRegister(FIO_SegDataBlock< complex<r_16> >);
  DObjRegister(FIO_SegDataBlock< complex<r_16> >, SegDataBlock< complex<r_16> >);
#endif

  // Enregistrement des handlers PPF pour les SwPPFSegDataBlock<T> 
  PPRegister(FIO_SwPPFSegDataBlock<char>);
  DObjRegister(FIO_SwPPFSegDataBlock<char>, SwPPFSegDataBlock<char>);
  PPRegister(FIO_SwPPFSegDataBlock<uint_2>);
  DObjRegister(FIO_SwPPFSegDataBlock<uint_2>, SwPPFSegDataBlock<uint_2>);
  PPRegister(FIO_SwPPFSegDataBlock<int_2>);
  DObjRegister(FIO_SwPPFSegDataBlock<int_2>, SwPPFSegDataBlock<int_2>);
  PPRegister(FIO_SwPPFSegDataBlock<int_4>);
  DObjRegister(FIO_SwPPFSegDataBlock<int_4>, SwPPFSegDataBlock<int_4>);
  PPRegister(FIO_SwPPFSegDataBlock<int_8>);
  DObjRegister(FIO_SwPPFSegDataBlock<int_8>, SwPPFSegDataBlock<int_8>);
  PPRegister(FIO_SwPPFSegDataBlock<uint_4>);
  DObjRegister(FIO_SwPPFSegDataBlock<uint_4>, SwPPFSegDataBlock<uint_4>);
  PPRegister(FIO_SwPPFSegDataBlock<uint_8>);
  DObjRegister(FIO_SwPPFSegDataBlock<uint_8>, SwPPFSegDataBlock<uint_8>);
  PPRegister(FIO_SwPPFSegDataBlock<r_4>);
  DObjRegister(FIO_SwPPFSegDataBlock<r_4>, SwPPFSegDataBlock<r_4>);
  PPRegister(FIO_SwPPFSegDataBlock<r_8>);
  DObjRegister(FIO_SwPPFSegDataBlock<r_8>, SwPPFSegDataBlock<r_8>);
  PPRegister(FIO_SwPPFSegDataBlock< complex<r_4> >);
  DObjRegister(FIO_SwPPFSegDataBlock< complex<r_4> >, SwPPFSegDataBlock< complex<r_4> >);
  PPRegister(FIO_SwPPFSegDataBlock< complex<r_8> >);
  DObjRegister(FIO_SwPPFSegDataBlock< complex<r_8> >, SwPPFSegDataBlock< complex<r_8> >);
  PPRegister(FIO_SwPPFSegDataBlock<string>);
  DObjRegister(FIO_SwPPFSegDataBlock<string>, SwPPFSegDataBlock<string>);
  PPRegister(FIO_SwPPFSegDataBlock<TimeStamp>);
  DObjRegister(FIO_SwPPFSegDataBlock<TimeStamp>, SwPPFSegDataBlock<TimeStamp>);

#ifdef SO_LDBLE128
  PPRegister(FIO_SwPPFSegDataBlock<r_16>);
  DObjRegister(FIO_SwPPFSegDataBlock<r_16>, SwPPFSegDataBlock<r_16>);
  PPRegister(FIO_SwPPFSegDataBlock< complex<r_16> >);
  DObjRegister(FIO_SwPPFSegDataBlock< complex<r_16> >, SwPPFSegDataBlock< complex<r_16> >);
#endif

  // Enregistrement des handlers PPF pour les vecteurs de la STL
  PPRegister(PPFWrapperSTLVector<char>);
  DObjRegister(PPFWrapperSTLVector<char>, std::vector<char>);
  PPRegister(PPFWrapperSTLVector<uint_2>);
  DObjRegister(PPFWrapperSTLVector<uint_2>, std::vector<uint_2>);
  PPRegister(PPFWrapperSTLVector<int_2>);
  DObjRegister(PPFWrapperSTLVector<int_2>, std::vector<int_2>);
  PPRegister(PPFWrapperSTLVector<int_4>);
  DObjRegister(PPFWrapperSTLVector<int_4>, std::vector<int_4>);
  PPRegister(PPFWrapperSTLVector<int_8>);
  DObjRegister(PPFWrapperSTLVector<int_8>, std::vector<int_8>);
  PPRegister(PPFWrapperSTLVector<uint_4>);
  DObjRegister(PPFWrapperSTLVector<uint_4>, std::vector<uint_4>);
  PPRegister(PPFWrapperSTLVector<uint_8>);
  DObjRegister(PPFWrapperSTLVector<uint_8>, std::vector<uint_8>);
  PPRegister(PPFWrapperSTLVector<r_4>);
  DObjRegister(PPFWrapperSTLVector<r_4>, std::vector<r_4>);
  PPRegister(PPFWrapperSTLVector<r_8>);
  DObjRegister(PPFWrapperSTLVector<r_8>, std::vector<r_8>);
  PPRegister(PPFWrapperSTLVector< complex<r_4> >);
  DObjRegister(PPFWrapperSTLVector< complex<r_4> >, std::vector< complex<r_4> >);
  PPRegister(PPFWrapperSTLVector< complex<r_8> >);
  DObjRegister(PPFWrapperSTLVector< complex<r_8> >, std::vector< complex<r_8> >);
#ifdef SO_LDBLE128
  PPRegister(PPFWrapperSTLVector<r_16>);
  DObjRegister(PPFWrapperSTLVector<r_16>, std::vector<r_16>);
  PPRegister(PPFWrapperSTLVector< complex<r_16> >);
  DObjRegister(PPFWrapperSTLVector< complex<r_16> >, std::vector< complex<r_16> >);
#endif

  PPRegister(PPFWrapperSTLVector< string >);
  DObjRegister(PPFWrapperSTLVector< string >, std::vector<string>);

  PPRegister(PPFWrapperSTLVector< TimeStamp >);
  DObjRegister(PPFWrapperSTLVector< TimeStamp >, std::vector<TimeStamp>);

  // Avr 2012: Enregistrement des handlers PPF pour les classes Units et PhysQty (unites et grandeurs physiques) 
  PPRegister(ObjFileIO<Units>);
  DObjRegister(ObjFileIO<Units>, Units);
  PPRegister(ObjFileIO<PhysQty>);
  DObjRegister(ObjFileIO<PhysQty>, PhysQty);


#if (!defined(__GNUG__) && !defined(HPUX))
#ifndef _MSC_VER //G.Barrand
  // pas de bufferisation pour printf   cmv 18/3/97 selon E.A.
  // setvbuf(stdout,NULL,_IOLBF,0); setvbuf(stderr,NULL,_IOLBF,0);
  setlinebuf(stdout);
  setlinebuf(stderr);
#endif  
#endif

  // si var env SOPHYA_NOPRTVER definie pas de print
  if(!getenv("SOPHYA_NOPRTVER")) PrintVersion(false);

  int pnice;
  char* snice = getenv("SOPHYA_NICE");
  if (!snice) pnice=8;
  else pnice = atoi(snice);
  nice(pnice);

  SophyaInitiator::RegisterModule("BaseTools", MOD_VERS);  // Module name and version number registration  
}

SophyaInitiator::~SophyaInitiator()
{
  FgInit--;
/*
  if (FgInit == 0)
    {
    delete PPersistMgr::classList; PPersistMgr::classList = NULL;
    delete PShPersist::objList;    PShPersist::objList = NULL;
    }
    */
}

/*! 
  \brief Return the SOPHYA version number. 
  \param svers contain the complete in addition the SOPHYA tag, the compiler name 
  and the compilation date
*/
double SophyaInitiator::GetVersion(string& svers)
{
  const char* compiler = 0;
  #ifdef __GNUG__
  compiler = "gcc " __VERSION__;
  #endif
  #ifdef __DECCXX
  compiler = "cxx " ;
  #endif
  #ifdef __aCC__
  compiler = const_cast<char *>("HP-aCC ") ;
  #endif
  #ifdef __KCC__
  compiler = const_cast<char *>("KCC ") ;
  #endif
  #ifdef __IBMCPP__
  #ifdef SO_ARCH64
  compiler = const_cast<char *>("IBM-xlC (-q64)") ;
  #else
  compiler = const_cast<char *>("IBM-xlC") ;
  #endif
  #endif
  #ifdef __INTEL_COMPILER
  compiler = const_cast<char *>("Intel-icc ") ;
  #endif
  #ifdef __SGICC__
  #ifdef SO_ARCH64
  compiler = const_cast<char *>("SGI-CC (-64) ") ;
  #else
  compiler = const_cast<char *>("SGI-CC ") ;
  #endif
  #endif

  char buff[512];
  sprintf(buff,"SOPHYA Version %4.1f Revision %d (%s) -- %s %s %s",
          SOPHYA_VERSION, SOPHYA_REVISION, SOPHYA_TAG,
          __DATE__, __TIME__, compiler);
  svers = buff;
  
  return(SOPHYA_VERSION + (SOPHYA_REVISION/1000.));
}

//! Print the SOPHYA version string and optionaly the list of registered modules
void SophyaInitiator::PrintVersion(bool fglist)
{
  string svers;
  GetVersion(svers);
  cout << svers << endl;
  if (fglist) ListModules(cout);   
}

//! Should be called by sub-classes to register module name and version 
int SophyaInitiator::RegisterModule(const char * name, double version)
{
  if (ModListP == NULL) 
    throw NullPtrError("SophyaInitiator::RegisterModule() ModListP= NULL !");
  map<string, double>& modlist = *ModListP;
  modlist[string(name)] = version;
  return int(modlist.size());
}

//! List of registered module names and version number
int SophyaInitiator::ListModules(ostream& os)
{
  if (ModListP == NULL) 
    throw NullPtrError("SophyaInitiator::ListModules() ModListP= NULL !");
  os << "--- SophyaInitiator::ListModules() Name / VersionNumber --- " << endl;
  map<string, double>& modlist = *ModListP;
  int k = 1;
  for(map<string, double>::iterator it = modlist.begin(); it != modlist.end(); it++, k++) 
    os << k << " : " << (*it).first << " V= " << (*it).second << endl; 
  os << " ----------------------------------------------------------- " << endl;
  return modlist.size();
}

//! Return the SOPHYA version number: VERS + REV/1000
double SOPHYA::SophyaVersion()
{
return(SOPHYA_VERSION + (SOPHYA_REVISION/1000.));
}
 
// On met un objet initiator en statique, pour les loaders qui savent 
// appeler le constructeur des objets statiques   Reza 08/98
// La presence de l'objet statique psophyainit semble poserun probleme 
// sur MacOSX 10.2 qui se plante a l'initialisation avec les shared-libs
// Suppression de #if !defined(Darwin) en Juil 2006
static SophyaInitiator psophyainit;

