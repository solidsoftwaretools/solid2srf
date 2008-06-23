//Copyright 2007 Asim Siddiqui for AB
//
// License TBD by AB

#ifndef SRF_SOLIDSTRUCTS_HH
#define SRF_SOLIDSTRUCTS_HH

typedef struct
{
    std::string sampName;
    std::vector<float> intensityValues;
}  SRF_IntensityData;

typedef struct
{
    std::string partialReadId;
    std::string panelId;
    std::string readIdSuffix;
    std::string calls;

    std::vector<int> confValues;

    std::vector<SRF_IntensityData> intensityData;
} SRF_SOLiDdataSet;

#endif
