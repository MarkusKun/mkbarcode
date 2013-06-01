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
  cout << "  --lookupEAN BARCODE7   lookup a single digit for EAN" << endl;
  cout << "  --lookupEAN BARCODE    lookup a complete EAN" << endl;
  cout << "  --lookupEAN            lookup EAN - interactive mode" << endl;
  cout << "  --createEAN NUMBER     create an EAN for this number" << endl;
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
  
void mkbarcode::printLookupResult(const ean::codeReturn result){
  using std::cout; using std::endl;
  switch (result.codeType){
  case ean::CODETYPE_UNKNOWN: std::cout << "unknown code"; break;
  case ean::CODETYPE_RIGHT:   std::cout << "Right "      ; break;
  case ean::CODETYPE_MIRROR:  std::cout << "Mirrored "   ; break;
  case ean::CODETYPE_INVERSE: std::cout << "Inverse "    ; break;
  } // switch
  if (ean::CODETYPE_UNKNOWN != result.codeType){
    std::cout << std::dec << (int)result.codeValue;
  }
  std::cout << std::endl;
}

void mkbarcode::printTest(std::ostream& out){
  uint8_t testZiffer;
  for(
    testZiffer=0;
    testZiffer<10;
    testZiffer++
    )
  {
    out << (int) testZiffer << " Right " << ean::getRightHand(testZiffer);
    out << " LeftI " << ean::getLeftHandInvert(testZiffer);
    out << " LeftM " << ean::getLeftHandMirror(testZiffer);
    out << " Order " << ean::getLeftHandOrder(testZiffer);
    out << " 128: " << code128::getBarCode(testZiffer);
    out << std::endl;
  }
}
  
void mkbarcode::create128interactive(){
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
  } // while true;
  std::cout << "Output Filename:> ";
  std::string outFilename;
  std::cin >> outFilename;

  std::string fullBarcode = code128::getBarCode(codeValues);
  { // create image and write to file
    pixelbild barcodeBild = toPixelbild(fullBarcode);
  
    bmpreader::writeFile(barcodeBild,outFilename);
  }
}

void mkbarcode::lookupEANinteractive(){
  using std::cout; using std::endl;
  std::cout << "Interactive mode; enter ? for help" << std::endl;
  while (true){
    std::string command;
    std::cout << "mkbarcode:> ";
    std::cin >> command;
    if ("?" == command){
      cout << "7-Char-Code : lookup single char" << endl;
      cout << "101         : mark start or end of code" << endl;
      cout << "01010       : mark middle of code" << endl;
      cout << ":q!         : quit" << endl;
      continue;
    }
    if (":q!" == command){
      exit(EXIT_SUCCESS);
      break;
    }
    if ("101" == command){
      cout << "Start or End" << endl;
      continue;
    }
    if ("01010" == command){
      cout << "Middle of code" << endl;
      continue;
    }
    { // else: try to read as barcode
      if (7==command.size()){
        ean::codeReturn lookupResult = ean::getType(command);
        mkbarcode::printLookupResult(lookupResult);
      }else{
        cout << "Wrong length; an EAN digit has 7 pixel" << endl;
      }
      continue;
    }
  }// while true;
}

void mkbarcode::lookupEAN(const std::string& givenBarcode){
  using std::endl;
  dout.startScope("mkbarcode::lookupEAN");
  unsigned int barcodeSize = givenBarcode.size();
  dout << "Size of given code: " << barcodeSize << endl;
  if (barcodeSize < 2*3+5){
    std::cerr << "Code too short" << endl;
    exit(EXIT_FAILURE);
  }
  barcodeSize = barcodeSize - 2*3;
  dout << "That is " << barcodeSize << " without Preamble and Postamble";
  barcodeSize = barcodeSize - 5;
  dout << " and " << barcodeSize << " without middle marker" << std::endl; 
  if (barcodeSize % 7 != 0){
    std::cerr << "Invalid size (" << barcodeSize << ") - not a multiple of 7" << std::endl;
    exit(EXIT_FAILURE);
  }
  unsigned int digitCount = barcodeSize / 7;
  dout << "It is safe to assume that this code contains " << digitCount << " digits" << std::endl;
  if (digitCount % 2 != 0){
    std::cerr << "I cannot work with " << digitCount << " digits (odd number)" << std::endl;
    exit(EXIT_FAILURE);
  }
  unsigned int startOfPreamble  = 0;
  dout << startOfPreamble << ",";
  unsigned int startOfLeftHand  = startOfPreamble + 3; // 3
  dout << startOfLeftHand << ",";
  unsigned int startOfMiddle    = startOfLeftHand + barcodeSize/2; 
  dout << startOfMiddle << ",";
  unsigned int startOfRightHand = startOfMiddle + 5;
  dout << startOfRightHand << ",";
  unsigned int startOfPostamble = startOfRightHand+ barcodeSize/2;
  dout << startOfPostamble << std::endl;
  
  std::string preamble = givenBarcode.substr(startOfPreamble,3);
  std::string leftHand  = givenBarcode.substr(startOfLeftHand,barcodeSize/2);
  std::string middle = givenBarcode.substr(startOfMiddle,5);
  std::string rightHand = givenBarcode.substr(startOfRightHand,barcodeSize/2);
  std::string postamble = givenBarcode.substr(startOfPostamble,3);
  
  { // reality checks on the constants
    if ("101" != preamble){
      std::cerr << "ERROR: Preamble is incorrect (" << preamble << " should be 101)" << std::endl;
      exit(EXIT_FAILURE);
    }
    if ("101" != postamble){
      std::cerr << "ERROR: Postamble is incorrect (" << preamble << " should be 101)" << std::endl;
      exit(EXIT_FAILURE);
    }
    if ("01010" != middle){
      std::cerr << "ERROR: Middle is incorrect (" << preamble << " should be 01010)" << std::endl;
      exit(EXIT_FAILURE);
    }
  }
    
  std::vector <ean::codeReturn> rightHandResults = ean::getTypes(rightHand);
  { // test if each char was correct in right hand. Delete results if not
    dout << "Right Hand, try1: ";
    std::vector<ean::codeReturn>::const_iterator resultIterator;
    for (
      resultIterator  = rightHandResults.begin();
      resultIterator != rightHandResults.end();
      resultIterator++
      )
    { // test if each char was correct right hand
      dout << (int)resultIterator->codeValue << " ";
      if (ean::CODETYPE_RIGHT != resultIterator->codeType){
        std::cout << "Invalid char in right hand." << std::endl;
        rightHandResults.clear(); // invalidate results
        break;
      }
    }
    dout << endl;
  }
  // if error was found, barcode could be upside down
  if (rightHandResults.empty()){ 
    std::cout << "Trying upside down" << std::endl;
    std::string oldRightHand = rightHand;
    rightHand = mirror(leftHand);
    leftHand  = mirror(oldRightHand);
    rightHandResults = ean::getTypes(rightHand);
  }
  std::string rightHandDecoded;
  { // test if each char was correct in right hand. Delete results if not
    dout << "Right Hand, try2: ";
    std::vector<ean::codeReturn>::const_iterator resultIterator;
    std::stringstream decodeStream;
    for (
      resultIterator  = rightHandResults.begin();
      resultIterator != rightHandResults.end();
      resultIterator++
      )
    { // test if each char was correct right hand
      dout << (int)resultIterator->codeValue << " ";
      if (ean::CODETYPE_RIGHT != resultIterator->codeType){
        std::cout << "Invalid char in right hand." << std::endl;
        rightHandResults.clear(); // invalidate results
        break;
      }
      decodeStream << (int)resultIterator->codeValue;
    }
    dout << endl;
    rightHandDecoded = decodeStream.str();
  }
  if (rightHandResults.empty()){
    std::cerr << "Right hand still invalid!" << std::endl;
    exit(EXIT_FAILURE);
  }
  std::vector<ean::codeReturn> leftHandResults = ean::getTypes(leftHand);
  std::string leftHandOrder;
  std::string leftHandDecoded;
  { // test left hand and collect order information
    std::vector<ean::codeReturn>::const_iterator resultIterator;
    std::stringstream orderStream;
    std::stringstream decodeStream;
    for (
      resultIterator  = leftHandResults.begin();
      resultIterator != leftHandResults.end();
      resultIterator++
      )
    { // test if each char was correct in left hand
      dout << (int)resultIterator->codeValue << " ";
      switch (resultIterator->codeType){
      case ean::CODETYPE_UNKNOWN: orderStream << "#"; break; // invalid
      case ean::CODETYPE_RIGHT  : orderStream << "R"; break; // right hand shouldn't be here
      case ean::CODETYPE_MIRROR : orderStream << "E"; break; // mirrored / even
      case ean::CODETYPE_INVERSE: orderStream << "O"; break; // inverse  / odd
      }//switch
      decodeStream << (int)resultIterator->codeValue;
    }
    dout << endl;
    leftHandOrder = orderStream.str();
    leftHandDecoded = decodeStream.str();
    dout << "LeftHand: " << leftHandDecoded << " Order: " << leftHandOrder << endl;
  } // after iteration;
  
  std::string orderDigit = "";
  { // check leftHand Order. calculate Digit if needed
    if (6==leftHandDecoded.size()){ // EAN13 - look up order for first digit
      unsigned int leftHandOrderIndex = ean::lookupLeftHandOrder(leftHandOrder);
      if (leftHandOrderIndex>9){
        std::cerr << "Left hand order " << leftHandOrder << " seems invalid!" << endl;
        exit(EXIT_FAILURE);
      }
      dout << "Order to index " << leftHandOrderIndex << endl;
      std::stringstream convertStream;
      convertStream << std::dec << leftHandOrderIndex;
      orderDigit = convertStream.str();
    }
    if (4==leftHandDecoded.size()){ // EAN8 - should be all odd
      if ("OOOO" != leftHandOrder){
        std::cerr << "Error in Left Hand Order: " << leftHandOrder << " should be OOOO" << endl;
        exit(EXIT_FAILURE);
      }
    }
  }
  
  std::string completeCode;
  
  { // concat all strings
    std::stringstream completeCodeStream;
    completeCodeStream << orderDigit;
    completeCodeStream << leftHandDecoded;
    completeCodeStream << rightHandDecoded;
    completeCode = completeCodeStream.str();
  }
  
  { // calculate and check checksum
    std::string calcChecksum;
    std::string readChecksum;
    if (! ean::checkChecksum(completeCode,readChecksum,calcChecksum)){
      std::cerr << "Invalid Checksum: " << readChecksum << " should be " << calcChecksum << endl;
      exit(EXIT_FAILURE);
    }
    dout << "checksums calc: " << calcChecksum << " read: " << readChecksum << endl;
  }
  std::cout << "Code is: " << completeCode << endl;
  dout.endScope("mkbarcode::lookupEAN");
}

void mkbarcode::createEAN(const std::string& givenNumber){
  using std::endl; using std::cerr; using std::cout;
  dout.startScope("mkbarcode::createEAN");
  /*
   * There are several options on what is given and on how
   * this should be interpreted:
   *
   * - If 7 numbers are given, an EAN-8-code should be created.
   *   The checksum has to be calculated
   * - If 8 numbers are given, an EAN-8-code should be created.
   *   The checksum should be validated
   * - if 11 numbers are given, an EAN-12 (UPC) should be created.
   *   The checksum has to be calculated
   * - if 12 numbers are given, this could be a request for an
   *   EAN-12 (UPC) or an EAN-13 for which the checksum is to
   *   be calculated. 
   *   In either case, the user should be informed about the
   *   ambiguity. 
   *   However, the program should try to validate the checksum as
   *   an UPC code and create that one, if the checksum is valid.
   *   If the checksum is invalid, assume that this is an EAN-13
   *   for which the barcode still needs to be calculated
   *
   * Remember that all EAN codes start and end with 101 and that
   * even the small EAN-8 have a middle zone of 01010.
   */
  std::string paddedNumber;
  { // pad number, if necessary
    std::stringstream padNumberStream;
    switch (givenNumber.size()){
    case 7:
      {
        std::cout << "Warning: Incomplete (7-digit) number given; ";
        std::string calcChecksum = ean::calculateChecksum(givenNumber);
        std::cout << "Adding Checksum: " << calcChecksum << endl;
        padNumberStream << givenNumber << calcChecksum;
        break;
      }
    case 11:
      {
        std::cout << "Warning: Incomplete (11-digit) number given; " << endl;
        std::string calcChecksum = ean::calculateChecksum(givenNumber);
        std::cout << "Adding Checksum: " << calcChecksum;
        std::cout << " and Prefix 0 for UPC" << endl;
        padNumberStream << "0" << givenNumber << calcChecksum;
        break;
      }
    case 12:
      {
        std::cout << "Warning: 12-digit-number can be interpreted two ways." << endl;
        // first: try to validate checksum as UPC
        std::string calcChecksumUPC;
        std::string readChecksumUPC;
        bool checksumValid = ean::checkChecksum(givenNumber,readChecksumUPC,calcChecksumUPC);
        std::string calcChecksumEAN13 = ean::calculateChecksum(givenNumber);
        std::cout << "This could be:" << std::endl;
        std::cout << "0"<<givenNumber << " (";
        if (!checksumValid){
          std::cout << "invalid checksum, should be " << calcChecksumUPC;
        }else{
          std::cout << "valid checksum";
        }
        std::cout << ")" << endl;
        std::cout << givenNumber << calcChecksumEAN13;
        std::cout << " (calculated checksum)" << endl;
        std::cout << "To prevent guessing, please give really wanted number" << endl;
        if (! checksumValid){
          std::cout << "Checksum invalid as UPC; treating as EAN-13 ;";
          std::cout << "Adding Checksum: " << calcChecksumEAN13 << endl;
          padNumberStream << givenNumber << calcChecksumEAN13;
        }else{ // it *is* valid as UPC
          std::cout << "Checksum valid as UPC; treating as UPC; ";
          std::cout << "Adding Prefix 0 for UPC" << endl;
          padNumberStream << "0" << givenNumber;
          // this is the ambiguity case which can lead to wrong/undesired behaviour 
        }
        break;
      }
    case 8:
    case 13: // nothing to do. Check Checksum, later
      {
        padNumberStream << givenNumber;
        break;
      }
    default:
      {
        std::cerr << "Error: Size (" << givenNumber.size() << ")not supported";
        std::cerr << " and not recoverable" << std::endl;
        exit(EXIT_FAILURE);
        break;
      }
    } // switch
    paddedNumber=padNumberStream.str();
  }
  dout << "Number after patching: " << paddedNumber << endl;
  
  
  { // test the checksum
    // for padded numbers, this should (hopefully) always work
    std::string calcChecksum;
    std::string readChecksum;
    if (! ean::checkChecksum(paddedNumber,readChecksum,calcChecksum)){
      std::string correctNumber = paddedNumber;
      std::cout << "Warning: Checksum is wrong. Is " << readChecksum;
      std::cout << " but should be " << calcChecksum << endl;
      std::cout << "I will respect your wish to create the wrong barcode ";
      std::cout << " - But don't expect it to be readble." << endl;
      correctNumber[correctNumber.size()-1] = calcChecksum[0];//  append correct sum
      std::cout << "You may want to try again with " << correctNumber << endl;
    }
    dout << "checksums calc: " << calcChecksum << " read: " << readChecksum << endl;
  } // checksum test
  
  // finally: The real creation
  std::string leftHand;
  std::string rightHand;
  std::string leftHandOrder;
  { // divide into hands and determine lefthand-order
    switch (paddedNumber.size()){
    case 8:
      {
        leftHand=paddedNumber.substr(0,4);
        rightHand=paddedNumber.substr(4,4);
        leftHandOrder="OOOO";
        break;
      }
    case 13:
      {
        leftHand=paddedNumber.substr(1,6);
        rightHand=paddedNumber.substr(7,6);
        uint8_t orderIndex = paddedNumber[0]-'0';
        leftHandOrder=ean::getLeftHandOrder(orderIndex);
        break;
      }
    default:
      std::cerr << "error: wrong size for barcode" << endl;
      exit(EXIT_FAILURE);
      break;
    } // switch
    dout << "L: " << leftHand << " R: " << rightHand << " O: " << leftHandOrder << endl;
  }
  
  std::stringstream barcodeStream;
  { // start
    barcodeStream << "101";
  }
  { // left hand
    std::string::const_iterator orderIterator;
    std::string::const_iterator handIterator;
    for (
      orderIterator  = leftHandOrder.begin(), handIterator  = leftHand.begin();
      orderIterator != leftHandOrder.end()  , handIterator != leftHand.end();
      orderIterator++                       , handIterator++
      )
    { // for all digits
      uint8_t currentDigit=(*handIterator)-'0';
      if ('E' == *orderIterator){ // even
        barcodeStream << ean::getLeftHandMirror(currentDigit);
      }else{ // assume odd
        barcodeStream << ean::getLeftHandInvert(currentDigit);
      }
    }
  } // left hand

  { // middle
    barcodeStream << "01010";
  }
  { // right hand
    std::string::const_iterator handIterator;
    for(
      handIterator  = rightHand.begin();
      handIterator != rightHand.end();
      handIterator++
      )
    { // for all digits on the right hand
      barcodeStream << ean::getRightHand((*handIterator)-'0');
    }
  } // right hand
  { // end
    barcodeStream << "101";
  }
  dout << "Barcode should be " << barcodeStream.str() << endl;
  std::string outFilename;
  { // convert number to filename
    outFilename = paddedNumber + ".EAN.bmp";
  }
   
  { // create image and write to file
    pixelbild barcodeBild = toPixelbild(barcodeStream.str());
    bmpreader::writeFile(barcodeBild,outFilename);
  }
  
  dout.endScope("mkbarcode::createEAN");
  
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
    mkbarcode::printTest(std::cout);
  }
  if ("--lookupEAN" == programMode){
    if (argc<3){ // no barcode given - interactive mode
      mkbarcode::lookupEANinteractive();
    }else{ // barcode was given
      std::string testcode = argv[2];
      if (7==testcode.size()){ // a single digit is easy
        cout << "7 bars -> single char" << endl;
        ean::codeReturn lookupResult = ean::getType(testcode);
        mkbarcode::printLookupResult(lookupResult);
      }else{ // complete code?
        cout << "not 7 bars -> complete code" << endl;
        mkbarcode::lookupEAN(testcode);
      }
    } // barcode given
  } // lookup EAN
  if ("--lookup128" == programMode){
    std::string testcode = argv[2];
    if (argc<3){
      std::cerr << "No barcode given - nothing to lookup" << std::endl;
      mkbarcode::printHelpReminder(argv[0]);
      exit(EXIT_FAILURE);
    }
    code128::readComplete(testcode);
  }
  if ("--createEAN" == programMode){ // EAN
    if (argc<3){
      std::cerr << "No string given - nothing to create" << std::endl;
      mkbarcode::printHelpReminder(argv[0]);
      exit(EXIT_FAILURE);
    }
    std::string codeToCreate = argv[2];
    mkbarcode::createEAN(codeToCreate);
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
    mkbarcode::create128interactive();
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

