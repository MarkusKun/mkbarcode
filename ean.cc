#include <map>
#include <string>

#include <stdint.h>

#include "ean.h"
#include "barstrings.h"

std::string ean::getRightHand(uint8_t number){
  switch (number){
    case 0:  return "1110010";
    case 1:  return "1100110";
    case 2:  return "1101100";
    case 3:  return "1000010";
    case 4:  return "1011100";
    case 5:  return "1001110";
    case 6:  return "1010000";
    case 7:  return "1000100";
    case 8:  return "1001000";
    case 9:  return "1110100";
    default: return "1111111 ERROR";
  }
}

std::string ean::getLeftHandOrder(uint8_t number){
  switch (number){
    case 0:  return "OOOOOO";
    case 1:  return "OOEOEE";
    case 2:  return "OOEEOE";
    case 3:  return "OOEEEO";
    case 4:  return "OEOOEE";
    case 5:  return "OEEOOE";
    case 6:  return "OEEEOO";
    case 7:  return "OEOEOE";
    case 8:  return "OEOEEO";
    case 9:  return "OEEOEO";
    default: return "ERRORE";
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

uint8_t ean::lookupLeftHandOrder(const std::string& givenOrder){
  static std::map <std::string, uint8_t > orderMap;
  if (orderMap.empty()){
    uint8_t curNum;
    for (curNum=0; curNum<10; curNum++){
      orderMap[getLeftHandOrder(curNum)]=curNum;
    } // fill map
  } // fill map
  std::map<std::string,uint8_t>::const_iterator mapFinder;
  mapFinder = orderMap.find(givenOrder);
  if (orderMap.end() != mapFinder){
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

std::vector<ean::codeReturn> ean::getTypes(const std::string& barcode){
  std::vector<ean::codeReturn> returnValues;
  unsigned int digitIterator;
  for (
    digitIterator = 0;
    digitIterator < barcode.size();
    digitIterator+= 7
    )
  {
    std::string currentDigit = barcode.substr(digitIterator,7);
    returnValues.push_back(ean::getType(currentDigit));
  }
  return returnValues;
}
  
std::string ean::calculateChecksum(const std::string& numbers){
  /*
   * note: we assume to have 7 or 12 digits. deletion of existing checksum
   * should be done previously
   */
  std::string::const_reverse_iterator stringIterator;
  unsigned int currentSum=0;
  unsigned int currentMul=3; // start with 3 on the right
  for (
    stringIterator  = numbers.rbegin();
    stringIterator != numbers.rend();
    stringIterator++
    )
  {
    unsigned int currentDigit = (*stringIterator)-'0';
    currentSum += currentDigit*currentMul;
    currentMul = 4-currentMul; // 1 becomes 3, 3 becomes 1
  }
  uint8_t calcSum = (10-currentSum%10)%10;
  return std::string(1,calcSum+'0'); // create string, one iteration of this char
}  

bool ean::checkChecksum(
  const std::string& givenNumber,
  std::string& readChecksum,
  std::string& calcChecksum
  )
{
  { // calculate and check checksum
    calcChecksum = ean::calculateChecksum(givenNumber.substr(0,givenNumber.size()-1));
    readChecksum = givenNumber.substr(givenNumber.size()-1,1); 
    if (readChecksum != calcChecksum){
      return false;
    }else{
      return true;
    }
  }
}
  
  
  
  


