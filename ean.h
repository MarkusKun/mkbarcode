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
  
  
  std::string getRightHand(uint8_t number);
  std::string getLeftHandMirror(uint8_t number); // also "even"
  std::string getLeftHandInvert(uint8_t number); // also "odd"
  
  uint8_t lookupRightHand(const std::string& barcode);
  
  std::string mirror(const std::string& barcode);
  std::string invert(std::string barcode);
  
  struct codeReturn{
    codeType_t  codeType;
    uint8_t     codeValue;
  };
  
  codeReturn getType(const std::string& barcode);
}; // namespace



#endif // _EAN_H
