//------------------------------------------------------------------------------------
// SOPHYA class library - (C)  UPS+LAL IN2P3/CNRS  , CEA-Irfu 
// Ancillary classes for  Image<T> class - class ImgSensorResponse
// Jan 2015 - R. Ansari, J.E.Campagne, C. Magneville
//------------------------------------------------------------------------------------

#ifndef IMGSENSRESP_H_SEEN
#define IMGSENSRESP_H_SEEN

#include "imgphotband.h"


namespace SOPHYA {

//--------------------------------------------------------------------------------   
//-------------------------  ImgSensorResponse class ----------------------------
//--------------------------------------------------------------------------------   

//! Class representing the response of an image sensor 
class ImgSensorResponse : public ImgPhotometricBand {
public:
  //! Default constructor 
  ImgSensorResponse();
  //! Constructor - with definition of parameters
  ImgSensorResponse(double minADU, double maxADU, double offset, double gain, double ronoise);
  //! Copy constructor 
  ImgSensorResponse(ImgSensorResponse const& a)
  { CopyFrom(a);  }

  //! Copy operator 
  ImgSensorResponse& operator = (ImgSensorResponse const& a)
  { CopyFrom(a);  return (*this); }

  //! define the sensor dynamic range in ADU (ADC units) 
  inline void setMinMaxADU(double minADU, double maxADU) 
  { minADU_=minADU; maxADU_=maxADU; }
  //! define the sensor output offset , in ADU (ADC units)
  inline void setOffset(double offset)  { offset_=offset; } 
  //! define the sensor amplifier gain in ADU/photo-electron and read-out noise in photo-electrons
  inline void setGainRONoise(double gain, double ronoise) 
  { gain_=gain; ronoise_=ronoise; }
  //! define the sensor quantum efficiency QE = <N_photo-electrons>/<N_photons>  
  inline void setQuantumEfficiency(double qe)  { qe_=qe; }

  //! return the minimum allowed image sensor output value, in ADU 
  inline double getMinADU() const { return minADU_; }
  //! return the maximum allowed image sensor output value, in ADU 
  inline double getMaxADU() const { return maxADU_; }
  //! return the  sensor output offset, in ADU 
  inline double getOffset() const { return offset_; }
  //! return the sensor gain, in ADU/photo-electron 
  inline double getGain() const { return gain_; }
  //! return the sensor read-out noise, in photo-electron 
  inline double getRONoise() const { return ronoise_; }
  //! return the sensor quantum efficiency QE = <N_photo-electrons>/<N_photons> 
  inline double getQuantumEfficiency() const { return ronoise_; }

  //! convert the ADU value to photo-electron number 
  inline double convertToPhotoElectrons(double adu) const  { return (adu-offset_)/gain_; } 
  //! convert the ADU value to photon number 
  inline double convertToPhotons(double adu) const  { return (adu-offset_)/gain_/qe_; } 
  //! compute noise expectation value in ADU 
  inline double computeNoiseADU(double adu)  const 
  { return sqrt(((adu>offset_)?(adu-offset_)/gain_:0.)+ronoise_*ronoise_)*gain_; }
  //! compute noise expectation value in photo-electrons 
  inline double computeNoisePE(double adu)  const 
  { return sqrt(((adu>offset_)?(adu-offset_)/gain_:0.)+ronoise_*ronoise_); }

  //! Prints the object on \b cout (return the cout stream object)
  inline std::ostream& Print()  const  
  { return Print(std::cout); }  
  //! Prints the object on stream \b os (return the os stream object)
  virtual std::ostream& Print(std::ostream& os)  const;

  //! copy function 
  void CopyFrom(ImgSensorResponse const& a)
  {
    ImgPhotometricBand::CopyFrom(a);
    minADU_=a.minADU_;  maxADU_=a.maxADU_; 
    offset_=a.offset_;  
    gain_=a.gain_;  ronoise_=a.ronoise_; 
    qe_=a.qe_; 
  }

  friend class ObjFileIO<ImgSensorResponse>;
protected:
  r_8 minADU_, maxADU_;      //!< Sensor dynamic range, minimum and maximum values in ADU (ADC units)  
  r_8 offset_;               //!< Sensor output offset in ADU <output> for 0 photo-electrons
  r_8 gain_,ronoise_;        //!< Sensor output gain , ADU/photo-electrons and read-out noise, in photo-electrons 
  r_8 qe_;                   //!< Sensor quantum efficiency, in the photometric band  
};

//! operator << overloading - Prints the ImgPhotometricBand object on the stream \b os
inline std::ostream& operator << (std::ostream& s, ImgSensorResponse const & sr)
{  sr.Print(s);  return(s);  }

//--------------------------------------------------------------------------------   
//------  PPF handler:  ObjFileIO<ImgSensorResponse> ----------------------------
//--------------------------------------------------------------------------------   

//! Writes the ImgSensorResponse object in the POutPersist stream \b os
inline POutPersist& operator << (POutPersist& os, ImgSensorResponse & obj)
{ ObjFileIO<ImgSensorResponse> fio(&obj);  fio.Write(os);  return(os); }
//! Reads the ImgSensorResponse object from the PInPersist stream \b is 
inline PInPersist& operator >> (PInPersist& is, ImgSensorResponse & obj)
{ ObjFileIO<ImgSensorResponse> fio(&obj); is.SkipToNextObject(); fio.Read(is); return(is); }

}  // End of namespace SOPHYA

#endif
