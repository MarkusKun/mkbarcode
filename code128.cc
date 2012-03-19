#include <map>
#include <string>
#include <iostream>
#include <sstream>

#include "code128.h"
#include "barstrings.h"

std::string code128::getBarCode(uint8_t number){
  switch (number){
  case 0  : return "11011001100"; case 54 : return "11101011000";
  case 1  : return "11001101100"; case 55 : return "11101000110"; 
  case 2  : return "11001100110"; case 56 : return "11100010110";
  case 3  : return "10010011000"; case 57 : return "11101101000";
  case 4  : return "10010001100"; case 58 : return "11101100010";
  case 5  : return "10001001100"; case 59 : return "11100011010";
  case 6  : return "10011001000"; case 60 : return "11101111010";
  case 7  : return "10011000100"; case 61 : return "11001000010";
  case 8  : return "10001100100"; case 62 : return "11110001010";
  case 9  : return "11001001000"; case 63 : return "10100110000";
  case 10 : return "11001000100"; case 64 : return "10100001100";
  case 11 : return "11001000100"; case 65 : return "10010110000";
  case 12 : return "10110011100"; case 66 : return "10010000110";
  case 13 : return "10011011100"; case 67 : return "10000101100";
  case 14 : return "10011001110"; case 68 : return "10000100110";
  case 15 : return "10111001100"; case 69 : return "10110010000";
  case 16 : return "10011101100"; case 70 : return "10110000100";
  case 17 : return "10011100110"; case 71 : return "10011010000";
  case 18 : return "11001110010"; case 72 : return "10011000010";
  case 19 : return "11001011100"; case 73 : return "10000110100";
  case 20 : return "11001001110"; case 74 : return "10000110010";
  case 21 : return "11011100100"; case 75 : return "11000010010";
  case 22 : return "11001110100"; case 76 : return "11001010000";
  case 23 : return "11101101110"; case 77 : return "11110111010";
  case 24 : return "11101001100"; case 78 : return "11000010100";
  case 25 : return "11100101100"; case 79 : return "10001111010";
  case 26 : return "11100100110"; case 80 : return "10100111100";
  case 27 : return "11101100100"; case 81 : return "10010111100";
  case 28 : return "11100110100"; case 82 : return "10010011110";
  case 29 : return "11100110010"; case 83 : return "10111100100";
  case 30 : return "11011011000"; case 84 : return "10011110100";
  case 31 : return "11011000110"; case 85 : return "10011110010";
  case 32 : return "11000110110"; case 86 : return "11110100100";
  case 33 : return "10100011000"; case 87 : return "11110010100";
  case 34 : return "10001011000"; case 88 : return "11110010010";
  case 35 : return "10001000110"; case 89 : return "11011011110";
  case 36 : return "10110001000"; case 90 : return "11011110110";
  case 37 : return "10001101000"; case 91 : return "11110110110";
  case 38 : return "10001100010"; case 92 : return "10101111000";
  case 39 : return "11010001000"; case 93 : return "10100011110";
  case 40 : return "11000101000"; case 94 : return "10001011110";
  case 41 : return "11000100010"; case 95 : return "10111101000";
  case 42 : return "10110111000"; case 96 : return "10111100010";
  case 43 : return "10110001110"; case 97 : return "11110101000";
  case 44 : return "10001101110"; case 98 : return "11110100010";
  case 45 : return "10111011000"; case 99 : return "10111011110";
  case 46 : return "10111000110"; case 100: return "10111101110";
  case 47 : return "10001110110"; case 101: return "11101011110";
  case 48 : return "11101110110"; case 102: return "11110101110";
  case 49 : return "11010001110"; case 103: return "11010000100";
  case 50 : return "11000101110"; case 104: return "11010010000";
  case 51 : return "11011101000"; case 105: return "11010011100";
  case 52 : return "11011100010"; case 106: return "1100011101011";
  case 53 : return "11011101110";         
  default: return "1010101010101 ERROR!";
  } // switch
}

uint8_t code128::lookup(const std::string& barcode){
  static std::map< std::string , uint8_t > typeMap;
  if (typeMap.empty()){
    uint8_t curNum;
    for (curNum=0; curNum<=106; curNum++){
      typeMap[getBarCode(curNum)]=curNum;
    } // fill map
  } // fill map
  std::map<std::string,uint8_t>::const_iterator mapFinder;
  mapFinder = typeMap.find(barcode);
  if (typeMap.end() != mapFinder){
    return mapFinder->second;
  }else{
    return 255; // error
  }
}

void code128::readComplete(std::string barcode){
  uint8_t table=0;
  
  // every char has 11 "modules";
  { // trying to find start
    uint8_t barValue;
    { // first check: mirror?
      barValue = code128::lookup(mirror(barcode.substr(0,13)));
      if (stopChar == barValue){
        std::cout << "First char = mirrored stopchar - are you reading upside down?" << std::endl;
        readComplete(mirror(barcode));
        return;
      }
    }
    { // second check: valid start?
      barValue = code128::lookup(barcode.substr(0,11));
      if (barValue > 106){
        std::cout << "First char unknown"<< std::endl;
        return;
      }
      if (startCharA == barValue){
        std::cout << "Starting in Table A" << std::endl;
        table=1;
      }
      else if (startCharB == barValue){
        std::cout << "Starting in Table B" << std::endl;
        table=2;
      }
      else if (startCharC == barValue){
        std::cout << "Starting in Table C" << std::endl;
        table=3;
      }
      else{
        std::cout << "First char valid, but not start (";
        std::cout << std::dec << (int)barValue << ")" << std::endl;
        return;
      }
    }
  } // checking for start
  while(true){
    uint8_t barValue;
    std::string subCode = barcode.substr(0,11);
    
    std::cout << subCode << " "; 
    barValue = code128::lookup(subCode);
    if (106 < barValue){
      std::cout << "?" ;
      subCode = barcode.substr(0,13);
      std::cout << subCode;
      if (stopChar == lookup(subCode)){ // if stop
        std::cout << " Valid Stop!" << std::endl;
      }else{
        std::cout << " invalid sign!" << std::endl;
      }
      break;
    }
    barcode = barcode.substr(11);
    std::cout << std::dec << (int) barValue << std::endl;
  } // while true.
}
     

uint8_t code128::getValueTypeB(unsigned char inChar){
  uint8_t barValue=0;
  if( 
    ('A'<= inChar) &&
    (inChar <= 'Z')
    ) // capital char
  {
    barValue = inChar - 'A' + 33;
    //cout << "Letter " << *stringIterator <<  " : " << (int) barValue << endl;
  }
  if (
    ('0'<= inChar) &&
    (inChar <= '9')
    ) // number
  {
    barValue = inChar - '0' + 16;
    //cout << "Number " << *stringIterator <<  " : " << (int) barValue << endl;
  }
  return barValue;
}
uint8_t code128::getValueTypeC(uint8_t inNum){
  if (inNum < 100){
    return inNum;
  }else{
    return 0;
  }
}

uint8_t code128::calculateChecksum(const std::vector<uint8_t>& codeValues){
  if (codeValues.size()<2){
    return 0;
  }
  unsigned int currentSum=codeValues[0];
  std::vector<uint8_t>::const_iterator vecIterator;
  unsigned int currentMul;
  for (
    vecIterator  = codeValues.begin(), currentMul=0;
    vecIterator != codeValues.end();
    vecIterator++,currentMul++
    )
  {
    currentSum += currentMul * (*vecIterator);
    currentSum = currentSum % 103;
  }
  return currentSum;
}

std::string code128::getBarCode(const std::vector<uint8_t>& codeValues){
  std::stringstream returnStream;
  std::vector<uint8_t>::const_iterator vecIterator;
  for (
    vecIterator  = codeValues.begin();
    vecIterator != codeValues.end();
    vecIterator++
    )
  {
    returnStream << getBarCode(*vecIterator);
  }
  return (returnStream.str());
}


