//
#include <map>
#include <string>
#include <SRF_util.hh>
#include <SRF_CommandLineArgs.hh>

SRF_CommandLineArgs::SRF_CommandLineArgs( void )
{
    pairedEndWrite = FALSE;
}
    
SRF_CommandLineArgs::~SRF_CommandLineArgs( void )
{
    // no-op
}


bool
SRF_CommandLineArgs::extract( int argc, char* argv[] )
{
    int ii = 1;
    while ( ii < argc )
    {
        std::map<std::string, std::string>::const_iterator it;
        std::string currArg( argv[ii] );

        it = args.find( currArg );
        if ( it != args.end() )
        {
            std::cout << "ERROR: Command line argument duplicated: " <<
                currArg << std::endl;
            return FALSE;
        }

        if ( (ii + 1) < argc )
        {
            std::string nextArg( argv[ii + 1] );
            if ( nextArg[0] != '-' )
            {
                args.insert( std::pair<std::string, std::string>( currArg,
                                                                  nextArg )); 
            }
            else
            {
                args.insert( std::pair<std::string, std::string>( currArg,
                                                                  "" )); 
            }
        }
        else
        {
            args.insert( std::pair<std::string, std::string>( currArg,
                                                              "" )); 
        }
        ii++;
    }

    if ( !check() )
    {
        return FALSE;
    }
        
    return TRUE;
}

bool
SRF_CommandLineArgs::check( void )
{
    // ensure that rquired args are present and args correctly formed
    std::string value;
    if ( ( !getArgValue( SRF_ARGS_CFASTA, &value ) || value == "" ) ||
         ( !getArgValue( SRF_ARGS_TITLE, &value ) || value == "" ) ||
         ( !getArgValue( SRF_ARGS_OUTPUT, &value ) || value == "" ) )
    {
        std::cout << "ERROR: mandatory command line args malformed or missing\n";
        return FALSE;
    }

    if ( ( getArgValue( SRF_ARGS_QUAL, &value ) && value == "" ) ||
         ( getArgValue( SRF_ARGS_INTENSITY, &value ) && value == "" ) ||
         ( getArgValue( SRF_ARGS_XML, &value ) && value == "" ) ||
         ( getArgValue( SRF_ARGS_TITLE_OVERRIDE, &value ) && value != "" ) ||
    ( getArgValue( SRF_ARGS_NO_PRIMER_BASE_IN_REGN, &value ) && value != "" ) ||
         ( getArgValue( SRF_ARGS_NO_AB_PREFIX, &value ) && value != "" ) )
    {
        std::cout << "ERROR: optional command line args malformed\n";
        return FALSE;
    }

    // paired end files
    pairedEndWrite = FALSE;
    if ( getArgValue( SRF_ARGS_CFASTA2, &value ) )
    {
        pairedEndWrite = TRUE;
        // check to see args correct
        if ( value == "" ||
             ( getArgValue( SRF_ARGS_QUAL2, &value ) && value == "" ) ||
             ( getArgValue( SRF_ARGS_INTENSITY2, &value ) && value == "" ) )
        {
            std::cout << "ERROR: argumenets specifying second set of files malformed\n";
            return FALSE;
        }

        // check to see if args align
        if ( ( getArgValue( SRF_ARGS_QUAL, &value ) &&
               !getArgValue( SRF_ARGS_QUAL2, &value ) ) ||
             ( !getArgValue( SRF_ARGS_QUAL, &value ) &&
               getArgValue( SRF_ARGS_QUAL2, &value ) ) ||
             ( getArgValue( SRF_ARGS_INTENSITY, &value ) &&
               !getArgValue( SRF_ARGS_INTENSITY2, &value ) ) ||
             ( !getArgValue( SRF_ARGS_INTENSITY, &value ) &&
               getArgValue( SRF_ARGS_INTENSITY2, &value ) ) )
        {
            std::cout << "ERROR: for paired end data, same sources must be specified for each file set\n";
            return FALSE;
        }
    }

    return TRUE;
}

bool
SRF_CommandLineArgs::getArgValue( const std::string& key,
                                  std::string* value ) const
{
    std::map<std::string, std::string>::const_iterator it;

    it = args.find( key );

    if ( it == args.end() )
    {
        return FALSE;
    }

    *value = (*it).second;

    return TRUE;
}

std::string
SRF_CommandLineArgs::retArgValue( const std::string& key ) const
{
    std::map<std::string, std::string>::const_iterator it;

    it = args.find( key );

    if ( it == args.end() )
    {
        return "";
    }

    return( (*it).second );
}


bool
SRF_CommandLineArgs::flagSet( const std::string& key ) const
{
    std::map<std::string, std::string>::const_iterator it;

    it = args.find( key );

    if ( it == args.end() )
    {
        return FALSE;
    }

    return TRUE;
}

void
SRF_CommandLineArgs::dump( void ) const
{
    std::map<std::string, std::string>::const_iterator it;
    it = args.begin();

    std::cout << "COMMAND LINE ARGS" << std::endl;

    while( it != args.end() )
    {
        std::cout << (*it).first << " " << (*it).second << std::endl;

        it++;
    }
}

bool
SRF_CommandLineArgs::isPairedEndWrite( void ) const
{
    return pairedEndWrite;
}
