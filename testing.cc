#include <iostream>
#include <sstream>

#include <vector>

#include <stdint.h>
#include <cstdlib>

#include "debugout.h"
#include "pixelbild.h"
#include "bmpreader.h"

#include "barstrings.h"
#include "ean.h"
#include "code128.h"

int main(int argc, char* argv[]){
  using std::cout; using std::endl;
  dout.loadConfigFile("debug.config");
  
  dout.startScope("main");
  
  if (argc<3){
    std::cerr << "Error on parameter parsing" << std::endl;
    std::cerr << "Try 'programname --help' for more information" << std::endl;
    exit(EXIT_FAILURE);
  }
  std::string programMode = argv[1];
  std::cout << programMode << std::endl;
  
  if ("--test" == programMode){
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
      std::cout << " 128: " << code128::getBarCode(testZiffer);
      std::cout << std::endl;
    }
  }
  if ("--lookupEAN" == programMode){
    std::string testcode = argv[2];
    switch (ean::getType(testcode).codeType){
    case ean::CODETYPE_UNKNOWN: std::cout << "unknown code" << std::endl; break;
    case ean::CODETYPE_RIGHT: std::cout << "Right" << std::endl; break;
    case ean::CODETYPE_MIRROR: std::cout << "Mirror" << std::endl; break;
    case ean::CODETYPE_INVERSE: std::cout << "Inverse" << std::endl; break;
    } // break
  }
  if ("--lookup128" == programMode){
    std::string testcode = argv[2];
    code128::readComplete(testcode);
  }
  if ("--create128" == programMode){
    std::string codeToCreate = argv[2];
    std::vector<uint8_t> codeValues;
    codeValues.push_back(code128::startCharB);
    std::string::const_iterator stringIterator;
    for(
      stringIterator  = codeToCreate.begin();
      stringIterator != codeToCreate.end();
      stringIterator++
      )
    {
      codeValues.push_back(code128::getValueTypeB(*stringIterator));
    }
    codeValues.push_back(code128::calculateChecksum(codeValues));
    codeValues.push_back(code128::stopChar);
    std::vector<uint8_t>::const_iterator vecIterator;
    for(
      vecIterator  = codeValues.begin();
      vecIterator != codeValues.end();
      vecIterator++
      )
    {
      std::cout << " " << (int) (*vecIterator);
    } // for all elements
    std::cout << endl;
  } // if create
  if("--create128K" == programMode){
    using std::dec;
    unsigned int numberToCreate;
    { // convert given number K12345678 
      std::stringstream convertStream;
      convertStream << argv[2];
      convertStream >> dec >> numberToCreate;
    }
    std::cout << "Creating for number " << numberToCreate << std::endl;
    std::vector<uint8_t> codeValues;
    codeValues.push_back(code128::startCharB);
    codeValues.push_back(code128::getValueTypeB('K'));
    codeValues.push_back(code128::changeCharC);
    codeValues.push_back(code128::getValueTypeC(numberToCreate/1000000));
    numberToCreate = numberToCreate % 1000000;
    codeValues.push_back(code128::getValueTypeC(numberToCreate/10000));
    numberToCreate = numberToCreate % 10000;
    codeValues.push_back(code128::getValueTypeC(numberToCreate/100));
    numberToCreate = numberToCreate % 100;
    codeValues.push_back(code128::getValueTypeC(numberToCreate));
    codeValues.push_back(code128::calculateChecksum(codeValues));
    codeValues.push_back(code128::stopChar);
    
    std::vector<uint8_t>::const_iterator vecIterator;
    for(
      vecIterator  = codeValues.begin();
      vecIterator != codeValues.end();
      vecIterator++
      )
    {
      std::cout << " " << (int) (*vecIterator);
    } // for all elements
    std::cout << endl;
    std::string fullBarcode = code128::getBarCode(codeValues);
    std::cout << fullBarcode << endl;
    
    pixelbild meinBild(fullBarcode.size(),1,defaultBGPixel); // default is black
    { // fill bild with pixels from barcode
      pixel whitePixel;
      { 
        whitePixel.red=255;
        whitePixel.green=255;
        whitePixel.blue=255;
      }
      std::string::const_iterator stringIterator;
      unsigned int pixelPos;
      for (
        stringIterator  = fullBarcode.begin(), pixelPos=0;
        stringIterator != fullBarcode.end();
        stringIterator++, pixelPos++
        )
      {
        std::cout << (*stringIterator);
        if ('0' == *stringIterator){ // should be white
          meinBild.setPixel(0,pixelPos,whitePixel);
        }
      }
      std::cout << endl;
    }
    { // then, write to bitmap file
      std::string outFilename;
      { // convert number to filename
        std::ostringstream convertStream;
        convertStream << dec << numberToCreate;
        convertStream << ".code128.bmp";
        outFilename = convertStream.str();
      }
      bmpreader::writeFile(meinBild, outFilename);
    }
  } // if create
  
  
  dout.endScope("main");      
  return 0;
}

