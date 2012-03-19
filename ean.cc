#include <map>
#include <string>

#include <stdint.h>

#include "ean.h"
#include "barstrings.h"

std::string ean::getRightHand(uint8_t number){
  switch (number){
    case 0: return "1110010";
    case 1: return "1100110";
    case 2: return "1101100";
    case 3: return "1000010";
    case 4: return "1011100";
    case 5: return "1001110";
    case 6: return "1010000";
    case 7: return "1000100";
    case 8: return "1001000";
    case 9: return "1110100";
    default: return "1111111 ERROR";
  }
}


std::string ean::getLeftHandInvert(uint8_t number){
  return invert(getRightHand(number));
}

std::string ean::getLeftHandMirror(uint8_t number){
  return mirror(getRightHand(number));
}


uint8_t ean::lookupRightHand(const std::string& barcode){
  static std::map< std::string , uint8_t > typeMap;
  if (typeMap.empty()){
    uint8_t curNum;
    for (curNum=0; curNum<10; curNum++){
      typeMap[getRightHand(curNum)]=curNum;
    } // fill map
  } // fill map
  std::map<std::string,uint8_t>::const_iterator mapFinder;
  mapFinder = typeMap.find(barcode);
  if (typeMap.end() != mapFinder){
    return mapFinder->second;
  }else{
    return 42; // error
  }
}

ean::codeReturn ean::getType(const std::string& barcode){
  using std::map; using std::string;
  codeReturn returnValue;
  { // try right
    returnValue.codeValue = lookupRightHand(barcode);
    if (returnValue.codeValue < 10){
      returnValue.codeType = CODETYPE_RIGHT;
      return returnValue;
    }
  }
  { // try mirror
    returnValue.codeValue = lookupRightHand(mirror(barcode));
    if (returnValue.codeValue < 10){
      returnValue.codeType = CODETYPE_MIRROR;
      return returnValue;
    }
  }
  { // try inverse
    returnValue.codeValue = lookupRightHand(invert(barcode));
    if (returnValue.codeValue < 10){
      returnValue.codeType = CODETYPE_INVERSE;
      return returnValue;
    }
  }
  // unsuccess
  returnValue.codeType = CODETYPE_UNKNOWN;
  return returnValue;
  
  
}

