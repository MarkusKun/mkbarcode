#ifndef _EAN_H
#define _EAN_H

#include <string>
#include <vector>
#include <stdint.h>

namespace ean{
  enum codeType_t{
    CODETYPE_UNKNOWN,
    CODETYPE_RIGHT,
    CODETYPE_MIRROR,
    CODETYPE_INVERSE
  };
  
  /*!
   * \brief return "right hand"-barcode for given number (0-9)
   *
   * this function returns the "right-hand"-representation of the barcode
   * for the given number. It is called "right hand" as it is the 
   * later part of the barcode.
   */
  std::string getRightHand(uint8_t number);
  
  /*!
   * \brief return "left hand mirror"-barcode for given number (0-9)
   *
   * This function returns the "left-hand mirror"-representation of the barcode
   * for the given number. Is is called "left hand" as it is the 
   * first part of a regular barcode. This is the same as the
   * right-hand-code mirrored (read from right to left). Sometimes, this
   * is also called "even" representation.
   */
  std::string getLeftHandMirror(uint8_t number);
  /*!
   * \brief return "left hand inverted"-barcode for given number (0-9)
   *
   * This function returns the "left-hand inverted"-representation of the barcode
   * for the given number. Is is called "left hand" as it is the 
   * first part of a regular barcode. This is the same as the
   * right-hand-code inverted (black to white and white to black). Sometimes, 
   * this is also called "odd" representation.
   * 
   * Note that at least on "odd" has to be present in the left hand
   * as else an upside-down (mirrored) barcode could be decoded, too 
   */
  std::string getLeftHandInvert(uint8_t number); 
  
  /*!
   * \brief return the "order" for left hand for given number (0-9)
   *
   * This function returns the "order" for the left hand for a given number.
   * The order is the first digit of an EAN-13.
   */
  std::string getLeftHandOrder(uint8_t number);
  
  /*!
   * \brief lookup the barcode and return the number (for right hand)
   * 
   * This function assumes the given string to be a right-hand-barcode
   * and checks whether it is the same as one of the known strings
   * for ean-barcodes 0-9. If it is, the number is returned.
   * If not, magic number 42 is used to indicate an error.
   *
   * \return 0-9 if string is known barcode; 42 on error
   */
  uint8_t lookupRightHand(const std::string& barcode);
  
  /*!
   * \brief lookup a given sort order, return the index (if valid)
   *
   * This function takes the order of the left-hand and looks up
   * the index number (if the order is valid).
   * In EAN-13 the order is given for six digits and makes up the
   * first digit of the code. This way, an american UPC-12
   * expands to an EAN-13 with a Zero in front.
   */
  uint8_t lookupLeftHandOrder(const std::string& givenOrder);
   
  
  
  struct codeReturn{
    codeType_t  codeType;
    uint8_t     codeValue;
  };
  
  /*!
   * \brief lookup the barcode and return the number (for all)
   *
   * This function tries to match the barcode against the known
   * barcodes for numbers 0-9. It first checks right-hand, but also
   * tries mirroring or inverting the code to get a result.
   * If a match is found, the appropiate type and number are returned
   */
  codeReturn getType(const std::string& barcode);
  
  std::vector<ean::codeReturn> getTypes(const std::string& barcode);
  
  /*!
   * \brief calculate checksum for a string of numbers
   *
   * This function calculates the checksum for the given 
   * number string. Ensure that an existing checksum is 
   * split from the numbers. Validation should be done
   * outside this function
   */
  unsigned int calculateChecksum(const std::string& numbers);
  
}; // namespace



#endif // _EAN_H
