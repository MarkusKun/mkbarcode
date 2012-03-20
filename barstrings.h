#ifndef _BARSTRINGS_H
#define _BARSTRINGS_H

#include <string>

/*!
 * \brief mirrors the given string
 *
 * This function returns a backward-version of the given string.
 * It is needed to identify barcodes read upside down. Also
 * EAN (and probably others) use a mirrored representation for
 * some codes.
 *
 * "1100101" becomes "1010011"
 *
 * \note In general, this function could be used for anything.
 *       there is no restriction to the used chars
 *
 * \param[in] barcode   code to mirror
 * \return              mirrored code
 */
std::string mirror(const std::string& barcode);
/*!
 * \brief inverts the given string
 *
 * This function returns an inverse of the given string.
 * Zeros are replaced by Ones, Ones are replaced by Zeros.
 * 
 * \note Characters in the string that are not '0' are replaced by '0'
 *       (obviously true for '1', but all others are treated as '1', too)
 *
 * "1100101" becomes "0011010"
 *
 * \param[in] barcode   code to invert
 * \return              inverted code
 */
std::string invert(std::string barcode);

#endif // BARSTRINGS_H
