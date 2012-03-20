#ifndef _EAN_H
#define _EAN_H

#include <string>
#include <stdint.h>

namespace ean{
  enum codeType_t{
    CODETYPE_UNKNOWN,
    CODETYPE_RIGHT,
    CODETYPE_MIRROR,
    CODETYPE_INVERSE
  };
  
  /*!
   * \brief return "right hand"-barcode for given number (0-9)
   *
   * this function returns the "right-hand"-representation of the barcode
   * for the given number. It is called "right hand" as it is the 
   * later part of the barcode.
   */
  std::string getRightHand(uint8_t number);
  std::string getLeftHandMirror(uint8_t number); // also "even"
  std::string getLeftHandInvert(uint8_t number); // also "odd"
  
  uint8_t lookupRightHand(const std::string& barcode);
  
  
  struct codeReturn{
    codeType_t  codeType;
    uint8_t     codeValue;
  };
  
  codeReturn getType(const std::string& barcode);
}; // namespace



#endif // _EAN_H
