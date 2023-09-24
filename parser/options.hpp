#ifndef options_hpp
#define options_hpp

#include <string>

/*******************************************************************************
 * Parses and stores the command line options.
 */
class Options {
  public:
    bool parse(int argc, char *argv[]);
    void display_help();
    void display_license();

    std::string inpath;
    std::string outpath;
    bool show_help      = false;
    bool show_version   = false;
    bool show_lexer     = false;
    bool show_parser    = false;
    bool show_states    = false;
    
private:
    bool parse_option(char c, int argc, char *argv[], int* idx);
};


#endif
