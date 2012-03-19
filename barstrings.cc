#include "barstrings.h"

std::string invert(std::string barcode){
  std::string::iterator stringRunner;
  for (
    stringRunner  = barcode.begin();
    stringRunner != barcode.end();
    stringRunner++
    )
  {
    if ('0' == (*stringRunner)){
      (*stringRunner)='1';
    }else{
      (*stringRunner)='0';
    }
  }
  return barcode;
}

std::string mirror(const std::string& barcode){
  std::string returnString;
  std::string::const_reverse_iterator stringRunner;
  for (
    stringRunner  = barcode.rbegin();
    stringRunner != barcode.rend();
    stringRunner++
    )
  {
    returnString.push_back(*stringRunner);
  }
  return returnString;
}

