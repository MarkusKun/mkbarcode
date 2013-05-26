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

#include "mkbarcode.h"

void mkbarcode::printHelp(const std::string& programName){
  using std::cout; using std::endl;
  cout << "Usage: " << programName << " MODE [Parameter]" << endl;
  cout << "  --help                 print this help message" << endl;
  
  cout << "Available Modes:" << endl;
  cout << "  --test                 Print EAN codes for numbers 0-9" << endl;
  cout << "  --lookupEAN BARCODE    lookup a single digit for EAN" << endl;
  cout << "  --lookup128 BARCODE    lookup a complete Code128-barcode" << endl;
  cout << "  --create128B STRING    Convert the given String to Code128 in Code B" << endl;
  cout << "  --create128I           Interactive mode for Code128 creation" << endl;
  cout << "  --create128K NUMBER    Create 'K12345678' (leading 'K', 8 chars) in Code 128." << endl;
  cout << "                         'K' is in CodeB, the numbers in CodeC" << endl;  
  
}


void mkbarcode::printHelpReminder(const std::string& programName){
  using std::cout; using std::endl;
  cout << "Try '" << programName << "  --help' for more information" << endl;
}
  

int main(int argc, char* argv[]){
  using std::cout; using std::endl;
  dout.loadConfigFile("debug.config");
  
  dout.startScope("main");
  
  if (argc<2){ // no parameter / no mode
    std::cerr << "No mode given" << std::endl;
    mkbarcode::printHelpReminder(argv[0]);
    exit(EXIT_FAILURE);
  }
    
  std::string programMode = argv[1];
  dout << programMode << std::endl;
  
  if ("--help" == programMode){
    mkbarcode::printHelp(argv[0]);
    exit(EXIT_SUCCESS);
  }
  
  if ("--test" == programMode){
    uint8_t testZiffer;
    for(
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
    if (argc<3){
      std::cerr << "No barcode given - nothing to lookup" << std::endl;
      mkbarcode::printHelpReminder(argv[0]);
      exit(EXIT_FAILURE);
    }
    std::string testcode = argv[2];
    ean::codeReturn lookupResult = ean::getType(testcode);
    
    switch (lookupResult.codeType){
    case ean::CODETYPE_UNKNOWN: std::cout << "unknown code"; break;
    case ean::CODETYPE_RIGHT:   std::cout << "Right "      ; break;
    case ean::CODETYPE_MIRROR:  std::cout << "Mirrored "   ; break;
    case ean::CODETYPE_INVERSE: std::cout << "Inverse "    ; break;
    } // switch
    if (ean::CODETYPE_UNKNOWN != lookupResult.codeType){
      std::cout << std::dec << (int)lookupResult.codeValue;
    }
    std::cout << std::endl;
    
  }
  if ("--lookup128" == programMode){
    std::string testcode = argv[2];
    if (argc<3){
      std::cerr << "No barcode given - nothing to lookup" << std::endl;
      mkbarcode::printHelpReminder(argv[0]);
      exit(EXIT_FAILURE);
    }
    code128::readComplete(testcode);
  }
  if ("--create128B" == programMode){ // code B
    if (argc<3){
      std::cerr << "No string given - nothing to create" << std::endl;
      mkbarcode::printHelpReminder(argv[0]);
      exit(EXIT_FAILURE);
    }
    std::string codeToCreate = argv[2];
    std::string outFilename = codeToCreate + ".code128.bmp";
    
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
      dout << " " << (int) (*vecIterator);
    } // for all elements
    dout << endl;

    std::string fullBarcode = code128::getBarCode(codeValues);
    dout << fullBarcode << endl;

    { // create image and write to file
      pixelbild barcodeBild = toPixelbild(fullBarcode);
    
      bmpreader::writeFile(barcodeBild,outFilename);
    }

  } // if create
  if("--create128I" == programMode){ // 'i'nteractive
    std::vector<uint8_t> codeValues;
    std::cout << "Interactive Mode; enter ? for help" << std::endl;
    while (true){
      std::string command;
      std::cout << "mkbarcode:> ";
      std::cin >> command;
      if ("?" == command){
        using std::cout; using std::endl;
        cout << "Decimal Value 0-106 : Code for value" << endl;
        cout << "'char'              : code for char in CodeB" << endl;
        cout << "StartA              : Start in Code A (103)"  << endl;
        cout << "StartB              : Start in Code B (104)"  << endl;
        cout << "StartC              : Start in Code C (105)"  << endl;
        cout << "SwitchA             : Switch to Code A (101)" << endl;
        cout << "SwitchB             : Switch to Code B (100)" << endl;
        cout << "SwitchC             : Switch to Code C (99)"  << endl;
        cout << "Checksum            : Calculate (and add) checksum" << endl;
        cout << "Stop                : Stop Char (106)"        << endl;
        cout << endl;
        cout << "?                   : Print this help" << endl;
        cout << "?128                : Print detailled help for Code128" << endl;
        cout << "p                   : Print Values (before converting)" << endl;                   
        cout << ":wq                 : Quit and Save" << endl;
        cout << ":q!                 : Quit without saving" << endl;
        continue;
      }
      if ("?128" == command){
        code128::printHelp(std::cout);
        continue;
      }
      if ("q" == command){
        std::cout << "Use :wq to write or :q! to quit" << std::endl;
        continue;
      }
      if (":q" == command){
        std::cout << "Use :wq to write or :q! to quit" << std::endl;
        continue;
      }
      if (":wq" == command){
        break;
      }
      if (":q!" == command){
        exit(EXIT_SUCCESS);
        break;
      }
      if ("p" == command){
        std::cout << "Input: ";
        std::vector<uint8_t>::const_iterator value_iterator;
        for (
          value_iterator  = codeValues.begin();
          value_iterator != codeValues.end();
          value_iterator++
          )
        {
          std::cout << " " << std::dec << (int)(*value_iterator);
        }
        std::cout << std::endl;
        continue;
      }
      if ("StartA" == command){
        codeValues.push_back(103);
        continue;
      }
      if ("StartB" == command){
        codeValues.push_back(104);
        continue;
      }
      if ("StartC" == command){
        codeValues.push_back(105);
        continue;
      }
      if ("SwitchA" == command){
        codeValues.push_back(101);
        continue;
      }
      if ("SwitchB" == command){
        codeValues.push_back(100);
        continue;
      }
      if ("SwitchC" == command){
        codeValues.push_back(99);
        continue;
      }
      if ("Stop" == command){
        codeValues.push_back(106);
        continue;
      }
      if ('\'' == command[0]){
        if (command.size()<2){
          std::cout << "no char given?" << std::endl;
          continue;
        }
        uint8_t newValue = code128::getValueTypeB(command[1]);
        std::cout << command[1] << " is value " << std::dec << (int)newValue << std::endl;
        codeValues.push_back(newValue);
        continue;
      }
      if ("Checksum" == command){
        int checkSum = code128::calculateChecksum(codeValues);
        std::cout << "Checksum is " << std::dec << checkSum << std::endl;
        codeValues.push_back(checkSum);
        continue;
      }
      { // else: try to read as decimal int
        std::istringstream convertStream(command);
        int newValue;
        convertStream >> std::dec;
        convertStream.clear(); // clears error flags
        if (convertStream >> newValue){ // true if success
          codeValues.push_back(newValue);
        }else{
          std::cout << "decimal value could not be read" << std::endl;
        }
        continue;
      }
      
     
    }
    std::cout << "Output Filename:> ";
    std::string outFilename;
    std::cin >> outFilename;

    std::string fullBarcode = code128::getBarCode(codeValues);
    { // create image and write to file
      pixelbild barcodeBild = toPixelbild(fullBarcode);
    
      bmpreader::writeFile(barcodeBild,outFilename);
    }
  }
  if("--create128K" == programMode){
    if (argc<3){
      std::cerr << "No number given - nothing to create" << std::endl;
      mkbarcode::printHelpReminder(argv[0]);
      exit(EXIT_FAILURE);
    }
    using std::dec;
    unsigned int numberToCreate;
    { // convert given number K12345678 
      std::stringstream convertStream;
      convertStream << argv[2];
      convertStream >> dec >> numberToCreate;
    }
    std::string outFilename;
    { // convert number to filename
      std::ostringstream convertStream;
      convertStream << dec << numberToCreate;
      convertStream << ".code128.bmp";
      outFilename = convertStream.str();
    }
    
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
      dout << " " << (int) (*vecIterator);
    } // for all elements
    dout << endl;
    std::string fullBarcode = code128::getBarCode(codeValues);
    dout << fullBarcode << endl;
    
    { // create image and write to file
      pixelbild barcodeBild = toPixelbild(fullBarcode);
    
      bmpreader::writeFile(barcodeBild,outFilename);
    }
    
  } // if create
  
  
  dout.endScope("main");      
  return 0;
}

