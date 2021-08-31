//------------------------------------------------------------------------------------
// SOPHYA class library - (C)  UPS+LAL IN2P3/CNRS  , CEA-Irfu 
// Ancillary classes for  Image<T> class - class ImgPhotometricBand
// Jan 2015 - R. Ansari, J.E.Campagne, C. Magneville
//------------------------------------------------------------------------------------

#ifndef IMGPHOTBAND_H_SEEN
#define IMGPHOTBAND_H_SEEN

#include "sunitpcst.h"
#include "objfio.h"

#include <string>

namespace SOPHYA {

//--------------------------------------------------------------------------------   
//-------------------------  ImgPhotometricBand class ----------------------------
//--------------------------------------------------------------------------------   

//! Class representing the photometric band of an image 
class ImgPhotometricBand : public AnyDataObj {
public:
  // ===> DO NOT CHANGE EXISTING enum type VALUES - should be < 65535 
  enum StandardPhotometricFilterId {NonStandardFilter=0, U_Filter=1, B_Filter=2, V_Filter=3, R_Filter=4, I_Filter=5 };
  enum PassBandType {kSquarePassBand=0, kGaussianPassBand=1}; 

  //! Constructor - definition of photometric band through one of the standard filters 
  ImgPhotometricBand(StandardPhotometricFilterId fid=ImgPhotometricBand::NonStandardFilter);
  ImgPhotometricBand(PhysQty const& lambda_nu_0, double relbandw, double mxtrans=1., 
		     PassBandType typ=ImgPhotometricBand::kSquarePassBand);
  //! Copy constructor 
  ImgPhotometricBand(ImgPhotometricBand const& a)
  { CopyFrom(a); }

  //! Copy operator 
  ImgPhotometricBand& operator = (ImgPhotometricBand const& a)
  { CopyFrom(a);  return (*this); }

  //  virtual ~ImgPhotometricBand();

  //! return the filter identification 
  inline StandardPhotometricFilterId getFilterId() const { return stdfilterid_; }
  //! return the central wavelength (lambda_0) of the photometric band-pass filter 
  inline PhysQty getCentralWavelength() const { return lambda0_; }
  //! return the central frequency (nu_0) of the photometric band-pass filter 
  inline PhysQty getCentralFreqquency() const { return PhysQty::SpeedofLight()/lambda0_; }
  //! return the maximum transmission at lambda_0 / nu_0 of the band-pass filter 
  inline double  getMaxTransmission() const  { return maxtrans_; }
  //! return the relative bandwidth Delta lambda / lambda_0 
  inline double  getRelativeBandWidth() const { return relbandwidth_; }
  //! return the relative bandwidth Delta lambda / lambda_0 
  inline PassBandType getPassBandType() const { return pbtype_; }
  //! return the filter identification as a string 
  std::string getFilterName() const;

  //! Prints the object on \b cout (return the cout stream object)
  inline std::ostream& Print()  const  
  { return Print(std::cout); }  
  //! Prints the object on stream \b os (return the os stream object)
  virtual std::ostream& Print(std::ostream& os)  const;

  //! copy function 
  inline void CopyFrom(ImgPhotometricBand const& a)
  { 
    stdfilterid_=a.stdfilterid_; lambda0_=a.lambda0_;  relbandwidth_=a.relbandwidth_;  
    maxtrans_=a.maxtrans_; pbtype_=a.pbtype_; 
  }

  friend class ObjFileIO<ImgPhotometricBand>;
protected:
  inline void defineFilter(StandardPhotometricFilterId fid, PhysQty const& lambda0, double relbw, double maxtr, PassBandType pbt)
  { stdfilterid_=fid; lambda0_=lambda0;  relbandwidth_=relbw;  maxtrans_=maxtr;  pbtype_=pbt; return; }

  ImgPhotometricBand::StandardPhotometricFilterId  stdfilterid_;    //!< Standard photometric filter identification  
  PhysQty lambda0_;      //!<  Central wavelength 
  r_8 relbandwidth_;  //!<  relative bandwidth   delta_lambda / lambda_0
  r_8 maxtrans_;      //!<  maximum transmission at lambda = lambda_0 
  ImgPhotometricBand::PassBandType pbtype_;   //!<  pass-band filter type 
};

//! operator << overloading - Prints the ImgPhotometricBand object on the stream \b os
inline std::ostream& operator << (std::ostream& s, ImgPhotometricBand const & pb)
{  pb.Print(s);  return(s);  }

//! Writes the ImgPhotometricBand object in the POutPersist stream \b os
inline POutPersist& operator << (POutPersist& os, ImgPhotometricBand & obj)
{ ObjFileIO<ImgPhotometricBand> fio(&obj);  fio.Write(os);  return(os); }
//! Reads the ImgPhotometricBand object from the PInPersist stream \b is 
inline PInPersist& operator >> (PInPersist& is, ImgPhotometricBand & obj)
{ ObjFileIO<ImgPhotometricBand> fio(&obj); is.SkipToNextObject(); fio.Read(is); return(is); }

}  // End of namespace SOPHYA

#endif
