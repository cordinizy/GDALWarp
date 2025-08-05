//
// Created by zhangyue on 4/16/25.
//

#ifndef GF5A_WTI_CORRECT_RESOLUTIONTOMETERS_H
#define GF5A_WTI_CORRECT_RESOLUTIONTOMETERS_H

#include <iostream>
#include <cfloat>
#include <cmath>
//#include <proj_api.h>
#include <projects.h>

class ResolutionToMeters {
public:
    int CalculateUTMZone(double lon, double lat);

    void ConvertResolutionToMeters(double &xResolutionDeg, double &yResolutionDeg, double centerLon, double centerLat);

};


#endif //GF5A_WTI_CORRECT_RESOLUTIONTOMETERS_H
