#ifndef _CODE128_H
#define _CODE128_h

#include <string>
#include <vector>
#include <stdint.h>

namespace code128{
  const uint8_t stopChar = 106;
  const uint8_t startCharA = 103;
  const uint8_t startCharB = 104;
  const uint8_t startCharC = 105;
  const uint8_t changeCharC = 99;
  
  std::string getBarCode(uint8_t number);
  std::string getBarCode(const std::vector<uint8_t>& codeValues);
  
  uint8_t lookup(const std::string& barcode);
  
  void readComplete(std::string barcode);
  
  uint8_t getValueTypeB(unsigned char inChar);
  uint8_t getValueTypeC(uint8_t inNum);
  
  uint8_t calculateChecksum(const std::vector<uint8_t>& codeValues);
  
  
}; // namespace







#endif // _CODE128_H
