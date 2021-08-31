// This may look like C code, but it is really -*- C++ -*-
//-----------------------------------------------------------
// Class  FlagVector<N> : Vector of flags (bits)  
// SOPHYA class library - (C)  UPS+LAL IN2P3/CNRS  , CEA-Irfu 
// R. Ansari      UPS+LAL IN2P3/CNRS  2014
//-----------------------------------------------------------

#ifndef FLAGVECTOR_H_SEEN
#define FLAGVECTOR_H_SEEN

#include "machdefs.h"
#include <strings.h> 
#include <iostream> 

/*!
   \class SOPHYA::FlagVector
   \ingroup SUtils 
   class for representing and manipulating a set of flags (true/false values)  
   as a vector of boolean values. Each flag is represented as a single bit. 
   The template parameter N defines the vector size Vsz = 8*N. 
   Equality (==) and not-equal (!=) operators are also provided.

   \sa SOPHYA::SegDataBlock 
   \sa SOPHYA::SwSegDataBlock
*/

namespace SOPHYA {

template <uint_2 N>
class FlagVector {

public:
  //! Default constructor - buffer initialized to null characters.
  explicit FlagVector()
  { bzero(bitbuff_,N); }
  //! Copy constructor
  FlagVector(FlagVector<N> const & flgs)
  { CopyFrom(flgs); }
  //! return the vector size or length (= 8*N ) - same as length()
  inline uint_2 size() const { return 8*N; } 
  //! return the vector size or length (= 8*N ) - alias for size()
  inline uint_2 length() const { return 8*N; } 
  //! return flag/bit \b k as a boolean (true if set) 
  inline bool operator[](unsigned int k) const  { return (bitbuff_[k/8]&FlgPos(k%8)); }
  //! set the flag/bit \b k to 1 (true) 
  inline void set(unsigned int k)   { bitbuff_[k/8] |= FlgPos(k%8);  return; }
  //! clear the flag/bit \b k  (set it to 0 (false)) 
  inline void clear(unsigned int k)   { bitbuff_[k/8] &= (~FlgPos(k%8));  return; }
  //! set/clear the flag/bit \b k as \b flg
  inline void setFlag(unsigned int k, bool flg) 
  {  
    if (flg) set(k); else clear(k); 
  }
  //! set all flags (all bits to 1)
  void setAll() 
  {
    for(int i=0; i<N; i++)  bitbuff_[i]=0xFF;
  }
  //! clear all flags (all bits to 0) 
  void clearAll() 
  {
    for(int i=0; i<N; i++)  bitbuff_[i]=0x00;
  }
  //! Copy operator : *this = flgs 
  FlagVector<N>& operator = (FlagVector<N> const & flgs)
  { CopyFrom(flgs);  return (*this); } 

  //! print the object as a set of 1/0 
  std::ostream&  print(std::ostream& os, bool verbose=false)  const  
  { 
    if (verbose) os << "FlagVector<" << N << "> : ";
    for(int i=0; i<8*N; i++) os << ((*this)[i]?"1":"0");
    if (verbose) os << std::endl;  return os; 
  }
  //! print the object on cout
  inline std::ostream&  print(bool verbose=false)  const  
  { return print(std::cout, verbose); }
  /*! 
    \brief Compare two FlagVector<N>: return true if equal 
  */ 
  bool IsEqual(FlagVector<N> const & flgs) const 
  { for(int i=0; i<N; i++)  if (bitbuff_[i]!=flgs.bitbuff_[i])  return false;  return true; }

  //!  conversion to string, in the form 1100011  
  std::string convertToString()  const 
  { 
    size_t n = size();
    std::string rs(n,'0');  
    for(int i=0; i<n; i++)
      if ((*this)[i])  rs[i]='1';
    return rs;
  }
  //!  conversion of the bit array to long integer
  int_8 convertToInteger()  const 
  { 
    int_8 rl=0;
    uint_1 *cp=(uint_1 *)(&rl);
    int ncopy=(N<=8)?N:8;
    for(int i=0; i<ncopy; i++)  cp[i]=bitbuff_[i];
    return rl;
  }
  //!  decode a string in the form 1100011, and set the flags where s[i]='1' 
  void decodeString(std::string const& s)  
  { 
    clearAll();
    size_t n = s.length();
    if (n>8*N) n = 8*N;
    for(size_t i=0; i<n; i++) 
      if (s[i]=='1')  set(i);
    return;
  }
  //!  set flags based on the long integer bit values (up to a maximum of 64 bits)
  void decodeInteger(int_8 lv)  
  { 
    uint_1 *cp=(uint_1 *)&lv;
    int ncopy=(N<=8)?N:8;
    for(int i=0; i<ncopy; i++)  bitbuff_[i]=cp[i];
    return;
  }
  //! access to the bit array 
  inline uint_1 * bit_array() { return bitbuff_; }
  //! access to the bit array - const version 
  inline const uint_1 * bit_array() const { return bitbuff_; }

protected:
  inline void CopyFrom(FlagVector<N> const & flgs)
  {  for(int i=0; i<N; i++)  bitbuff_[i]=flgs.bitbuff_[i];  }
  static inline uint_1 FlgPos(int i) 
  {
    uint_1 pos_[8]={1,2,4,8,16,32,64,128}; 
    return pos_[i];
  }
  uint_1 bitbuff_[N];
};

/*! operator << overloading - Prints the FlagVector<N> object on the stream \b s as as string of 1,0*/
template <uint_2 N>
std::ostream& operator << (std::ostream& s, FlagVector<N> const & fsl)
{  fsl.print(s); return(s);  }

/*! Equality comparison operator == for FlagVector<N>s : return true if (a == b)  */
template <uint_2 N>
inline bool operator == (FlagVector<N> const& a, FlagVector<N> const& b)
{ return a.IsEqual(b); }

/*! Not equal comparison operator != for FlagVector<N>s : return true if (a != b)  */
template <uint_2 N>
inline bool operator != (FlagVector<N> const& a, FlagVector<N> const& b)
{ return !a.IsEqual(b); }


//! vector of 8 flags (bits) 
typedef FlagVector<1> FlagVector8 ;
//! vector of 16 flags (bits) 
typedef FlagVector<2> FlagVector16 ;
//! vector of 32 flags (bits) 
typedef FlagVector<4> FlagVector32 ;
//! vector of 64 flags (bits) 
typedef FlagVector<8> FlagVector64 ;
//! vector of 128 flags (bits) 
typedef FlagVector<16> FlagVector128 ;


} // Fin namespace

#endif
