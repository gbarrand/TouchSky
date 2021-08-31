/*    
   --- SOPHYA software - HDF5IOServer module ---
   R. Ansari,   2018
   (C) UPS+LAL IN2P3/CNRS     (C) IRFU-SPP/CEA 
*/

//  Adapte de la classe FitsManager (FitsIOServer/fitsmanager.h .cc)  

#include "machdefs.h"
#include "sopnamsp.h"

#include <stdio.h>
#include <string.h>
#include <iostream>
#include <typeinfo>

#include "hdf5manager.h"
#include "hdf5handler.h"

/*! 
  \class SOPHYA::HDF5Manager
  \ingroup HDF5IOServer

  \brief Top level HDF5 I/O operations and handler management

  This class manages the HDF5 I/O handler classes for SOPHYA objects 
  and provide high level interface for reading/writing SOPHYA objets 
  from / to HDF5 files.

  \code
  //-- Scan the content of an HDF5 file and list all groups and datasets
  string h5filename="toto.h5";
  bool fgatt=true;   // we want also the list of attributes for each data set and each group 
  HDF5Manager::ScanFile(h5filename, fgatt);
  \endcode 

  \code
  //-- Try to read a specific data set from file 
  string h5filename="toto.h5";
  string dsname="mydata";
  // 1- Open the file 
  HDF5InOutFile h5is(h5filename, HDF5InOutFile::RO);
  // 2- Read the object 
  HDF5HandlerInterface * h5h= HDF5Manager::Read(h5is, dsname); 
  if (h5h) cout << " HDF5Manager::Read()  OK "<<endl;
  else " cout << " HDF5Manager::Read()  ERROR "<<endl;
  \endcode 
  
*/

using namespace std;

namespace SOPHYA {


struct h5hand_list_el {
  HDF5HandlerInterface * fhi;
  int glev;
  string desc;
};

typedef list<h5hand_list_el> H5HandlerList;

static H5HandlerList * hlistp = NULL;

static inline void ChkH5HLP() 
{
  if (hlistp == NULL) hlistp = new H5HandlerList;
}
/*!
  Generic handlers (read/write datasets from/to arrays) will have a higher global priority level
  than specific handlers. Use glev=0 for specific handlers and glev=1 for generic
  handlers.  
  \param fhi : handler object pointer (created by new) which will be kept in the 
  handler list
  \param glev : global priority level associated with the handler. Used when different
  handlers return the same value for CheckHandling() or CheckReadability().
  \param desc : classe name description associated with the handler
*/
int HDF5Manager::RegisterHandler(HDF5HandlerInterface * fhi, int glev, string desc)
{
  ChkH5HLP(); 
  if (fhi == NULL)
    throw NullPtrError("HDF5Manager::RegisterHandler() fhi=NULL ");
  HDF5HandlerInterface & rfhi=*fhi;   // 03/2018: pour eviter warning compilo argument to typeid with side effects
  H5HandlerList::iterator it;
  for(it = hlistp->begin(); it != hlistp->end(); it++) {
    HDF5HandlerInterface & ritfhi = *((*it).fhi);  // 03/2018: pour eviter warning compilo - voir ci-dessus
    if (typeid(ritfhi) == typeid(rfhi)) 
      throw DuplicateIdExc("HDF5Manager::RegisterHandler() Already registered handler");
  }
  h5hand_list_el hle;
  hle.fhi = fhi;
  hle.glev = glev;
  hle.desc = desc;
  hlistp->push_back(hle);
  return hlistp->size();
}

int HDF5Manager::ListHandlers()
{
  ChkH5HLP(); 
  int kk=0;
  cout << "---- HDF5Manager::ListHandlers()  NbHandlers= " << hlistp->size()
       << endl;
  H5HandlerList::iterator it;
  for(it = hlistp->begin(); it != hlistp->end(); it++) {
    kk++;
    HDF5HandlerInterface & ritfhi = *((*it).fhi);  // 03/2018: pour eviter warning compilo - voir ci-dessus
    cout << kk << "- " << (*it).desc << " : " 
	 << typeid(ritfhi).name() << " glev= " <<(*it).glev << endl;
  }
  return hlistp->size();
}

/*!
  Return a NULL pointer if no handler is registered for object \b o .
  The returned pointer should not be used directly and should NOT
  be deleted. For read/write operations, the Clone() method should
  be called on the returned object.
*/
HDF5HandlerInterface* HDF5Manager::FindHandler(AnyDataObj const & o)
{
  ChkH5HLP(); 
  HDF5HandlerInterface * fhi = NULL;
  H5HandlerList::iterator it;
  int hfg = 0;
  int bhfg = 0;
  //unused: int clev = 0;
  int blev = 0;
  for(it = hlistp->begin(); it != hlistp->end(); it++) {
    hfg = (*it).fhi->CheckHandling(o);
    if (hfg < 1) continue;
    if ( ( hfg > bhfg ) || ( (hfg == bhfg) && ((*it).glev > blev) ) ) {
      fhi = (*it).fhi;  bhfg = hfg;  blev = (*it).glev;
    }
  }
  return fhi ;
}

/*!
  Throws an exception ( NotFoundExc ) if the write operation can not be performed. 
*/
void HDF5Manager::Write(HDF5InOutFile& os, string const& opath, AnyDataObj const & o)
{
  HDF5HandlerInterface * fhi = FindHandler(o);
  if (fhi == NULL) {
    string msg = "HDF5Manager::Write()/FindHandler() Handler not found for ";
    msg += typeid(o).name();
    throw NotFoundExc(msg);
  }
  HDF5HandlerInterface * fhi2 = fhi->Clone();
  //DBG  cout << " **DBG**HDF5Manager::Write() Handler found : "<<typeid(*fhi2).name()<< " -> trying to write to DataSet="<<opath<<endl;
  fhi2->SetDataObj(const_cast<AnyDataObj &>(o));
  fhi2->Write(os, opath.c_str());
  delete fhi2 ;
  return;
}

/*!
  Throws an exception ( NotFoundExc ) if the read operation can not be performed. 
*/
void HDF5Manager::Read(HDF5InOutFile& is, string const& opath, AnyDataObj & o)
{
  HDF5HandlerInterface * fhi = FindHandler(o);
  if (fhi == NULL) {
    string msg = "HDF5Manager::Read()/FindHandler() Handler not found for ";
    msg += typeid(o).name();
    throw NotFoundExc(msg);
  }
  HDF5HandlerInterface * fhi2 = fhi->Clone();
  fhi2->SetDataObj(o);
  fhi2->Read(is, opath.c_str());
  delete fhi2;
  return;
}

/*!
  Return a NULL pointer if no handler is found capable of reading
  the specified dataset or group.
  The returned pointer should not be used directly and should NOT
  be deleted. For read/write operations, the Clone() method should
  be called on the returned object.
*/
HDF5HandlerInterface * HDF5Manager::FindReader(HDF5InOutFile& is, string const& path)
{
  ChkH5HLP(); 
  HDF5HandlerInterface * fhi = NULL;
  H5HandlerList::iterator it;
  int hfg = 0;
  int bhfg = 0;
  int blev = 0;
  for(it = hlistp->begin(); it != hlistp->end(); it++) {
    hfg = (*it).fhi->CheckReadability(is, path.c_str());
    if (hfg < 1) continue;
    if ( ( hfg > bhfg ) || ( (hfg == bhfg) && ((*it).glev > blev) ) ) {
      fhi = (*it).fhi;  bhfg = hfg;  blev = (*it).glev;
    }
  }
  return fhi;
}

/*!
  Throws an exception ( NotFoundExc ) if the read operation can not be performed. 
  In case of success, the calling function is responsible for deleting 
  the returned HDF5HandlerInterface object, when the correspondind data object
  is not needed any more.
*/
HDF5HandlerInterface * HDF5Manager::Read(HDF5InOutFile& is,  string const& path)
{
  HDF5HandlerInterface * fhi = FindReader(is, path);
  if (fhi == NULL) {
    string msg = "HDF5Manager::Read()/FindReader() Reader/Handler not found  ";
    throw NotFoundExc(msg);
  }
  HDF5HandlerInterface * fhi2 = fhi->Clone();
  fhi2->Read(is, path.c_str());
  return fhi2;
}

/*!
  \param filename : HDF5 file name to be scanned
  \param os : infomation will be sent (printed) to formatted output stream os
  \param fgpatt : If true, print the attributes associated with each dataset or group

  RC : return number of scanned Datasets 
 */
  int HDF5Manager::ScanFile(string filename, ostream& os, bool fgpatt)
{
  HDF5InOutFile h5f(filename, HDF5InOutFile::RO);
  os << "=== HDF5Manager::ScanFile( " << filename << " )  FgPrintAtt = "<<(fgpatt?"YES":"NO")<<endl;
  int n =0;
  n=ScanGroup(h5f, "/", os, fgpatt);
  return n;
}

int HDF5Manager::ScanGroup(HDF5InOutFile &h5f, string const & grp, ostream& os, bool fgpatt)
{
  HDF5IdWrapper g1 = h5f.CurrentGroup();
  HDF5IdWrapper g2 = h5f.OpenGroup(grp);
  h5f.SetCurrentGroup(g2);
  std::vector<std::string> dslist;
  std::vector<std::string> grplist;
  int n=h5f.GetGroupMemberList(g2, dslist, grplist);
  int ncnt=0;
  os << " ---------------- List of DataSets in group " << grp << endl;
  for(size_t i=0; i<(size_t)dslist.size(); i++) {
    os << "=== "<<i+1<<" - DataSetName: "<<dslist[i]<<endl;
    HDF5IdWrapper datatype, dataspace;
    HDF5IdWrapper dataset = h5f.OpenDataSet(dslist[i], datatype, dataspace);
    os << datatype << "  " << dataspace << endl;
    if (fgpatt) {
      DVList att;
      h5f.ReadAttributes(dataset, att);
      os << att;
    }
    ncnt++;
  }
  os << " ---------------- List of groups in group " << grp << endl;
  for(size_t i=0; i<(size_t)grplist.size(); i++) {
    os << "=== "<<i+1<<" - GroupName: "<<grplist[i]<<endl;
    if (fgpatt) {
      HDF5IdWrapper cgrp=h5f.OpenGroup(grplist[i]);
      DVList att;
      h5f.ReadAttributes(cgrp, att);
      os << att;
    }

  }  
  for(size_t i=0; i<(size_t)grplist.size(); i++) {
    string grp2;
    if (grp == "/") grp2=grp+grplist[i];
    else  grp2=grp+'/'+grplist[i];
    ncnt += ScanGroup(h5f, grp2, os, fgpatt);
  }
  h5f.SetCurrentGroup(g1);
  return ncnt;
}

} // namespace SOPHYA
