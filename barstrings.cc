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

pixelbild toPixelbild(const std::string& barcode){
  pixel blackPixel(0,0,0);
  pixelbild meinBild(barcode.size(),1,blackPixel); // default is black
  { // fill bild with pixels from barcode
    pixel whitePixel(255,255,255);
    std::string::const_iterator stringIterator;
    unsigned int pixelPos;
    for (
      stringIterator  = barcode.begin(), pixelPos=0;
      stringIterator != barcode.end();
      stringIterator++, pixelPos++
      )
    {
      if ('0' == *stringIterator){ // should be white
        meinBild.setPixel(0,pixelPos,whitePixel);
      }
    }
  }
  return meinBild;
}

