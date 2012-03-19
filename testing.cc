#include <iostream>

#include "stdint.h"

#include "ean.h"

int main(int argc, char* argv[]){
  std::cout << "test" << std::endl;
  
  uint8_t testZiffer;
  for (
    testZiffer=0;
    testZiffer<10;
    testZiffer++
    )
  {
    std::cout << (int) testZiffer << " Right " << ean::getRightHand(testZiffer);
    std::cout << " LeftI " << ean::getLeftHandInvert(testZiffer);
    std::cout << " LeftM " << ean::getLeftHandMirror(testZiffer);
    std::cout << std::endl;
  }
  if (argc>1){
    std::string testcode = argv[1];
    switch (ean::getType(testcode).codeType){
    case ean::CODETYPE_UNKNOWN: std::cout << "unknown code" << std::endl; break;
    case ean::CODETYPE_RIGHT: std::cout << "Right" << std::endl; break;
    case ean::CODETYPE_MIRROR: std::cout << "Mirror" << std::endl; break;
    case ean::CODETYPE_INVERSE: std::cout << "Inverse" << std::endl; break;
    } // break
    
  } // if param
  
  
  return 0;
}

