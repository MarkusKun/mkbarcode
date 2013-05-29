#ifndef _MKBARCODE_H
#define _MKBARCODE_H


namespace mkbarcode{
  /*!
   * \brief print help output
   *
   * This function prints the help message to stdout.
   * It is to be called if the program is started with the
   * parameter --help.
   *
   * According to gnits, this should not automatically be printed 
   * on error
   *
   * \param[in] programName   The name of this program as it was called from cmdline
   */
  void printHelp(const std::string& programName);
  
  /*!
   * \brief print reminder on how to get help
   *
   * This function prints the reminder on how to get more help
   * to stdout. It is to be called if something didn't work out
   * with parameter parsing or so.
   *
   * \param[in] programName  The name of this program as it was called from cmdline
   */
  void printHelpReminder(const std::string& programName);
  
  void printLookupResult(const ean::codeReturn result);
  
  void printTest(std::ostream& out);
  
  void create128interactive();
  
  void lookupEANinteractive();
  
  void lookupEAN(const std::string& givenBarcode); 
  
}; // namespace mkbarcode


/*!
 * \brief main function as called from outside
 *
 * This is the main function as called from the shell or from scripts.
 * 
 * \param[in] argc      argument count
 * \param[in] argv      argument value (pointer to c-strings)
 */
int main(int argc, char* argv[]);


#endif // _MKBARCODE_H


