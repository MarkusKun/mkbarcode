#ifndef _CODE128_H
#define _CODE128_h

#include <string>
#include <stdint.h>

namespace code128{
  std::string getBarCode(uint8_t number);
  
  uint8_t lookup(const std::string& barcode);
  
  void readComplete(std::string barcode);
  
}; // namespace







#endif // _CODE128_H
