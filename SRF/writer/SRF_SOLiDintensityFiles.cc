//
#include <iostream>
#include <map>
#include <string>
#include <sys/stat.h>
#include "SRF_util.hh"
#include "ZTR_util.hh"
#include "SRF_SOLiDintensityFiles.hh"

SRF_SOLiDintensityFiles::SRF_SOLiDintensityFiles( void )
{
    // no-op
}
    
SRF_SOLiDintensityFiles::~SRF_SOLiDintensityFiles( void )
{
    // no-op
}


bool
SRF_SOLiDintensityFiles::open( const std::string& intensityFilesRoot,
                               const std::string& title,
                               bool titleOverride )
{
    std::string famExt( "FAM.fasta" );
    std::string cy3Ext( "CY3.fasta" );
    std::string txrExt( "TXR.fasta" );
    std::string cy5Ext( "CY5.fasta" );
    std::string famIntensStr = intensityFilesRoot + famExt;
    std::string cy3IntensStr = intensityFilesRoot + cy3Ext;
    std::string txrIntensStr = intensityFilesRoot + txrExt;
    std::string cy5IntensStr = intensityFilesRoot + cy5Ext;

    if(!checkExtension(famIntensStr)) {
        // Legacy support for FTC files
        std::string ftcExt( "FTC.fasta" );
        famIntensStr = intensityFilesRoot + ftcExt;
        checkExtension(famIntensStr);
    }
    checkExtension(cy3IntensStr);
    checkExtension(txrIntensStr);
    checkExtension(cy5IntensStr);

    if ( !famIntens.open( famIntensStr.c_str(),
                          title,
                          titleOverride ) ||
         !cy3Intens.open( cy3IntensStr.c_str(),
                          title,
                          titleOverride ) ||
         !txrIntens.open( txrIntensStr.c_str(),
                          title,
                          titleOverride ) ||
         !cy5Intens.open( cy5IntensStr.c_str(),
                          title,
                          titleOverride ) )
      {
          std::cout << "ERROR: can't open intensity files\n";
          return FALSE;
      }
    return TRUE;

}

bool
SRF_SOLiDintensityFiles::moreData( void )
{
    if ( famIntens.moreData() ||
         cy3Intens.moreData() ||
         txrIntens.moreData() ||
         cy5Intens.moreData() )
    {
        return TRUE;
    }

    return FALSE;
}

bool
SRF_SOLiDintensityFiles::readNextBlock( const std::string& cfastaPartialReadId,
                                        SRF_SOLiDdataSet* dataSet )
{
    if ( !famIntens.isFileOpen() ) // do we have intensity values ?
    {                              // if not, just return
        return TRUE;
    }

    if ( !famIntens.readNextBlockCommon( cfastaPartialReadId ) ||
         !cy3Intens.readNextBlockCommon( cfastaPartialReadId ) ||
         !txrIntens.readNextBlockCommon( cfastaPartialReadId ) ||
         !cy5Intens.readNextBlockCommon( cfastaPartialReadId ) )
    {
       return FALSE;
    }

    std::vector<float> famIntensValues;
    std::vector<float> cy3IntensValues;
    std::vector<float> txrIntensValues;
    std::vector<float> cy5IntensValues;

    if ( !readIntensityValues( famIntens, famIntensValues ) ||
         !readIntensityValues( cy3Intens, cy3IntensValues ) ||
         !readIntensityValues( txrIntens, txrIntensValues ) ||
         !readIntensityValues( cy5Intens, cy5IntensValues ) )
    {
       return FALSE;
    }

    SRF_IntensityData fam, cy3, txr, cy5;
    dataSet->intensityData.clear();

    fam.sampName = "0FAM";
    fam.intensityValues = famIntensValues;
    dataSet->intensityData.push_back( fam );

    cy3.sampName = "1CY3";
    cy3.intensityValues = cy3IntensValues;
    dataSet->intensityData.push_back( cy3 );

    txr.sampName = "2TXR";
    txr.intensityValues = txrIntensValues;
    dataSet->intensityData.push_back( txr );

    cy5.sampName = "3CY5";
    cy5.intensityValues = cy5IntensValues;
    dataSet->intensityData.push_back( cy5 );

    return TRUE;
}

bool
SRF_SOLiDintensityFiles::readIntensityValues( SRF_SOLiDfile& file,
                                              std::vector<float>& values )
{
    values.clear();
    std::string intensValuesString;
    std::getline( *(file.file), intensValuesString );
    ZTR_ConvertStringToFloatVector( intensValuesString, &values );

    return TRUE;
}

bool
SRF_SOLiDintensityFiles::checkExtension( std::string& fileStr )
{
  struct stat st;
  if(stat(fileStr.c_str(), &st) != 0) {
        std::string testgz = fileStr + ".gz";
        if(stat(testgz.c_str(), &st) == 0) {
            fileStr += ".gz";
            return TRUE;
        }
        return FALSE;
  }
  return TRUE;
  // NB fileStr is modified
}

bool
SRF_SOLiDintensityFiles::areFilesOpen( void ) const
{
    // NB: Since the files are opened in order, 
    //  it's possible that the first few may succeed, and the rest could still be closed.
    // Could get away with just checking last file (cy5) here.
    return ( famIntens.isFileOpen() &&
             cy3Intens.isFileOpen() &&
             txrIntens.isFileOpen() &&
             cy5Intens.isFileOpen() );
}
