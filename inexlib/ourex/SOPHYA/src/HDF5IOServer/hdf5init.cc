/*    
   --- SOPHYA software - HDF5IOServer module ---
   R. Ansari,   2018
   (C) UPS+LAL IN2P3/CNRS     (C) IRFU-SPP/CEA 

   Classe d'initialisation du module HDF5IOServer
*/

#include <iostream> 
#include "machdefs.h"
#include "hdf5init.h"
#include "hdf5manager.h"
#include "hdf5inoutfile.h"

#include "hdf5handler.h"
#include "hdf5arrhand.h"
#include "hdf5ntuphand.h"
#include "hdf5segdbhand.h"
#include "hdf5dtablehand.h"

using namespace std;

namespace SOPHYA {

/*! 
  \class HDF5IOServerInitiator
  \ingroup HDF5IOServer
  Perform registration of HDF5 handlers.
  \sa SOPHYA::HDF5Manager
 */

// Module version number history 
//  HDF5IOServer V= 0.7 , July 2018
//  HDF5IOServer V= 0.8 , Jan 2019
//  HDF5IOServer V= 0.9 , October 2019
//  HDF5IOServer V= 1.0 , November 2019 
#define HDF5IOServer_MOD_VERS   1.0

double HDF5IOServer_Module_Version()
{
  return HDF5IOServer_MOD_VERS;
}


int HDF5IOServerInitiator::FgInit = 0;

HDF5IOServerInitiator::HDF5IOServerInitiator()
  : SophyaInitiator()
{
  HDF5IOServerInitiator::FgInit++;
  if (HDF5IOServerInitiator::FgInit > 1)  return;

  //--- Initialisation diverses, specifique aux classes interfaces avec la librairie HDF5 
  HDF5IdWrapper::doInit();
  HDF5Types::doInit();

  //---- Enregistrement des classes HDF5Handler
  /* RegisterHandler(HDF5HandlerInterface * fhi, int glev, string desc)
    Generic handlers (read/write datasets from/to arrays) will have a higher global priority level
    than specific handlers. Use glev=0 for specific handlers and glev=1 for generic handlers.  */

  //DBG cout << " ---- HDF5IOServerInitiator / DEBUG ---- debut RegisterHandler " << endl;
  HDF5Manager::RegisterHandler(new HDF5ArrayHandler<int_1> , 1, "TArray<int_1>");
  HDF5Manager::RegisterHandler(new HDF5ArrayHandler<int_2> , 1, "TArray<int_2>");
  HDF5Manager::RegisterHandler(new HDF5ArrayHandler<int_4> , 1, "TArray<int_4> ");
  HDF5Manager::RegisterHandler(new HDF5ArrayHandler<int_8> , 1, "TArray<int_8> ");
  
  HDF5Manager::RegisterHandler(new HDF5ArrayHandler<uint_1> , 1, "TArray<uint_1>");
  HDF5Manager::RegisterHandler(new HDF5ArrayHandler<uint_2> , 1, "TArray<uint_2>");
  HDF5Manager::RegisterHandler(new HDF5ArrayHandler<uint_4> , 1, "TArray<uint_4>");
  HDF5Manager::RegisterHandler(new HDF5ArrayHandler<uint_8> , 1, "TArray<uint_8>");

  HDF5Manager::RegisterHandler(new HDF5ArrayHandler<r_4> , 1, "TArray<r_4>");
  HDF5Manager::RegisterHandler(new HDF5ArrayHandler<r_8> , 1, "TArray<r_8>");

  HDF5Manager::RegisterHandler(new HDF5ArrayHandler< complex<r_4> > , 1, "TArray< complex<r_4> >");
  HDF5Manager::RegisterHandler(new HDF5ArrayHandler< complex<r_8> > , 1, "TArray< complex<r_8> >");

#ifdef  SO_LDBLE128
  HDF5Manager::RegisterHandler(new HDF5ArrayHandler<r_16> , 1, "TArray<r_8>");
  HDF5Manager::RegisterHandler(new HDF5ArrayHandler< complex<r_16> > , 1, "TArray< complex<r_8> >");
#endif
  //   Handler de NTuple 
  HDF5Manager::RegisterHandler(new HDF5Handler<NTuple>, 0, "NTuple");

  //   Handler de SegDataBlock  
  HDF5Manager::RegisterHandler(new HDF5SegDBHandler<int_4> , 0, "SegDataBlock<int_4>");
  HDF5Manager::RegisterHandler(new HDF5SegDBHandler<int_8> , 0, "SegDataBlock<int_8>");

  HDF5Manager::RegisterHandler(new HDF5SegDBHandler<r_4> , 0, "SegDataBlock<r_4>");
  HDF5Manager::RegisterHandler(new HDF5SegDBHandler<r_8> , 0, "SegDataBlock<r_8>");

  HDF5Manager::RegisterHandler(new HDF5SegDBHandler< complex<r_4> > , 0, "SegDataBlock<r_4>");
  HDF5Manager::RegisterHandler(new HDF5SegDBHandler< complex<r_8> > , 0, "SegDataBlock<r_8>");

#ifdef  SO_LDBLE128
  HDF5Manager::RegisterHandler(new HDF5SegDBHandler<r_16> , 0, "SegDataBlock<r_8>");
  HDF5Manager::RegisterHandler(new HDF5SegDBHandler< complex<r_16> > , 0, "SegDataBlock<r_8>");
#endif

  HDF5Manager::RegisterHandler(new HDF5SegDBHandler< TimeStamp > , 1, "SegDataBlock<TimeStamp>");

  HDF5Manager::RegisterHandler(new HDF5SegDBHandler< FMLString<16> > , 1, "SegDataBlock< FMLString<16> >");
  HDF5Manager::RegisterHandler(new HDF5SegDBHandler< FMLString<32> > , 1, "SegDataBlock< FMLString<32> >");
  HDF5Manager::RegisterHandler(new HDF5SegDBHandler< FMLString<64> > , 1, "SegDataBlock< FMLString<64> >");
  HDF5Manager::RegisterHandler(new HDF5SegDBHandler< FMLString<128> > , 1, "SegDataBlock< FMLString<128> >");

  // Handler des BaseDataTable
  HDF5Manager::RegisterHandler(new HDF5Handler<BaseDataTable>, 1, "DataTable");

  /*
  HDF5Manager::RegisterHandler(new HDF5SphereHEALPix<r_4> , 0, "SphereHEALPix<r_4>");
  HDF5Manager::RegisterHandler(new HDF5SphereHEALPix<r_8> , 0, "SphereHEALPix<r_8>");
  HDF5Manager::RegisterHandler(new HDF5SphereHEALPix<int_4> , 0, "SphereHEALPix<int_4>");
  */

  //DBG  cout << " ---- HDF5IOServerInitiator / DEBUG ---- ListHandlers() : " << endl;
  //DBG  HDF5Manager::ListHandlers();

  SophyaInitiator::RegisterModule("HDF5IOServer", HDF5IOServer_MOD_VERS);  // Module name and version number registration
}

HDF5IOServerInitiator::~HDF5IOServerInitiator()
{
  HDF5IOServerInitiator::FgInit--;
}


// On met un objet initiator en statique, pour les loaders qui savent 
// appeler le constructeur des objets statiques  
// #if !defined(Darwin)
// Probleme rencontre de nouveau en Juillet 2006 sur MacOS :
// La presence de l'objet statique fait planter les progs linkes libextsophya.dylib
// static HDF5IOServerInitiator s_hdf5_init_;
// #endif

// Reactivation de l'objet statique en Jan 2019 
static HDF5IOServerInitiator s_hdf5_init_;
} // Fin du namespace SOPHYA
