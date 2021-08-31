/*    
   --- SOPHYA software - HDF5IOServer module ---
   R. Ansari, 2005-2018
   (C) UPS+LAL IN2P3/CNRS     (C) IRFU-SPP/CEA 
*/
#ifndef HDF5HANDLER_H_SEEN
#define HDF5HANDLER_H_SEEN 

#include "machdefs.h"
#include <string>
#include <typeinfo>
#include "hdf5inoutfile.h"

namespace SOPHYA {

/*! 
  \ingroup HDF5IOServer
  \brief Interface definition for classes handling object storage and retrieval to/from HDF5 files 
*/
class HDF5HandlerInterface {

public:
  HDF5HandlerInterface() { SetMaxNbChunks(); }
  //! define maximum number of chunks used to write an object - adjust HDF5 file chunk size if applicable
  inline void SetMaxNbChunks(size_t maxnchunk=64) { max_nchunk_=maxnchunk; }
  virtual ~HDF5HandlerInterface() {}
  //! Return the real data object
  virtual AnyDataObj* DataObj() = 0;       // Retourne l'objet reel 

  /*! 
    \brief Return a positive value if HDF5 I/O for object \b o can be handled

    - Rc= 0 -> Can NOT handle HDF5 I/O operations for the object \b o 
    - Rc= 1 -> Can handle HDF5 I/O operations for the object \b o 
    - Rc= 2 -> This is a specific handler for object \b o
    - Rc > 2 -> Higher Rc values can be returned if needed 
      (for subclasses with specific handlers)

  */
  virtual int         CheckHandling(AnyDataObj const & o) const  = 0;
  //! Read/write operation will use the object o
  virtual void        SetDataObj(AnyDataObj & o) = 0;

  //! 
  /*! 
    \brief Return a positive value if specified object defined by \b objpath can be read by the handler

    - Rc= 0 -> Can NOT read the object at \b objpath 
    - Rc= 1 -> Can read the the object at \b objpath  (generic reader)
    - Rc= 2 -> Can read the the object at \b objpath, as a specific reader
    - Rc > 2 -> Higher Rc values can be returned if needed 
      (when multiple specific handlers are registered) 

  */
  virtual int         CheckReadability(HDF5InOutFile& is, const char* objpath) const = 0;
  
  //! Clone (duplicate) the handler class
  virtual HDF5HandlerInterface* Clone() const = 0;

  //! Perform the actual write operation to the output fits file
  virtual void        Write(HDF5InOutFile& os, const char* objpath) const = 0;
  //! Perform the actual read operation from input fits file
  virtual void        Read(HDF5InOutFile& is, const char* objpath) = 0;  
protected:
  size_t max_nchunk_;   // Maximum number of chunks in HDF5 file 
};

/*! 
  \ingroup HDF5IOServer
  \brief Generic implementation of HDF5HandlerInterface 
*/
template <class T>
class HDF5Handler : public HDF5HandlerInterface {

public :
	    HDF5Handler() { dobj=NULL; ownobj=true; }
	    HDF5Handler(T & obj) { dobj = &obj; ownobj=false; }
  virtual   ~HDF5Handler() { if (ownobj && dobj) delete dobj; }

  virtual   AnyDataObj* DataObj() { return(dobj); }
  virtual   int         CheckHandling(AnyDataObj const & o) const
  {  
    if (typeid(o) == typeid(T)) return 2;
    T const * po = dynamic_cast< T const * >(& o); 
    if (po == NULL) return 0;
    else return 1;
  }
  virtual   void        SetDataObj(AnyDataObj & o) 
  {  
    T * po = dynamic_cast< T * >(& o); 
    if (po == NULL)  {
      string msg = "HDF5Handler<T>::SetDataObj() Wrong object type: " ;
      msg += typeid(o).name(); 
      throw TypeMismatchExc(msg);    
    }
    if (ownobj && dobj) delete dobj;  dobj = po; ownobj = false; 
  }

  virtual int         CheckReadability(HDF5InOutFile& is, const char* objpath) const;
    
  virtual HDF5HandlerInterface* Clone() const 
    { return new HDF5Handler<T>() ; }

  inline operator T&() { return(*dobj); }

  virtual void        Read(HDF5InOutFile& is, const char* objpath);
  virtual void        Write(HDF5InOutFile& os, const char* objpath) const;

protected :
  T * dobj;
  bool ownobj;       // True si dobj obtenu par new
};



} // Fin du namespace

#endif   /* HDF5HANDLER_H_SEEN */


