//-----------------------------------------------------------
// SOPHYA class library - (C)  UPS+LAL IN2P3/CNRS  , CEA-Irfu 
// Gestion de block de donnees avec partage de references
//       R. Ansari -  Mars 2005 , updated Nov 2012
//       J.E Campagne update Oct 2014/Jan 2015
// LAL (Orsay) / IN2P3-CNRS  DAPNIA/SPP (Saclay) / CEA
//-----------------------------------------------------------

#ifndef SEGDATABLOCK_H
#define SEGDATABLOCK_H

#include "machdefs.h"
#include "anydataobj.h"
#include <string.h> 
#include <vector>
#include <iostream>
#include <iomanip>
#include <typeinfo>
#include "ppftpointerio.h"
#include "sunitpcst.h"
#include "thsafeop.h"  //  for ThreadSafe operations (Ref.Count/Share) 

/*!
   \class SOPHYA::SegDBInterface
   \ingroup BaseTools
   Interface definition for segmented data container (template class).
*/
/*!
   \class SOPHYA::SegDataBlock
   \ingroup BaseTools
   Template class impementing segmented data container in memory with 
   management of reference sharing. 
*/
/*!
  \class SOPHYA:: SegIterator
  \ingroup BaseTools
  Random Access Iterator class and associated begin/end methods for SOPHYA::SegDataBlock
*/

namespace SOPHYA {

////////////////////////////////////////////////////////////////
//// -------------- Class SegIterator ----------------------- //
//// ---------------- Class SegDBInterface ------------------ //
//// ----------------- Class SegDataBlock ------------------- //
////////////////////////////////////////////////////////////////
template<class Container, class T>
class SegIterator : public std::iterator<std::random_access_iterator_tag, T> {
public:
  //! Constructor (default)
  SegIterator(): sdb_(NULL), idx_(-1) {}
  //! Constructor
  SegIterator(Container&a, size_t id=0) : sdb_(&a), idx_(id) {}
  //! Copy Constructor
  SegIterator(SegIterator const& sdbit): sdb_(sdbit.sdb_), idx_(sdbit.idx_){}
  
  //! Op= with strong exception guarantee
  SegIterator& operator=(SegIterator  rhs){ //copy/mv constructor is called
    std::swap(sdb_,rhs.sdb_); //standard sswap
    std::swap(idx_,rhs.idx_);
    return *this;
  }
  
  SegIterator& operator++() {idx_++; return *this;} //PREFIX
  SegIterator& operator++(int) {SegIterator tmp(*this);  ++(*this);  return *this;  } //POSTFIX a quoi sert tmp?
  
  SegIterator& operator--() {idx_--; return *this;} //PREFIX
  SegIterator& operator--(int) {SegIterator tmp(*this); --(*this); return *this; } //POSTFIX
  
  SegIterator& operator+=(size_t n) {idx_+= n; return *this; }
  SegIterator& operator-=(size_t n) {idx_ -= n; return *this; }

  SegIterator operator+(size_t n) const {
    return SegIterator(*sdb_, idx_+n);
  }

  SegIterator operator-(size_t n) const {
    return SegIterator(*sdb_, idx_-n);
  }

  
  T& operator*() {
    size_t segsize = (*sdb_).SegmentSize();
    size_t k = idx_/segsize; 
    size_t i =  idx_%segsize;
    T* p = (*sdb_).GetSegment(k);
    return p[i];
  }
  
  T& operator[](size_t n){
    idx_ = n;
    return (this->operator*());
  }
  
  bool operator==(SegIterator const& rhs) { return idx_==rhs.idx_;}
  bool operator!=(SegIterator const& rhs) { return idx_!=rhs.idx_;}
  bool operator<(SegIterator const& rhs) { return idx_<rhs.idx_;}
  bool operator>(SegIterator const& rhs) { return idx_<rhs.idx_;}
  bool operator<=(SegIterator const& rhs) { return idx_<=rhs.idx_;}
  bool operator>=(SegIterator const& rhs) { return idx_>=rhs.idx_;}
  
  //  size_t getIndex() const {return idx_;}

  template<class Container2, class T2>
  friend bool operator==(const SegIterator<Container2,T2> & a, 
		 const SegIterator<Container2,T2> & b);
  template<class Container2, class T2>
  friend bool operator!=(const SegIterator<Container2,T2> & a, 
		 const SegIterator<Container2,T2> & b);
  template<class Container2, class T2>
  friend bool operator<(const SegIterator<Container2,T2> & a, 
		 const SegIterator<Container2,T2> & b);
  template<class Container2, class T2>
  friend bool operator>(const SegIterator<Container2,T2> & a, 
		 const SegIterator<Container2,T2> & b);
  template<class Container2, class T2>
  friend bool operator<=(const SegIterator<Container2,T2> & a, 
		 const SegIterator<Container2,T2> & b);
  template<class Container2, class T2>
  friend bool operator>=(const SegIterator<Container2,T2> & a, 
		 const SegIterator<Container2,T2> & b);



  template<class Container2, class T2>
  friend int operator-(const SegIterator<Container2,T2> & a, 
		 const SegIterator<Container2,T2> & b);

  template<class Container2, class T2>
  friend int operator+(const SegIterator<Container2,T2> & a, 
		 const SegIterator<Container2,T2> & b);
  
protected:
  Container* sdb_; //or a ref? 
  size_t idx_;
};//class SegIterator
  
  
  //   iterator operator - (iterator const & a, size_t n){
  //     iterator tmp(a); tmp -=n; return tmp; 
  //   }

  template<class Container2, class T2>
   bool operator==(const SegIterator<Container2,T2> & a, 
		   const SegIterator<Container2,T2> & b) {
    return (a.idx_ == b.idx_);
  }
  template<class Container2, class T2>
   bool operator!=(const SegIterator<Container2,T2> & a, 
		   const SegIterator<Container2,T2> & b) {
    return (a.idx_ != b.idx_);
  }

  template<class Container2, class T2>
   bool operator<(const SegIterator<Container2,T2> & a, 
		   const SegIterator<Container2,T2> & b) {
    return (a.idx_ < b.idx_);
  }

  template<class Container2, class T2>
   bool operator>(const SegIterator<Container2,T2> & a, 
		   const SegIterator<Container2,T2> & b) {
    return (a.idx_ > b.idx_);
  }

  template<class Container2, class T2>
   bool operator<=(const SegIterator<Container2,T2> & a, 
		   const SegIterator<Container2,T2> & b) {
    return (a.idx_ <= b.idx_);
  }

  template<class Container2, class T2>
   bool operator>=(const SegIterator<Container2,T2> & a, 
		   const SegIterator<Container2,T2> & b) {
    return (a.idx_ >= b.idx_);
  }

  template<class Container2, class T2>
  int operator-(const SegIterator<Container2,T2> & a, const  SegIterator<Container2,T2> & b) {
    return (a.idx_ - b.idx_);
  }
  template<class Container2, class T2>
  int operator+(const  SegIterator<Container2,T2> & a, const  SegIterator<Container2,T2> & b) {
    return (a.idx_ + b.idx_);
  }

 
template <class T>
class SegDBInterface : public AnyDataObj {

public:
  virtual ~SegDBInterface() {}
  //! Changes the data segment size and reallocates the memory segments
  // segsz: Segment size - should be multiple of nbitem;  nbseg: Number of data segments; 
  // nbitem: number of consecutive of data items  
  virtual void SetSize(size_t segsz, size_t nbseg=0, size_t nbitem=1) = 0;
  //! Alias for SetSize()
  inline void  ReSize(size_t segsz, size_t nbseg=0, size_t nbitem=1) { SetSize(segsz, nbseg, nbitem); }
  //! Adds one segment to the data structure - returns the new number of segments.
  virtual size_t Extend() = 0;
  //! Return the segment size data structure, should be multiple of nbitem
  virtual  size_t SegmentSize() const = 0; 
  //! Return the number of data segments
  virtual size_t NbSegments() const = 0;
  //! Return the number of data items in each group (def=1). SegmentSize() is a multiple of NbItems();
  virtual size_t NbItems() const = 0;
  //! Return the current size of the segmented data structure
  inline size_t Size() const    { return  SegmentSize()*NbSegments(); }
  //! Return the pointer to data segment \b k
  virtual T* GetSegment(size_t k) = 0;   
  //! Return the const (read-only) pointer to data segment \b k      
  virtual T const * GetCstSegment(size_t k) const = 0; 
  //! Define or change the physical unit of the data 
  virtual void SetUnits(const Units& un) = 0;
  //! Return the physical unit of the data. Flag sdone is set to true if SetUnits() has been called.
  virtual Units GetUnits(bool& sdone) const = 0;
  //! Return the physical unit of the data 
  virtual Units GetUnits() const = 0;
  /*! 
    \brief Fills the std::vector \b vec from the SegDBInterface object content, up to a maximum of \b maxcnt items  

    \param vec : output vector, which is resized to  SegDBInterface object size or maxcnt 
    \param maxcnt : maximum number of items copied , if maxcnt == 0 , copy all items (set maxcnt to this.Size())

    The output vector is resized to min(maxcnt , this.Size())
  */
  virtual void CopyTo(std::vector<T> & vec, size_t maxcnt=0)  const 
  {
    if (maxcnt < 1)  maxcnt = Size();
    if (maxcnt > Size())  maxcnt = Size();
    vec.resize(Size());
    size_t i=0;
    for(size_t k=0; k<NbSegments(); k++) {
      if (i >= maxcnt)  break;
      T const * ptr = GetCstSegment(k);
      size_t jmax=SegmentSize();
      if (i+jmax > maxcnt)  jmax=maxcnt-i; 
      for(size_t j=0; j<jmax; j++) {
	vec[i]=ptr[j]; i++;
      } 
    }
    return;
  }
  /*! 
    \brief Fills the SegDBInterface object content from std::vector \b vec 

    The number of items copied is equal to min( vec.size() , this->Size() )
  */
  virtual void CopyFrom(std::vector<T> const & vec)  
  {
    size_t maxcnt = ( (Size() < vec.size()) ? Size() : vec.size() );
    if (maxcnt < 1)  return;
    size_t i=0;
    for(size_t k=0; k<NbSegments(); k++) {
      if (i >= maxcnt)  break;
      T * ptr = GetSegment(k);
      size_t jmax=SegmentSize();
      if (i+jmax > maxcnt)  jmax=maxcnt-i; 
      for(size_t j=0; j<jmax; j++) {
	ptr[j]=vec[i]; i++;
      } 
    }
    return;
  }
  /*! 
    \brief Copy operation: Fills the SegDBInterface object content from another SegDBInterface \b a 

    The number of items copied is equal to min( a.Size() , this->Size() )
  */
  virtual void CopyFrom(SegDBInterface<T> const & a)  
  {
    size_t maxcnt = ( (Size() < a.Size()) ? Size() : a.Size() );
    if (maxcnt < 1)  return;
    size_t i = 0;
    size_t ksa=0, ksega=0, ioffa=0;
    size_t segsza=a.SegmentSize();
    T const * ptra = a.GetCstSegment(ksega);
    for(size_t k=0; k<NbSegments(); k++) {
      if (i >= maxcnt)  break;
      T * ptr = GetSegment(k);
      size_t jmax=SegmentSize();
      if (i+jmax > maxcnt)  jmax=maxcnt-i; 
      for(size_t j=0; j<jmax; j++) {
	ksa = i/segsza;  ioffa = i%segsza;
	if (ksa != ksega) { ptra = a.GetCstSegment(ksa);  ksega=ksa; }
	ptr[j]=ptra[ioffa];  i++;
      } 
    }
    return;
  }

public:
  typedef SegIterator< SegDBInterface<T> ,T > iterator;
  
  //a voir les version const???
  iterator begin() { return iterator(*this); }
  iterator end() { return iterator(*this,this->Size());}

  T&      front()       { return *(this->begin()); } //a voir
  //  T&      front() const { return *(this->begin()); } //a voir
  T&      back()       {return *(this->begin()+(Size()-1));} //a voir
  //T&      back() const {return *(this->begin()+(Size()-1));} //a voir

};//SegDBInterface



// classe de container avec partage de reference
template <class T>
class SegDataBlock : public SegDBInterface<T> {

public:
  //! Default constructor - optional specification of segment size and number of segments
  explicit SegDataBlock(size_t segsz=32, size_t nbseg=0, size_t nitem=1)
  {
    mSRef = NULL;
    SetSize(segsz, nbseg, nitem);
  }
  //! copy constructor - shares the data
  SegDataBlock(const SegDataBlock<T>& a)
  {
    a.mSRef->gThsop.lock();   //  (ThreadSafe) - Start of atomic operation 
    mSRef = a.mSRef;
    mSRef->nref++;
    a.mSRef->gThsop.unlock();   //  (ThreadSafe) - End of atomic operation
  }
  //! copy constructor with specification of flags for data sharing and element value copy
  SegDataBlock(const SegDataBlock<T>& a, bool share, bool cpval=true)
  {
    if (share) {
      a.mSRef->gThsop.lock();   //  (ThreadSafe) - Start of atomic operation 
      mSRef = a.mSRef;
      mSRef->nref++;
      a.mSRef->gThsop.unlock();   //  (ThreadSafe) - End of atomic operation
    }
    else {
      mSRef = NULL;
      Clone(a, cpval);
    }
  }
  //! copy constructor - shares the data if \b is a SegDataBlock, clones otherwise
  SegDataBlock(const SegDBInterface<T>& a)
  {
    SegDataBlock<T> * sdb = dynamic_cast< SegDataBlock<T> *>(&a);
    if (sdb != NULL) { 
      sdb->mSRef->gThsop.lock();   //  (ThreadSafe) - Start of atomic operation 
      mSRef = sdb->mSRef;
      mSRef->nref++;
      sdb->mSRefg->gThsop.unlock();   //  (ThreadSafe) - End of atomic operation
    }
    else Clone(a, true);
  }
  //! Destructor. The memory is freed when the last object referencing the data segment is destroyed
  virtual ~SegDataBlock()
  {
    //DBG    cout << " DEBUG-~SegDataBlock() " << hex << mSRef << dec << " NRef()= " << NRef() << endl;
    Delete();
  }


  //! Adds one segment to the data structure - returns the number of allocated segments.
  virtual size_t Extend()
  {
    T * p = new T[mSRef->segsize];
    mSRef->gThsop.lock();   //  (ThreadSafe) - Start of atomic operation 
    mSRef->dseg.push_back(p);
    size_t rs = mSRef->dseg.size();
    mSRef->gThsop.unlock();   //  (ThreadSafe) - End of atomic operation
    return( rs );
  }

  //! Changes the data segment size and reallocates the memory segments
  // segsz : Segment size ;  nbseg : Number of data segments
  virtual void SetSize(size_t segsz, size_t nbseg=0, size_t nitem=1)
  {
    if (segsz%nitem != 0) 
      throw ParmError("SegDataBlock<T>::SetSize() - segsz not a multiple of nitem");
    Delete();
    mSRef = new SDREF;
    mSRef->gThsop.lock();   //  (ThreadSafe) - Start of atomic operation 
    mSRef->nref = 1;
    mSRef->segsize = segsz;
    mSRef->nbitems = nitem;
    mSRef->unit_p = NULL;
    mSRef->dsid = AnyDataObj::getUniqueId();
    for(size_t k=0; k<nbseg; k++) Extend_P();
    mSRef->gThsop.unlock();   //  (ThreadSafe) - End of atomic operation
  }
//! Shares the data between two SegDataBlock objects
  void Share(const SegDataBlock<T>& a)
  {
    Delete();
    a.mSRef->gThsop.lock();   //  (ThreadSafe) - Start of atomic operation 
    mSRef = a.mSRef;
    mSRef->nref++;
    a.mSRef->gThsop.unlock();   //  (ThreadSafe) - End of atomic operation
  }

//! Makes a clone of the data structure and optionaly copie the data 
  void Clone(const SegDBInterface<T> & a, bool cpval=true)
  {
    Delete();
    mSRef = new SDREF;
    mSRef->gThsop.lock();   //  (ThreadSafe) - Start of atomic operation 
    mSRef->nref = 1;
    mSRef->segsize = a.SegmentSize();
    mSRef->nbitems = a.NbItems();
    bool fgud;
    Units un=a.GetUnits(fgud);
    if (fgud) mSRef->unit_p = new Units(un);
    else mSRef->unit_p = NULL;
    mSRef->gThsop.unlock();   //  (ThreadSafe) - End of atomic operation
    for(size_t k=0; k<a.NbSegments(); k++) {
      Extend();
      if (cpval) {
	T * dst = GetSegment(k);
	const T * src = a.GetCstSegment(k);
	PPF_TPointer_IO<T>::Copy(dst, src, mSRef->segsize);
      }
    }   
  }


  //! Return the segment size of the data structure
  virtual size_t SegmentSize() const  { return mSRef->segsize; }
  //! Return the number of data segments
  virtual size_t NbSegments() const  { return mSRef->dseg.size(); }
  //! Return the number of data items in each group (def=1). SegmentSize() is a multiple of NbItems().
  virtual size_t NbItems() const  { return mSRef->nbitems; }
  //! Return the current size of the segmented data structure
  inline size_t Size() const    { return mSRef->segsize*mSRef->dseg.size(); }

  //! Return the pointer to data segment \b k      
  virtual T* GetSegment(size_t k)              { return mSRef->dseg[k]; } 
  //! Return the const (read-only) pointer to data segment \b k      
  virtual T const * GetCstSegment(size_t k) const { return mSRef->dseg[k]; } 

  //! Define or change the physical unit of the data 
  virtual void SetUnits(const Units& un)
  {
    if (!mSRef)  return;
    mSRef->gThsop.lock();   //  (ThreadSafe) - Start of atomic operation 
    if (mSRef->unit_p) *(mSRef->unit_p)=un; 
    else  mSRef->unit_p=new Units(un);
    mSRef->gThsop.unlock();   //  (ThreadSafe) - End of atomic operation    
  }
  //! Return the physical unit of the data. Flag sdone is set to true if SetUnits() has been called.
  virtual Units GetUnits(bool& sdone) const
  {
    if (mSRef && mSRef->unit_p)  { sdone=true;  return *(mSRef->unit_p);  }
    sdone=false;
    return Units();
  }
  //! Return the physical unit of the data. 
  virtual Units GetUnits() const
  {
    if (mSRef && mSRef->unit_p)  return *(mSRef->unit_p);
    else return Units();
  }

  //! Return the segment index for  element \b i
  //JEC clang++ needs const version
  inline size_t SegIndex(size_t i) {  return i/mSRef->segsize; }

  inline size_t SegIndex(size_t i) const {  return i/mSRef->segsize; }


  //! Return the offset (in data segment)  for  element \b i
  //JEC clang++ needs const version
  inline size_t EltOffset(size_t i) {  return i%mSRef->segsize; }
  inline size_t EltOffset(size_t i) const {  return i%mSRef->segsize; }

  //! Return the \b i th element of the segmented data structure   
  inline T& operator()(size_t i)       { return *(mSRef->dseg[SegIndex(i)]+EltOffset(i));}
  //! Return the \b i th element of  the data structure
  inline T const& operator()(size_t i) const { return *(mSRef->dseg[SegIndex(i)]+EltOffset(i));}
  //! Return the \b i th element of the segmented data structure   
  inline T& operator[](size_t i)       { return *(mSRef->dseg[SegIndex(i)]+EltOffset(i));}
  //! Return the \b i th element of  the data structure
  inline T const& operator[](size_t i) const { return *(mSRef->dseg[SegIndex(i)]+EltOffset(i));}


  //! Return the number of references to the data structure
  inline size_t NRef() const { return mSRef->nref; }

  //! Equal operator. Set all element values to \b v 
  SegDataBlock<T>& operator = (T const & v)
  {
    for(size_t k=0; k<NbSegments(); k++) {
      T * p = mSRef->dseg[k];
      for(size_t j=0; j<SegmentSize(); j++) p[j] = v;
    }
    return (*this);
  }
  //! Equal operator. Clones and copie values from \b a
  inline SegDataBlock<T>& operator = (const SegDBInterface<T> & a)
  {
    Clone(a, true);
    return (*this);
  }
  //! Equal operator. Clones and copie values from \b a
  inline SegDataBlock<T>operator = (const SegDataBlock<T> & a)
  {
    Clone(a, true);
    return (*this);
  }

  //! ASCII formatted output (print)
  std::ostream& Print(std::ostream& os, int lev=0, const char * sep=NULL) const
  {
    os << "SegDataBlock< " << typeid(T).name() << "> mSRef= " << std::hex << mSRef
       << " NRef=" << std::dec << NRef() << " DSId= " <<  DRefId() << std::endl;
    os << " ... SegSize= " << SegmentSize() << " NbSeg= " << NbSegments() 
       << " NbItems=" << NbItems() << " Size= " << Size();
    bool fgud;
    Units un=GetUnits(fgud);
    if (fgud) os << " Units: " << un.Name() << std::endl;
    else os << std::endl;
    if (sep == NULL) sep = "  ";
    if (lev > 0) {  
      for(size_t k=0; k<NbSegments(); k++) {
	T * p = mSRef->dseg[k];
	os << " ..... DataSeg[ " << k << " ] : " << std::hex << p << std::dec << std::endl;
	if (lev > 1) 
	  for(size_t j=0; j<SegmentSize(); j++)  os << p[j] << sep;
	os << std::endl;     
      }
    }
    return os;
  }
  //! ASCII formatted output (print) on cout
  inline std::ostream& Print(int lev=0, const char * sep=NULL) const
  {
    return Print(std::cout, lev, sep);
  }

  //! Returns the unique object identifier 
  inline uint_8 DRefId() const { return ((mSRef)?mSRef->dsid:0); }

protected:
  //! NON-thread safe: Decrement the number of reference counts, and free the memory if NRef=0
  void Delete() 
  {
    if (mSRef == NULL) return;
    mSRef->gThsop.lock();
    mSRef->nref--;
    if (mSRef->nref > 0)  { 
     mSRef->gThsop.unlock();
     mSRef = NULL; 
      return; 
    }
    //DBG    cout << " DEBUG-SegDataBlock::Delete() NbSegments() = " << dec << NbSegments() << endl;
    for(size_t k=0; k<NbSegments(); k++) {
      //DBG  cout << " DEBUG-SegDataBlock::Delete() Segment["<< k <<"] = " << hex << mSRef->dseg[k] << endl;
      delete[] mSRef->dseg[k];
      mSRef->dseg[k] = NULL;
    }
    if (mSRef->unit_p)  { 
      //DBG cout << " DEBUG-SegDataBlock::Delete() unit_p= " << hex << mSRef->unit_p << endl;
      delete mSRef->unit_p;      

    }
    mSRef->gThsop.unlock();
    delete mSRef;
    mSRef = NULL;
  }
  //! NON-thread safe, version of Extend() : Adds one segment to the data structure 
  size_t Extend_P()
  {
    T * p = new T[mSRef->segsize];
    mSRef->dseg.push_back(p);
    return( mSRef->dseg.size());
  }

  /*! \cond 
    SDREF structure for reference management - for internal use by SegDataBlock
  */
  typedef struct {
    size_t nref;      // Number of references to the data structure
    uint_8 dsid;      // Data structure Id - Used by FIO_SegDataBlock
    std::vector<T *> dseg;
    size_t segsize;   // data segment size - is a multiple of nbitems 
    size_t nbitems;   // number of data items in each group
    Units* unit_p;    // Optional physical unit of the data 
    ThSafeOp gThsop;  // Mutex for thread safe operation 
  } SDREF;
  /*! \endcond */
  SDREF *  mSRef;    //!< SDREF structure for reference sharing

};


//! Definition of operator \<\< for ascii formatted output of SegDataBlock
template<class T> 
inline std::ostream& operator << (std::ostream& os, const SegDataBlock<T>& a)
  {  return a.Print(os); }

} // Fin du namespace

#endif
