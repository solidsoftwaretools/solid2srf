//Copyright 2007 Asim Siddiqui for AB
//
// License TBD by AB

#ifndef SRF_COMMANDLINEARGS_HH
#define SRF_COMMANDLINEARGS_HH
//
#include <string>
#include <map>

#define SRF_ARGS_OUTPUT "-o"
#define SRF_ARGS_CFASTA "-cf"
#define SRF_ARGS_QUAL "-q"
#define SRF_ARGS_INTENSITY "-i"
#define SRF_ARGS_XML "-x"
#define SRF_ARGS_TITLE "-t"
#define SRF_ARGS_TITLE_OVERRIDE "-to"
#define SRF_ARGS_NO_AB_PREFIX "-noABprefix"
#define SRF_ARGS_NO_PRIMER_BASE_IN_REGN "-noPrimerBaseInREGN"
#define SRF_ARGS_CFASTA2 "-cf2"
#define SRF_ARGS_QUAL2 "-q2"
#define SRF_ARGS_INTENSITY2 "-i2"

class SRF_CommandLineArgs
{
    public:
        SRF_CommandLineArgs( void );

        ~SRF_CommandLineArgs( void );

        bool extract( int argc, char** argv );

        bool flagSet( const std::string& key ) const;
        bool getArgValue( const std::string& key, std::string* value ) const;
        std::string retArgValue( const std::string& key ) const;
        bool isPairedEndWrite( void ) const;

        void dump( void ) const;
    protected:

    private:
        bool check( void );

        std::map<std::string, std::string> args;
        bool pairedEndWrite;
};

#endif
