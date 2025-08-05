//
// Created by zhangyue on 4/16/25.
//

#include "ResolutionToMeters.h"

int ResolutionToMeters::CalculateUTMZone(double lon, double lat) {
    return static_cast<int>(std::floor((lon + 180.0) / 6.0)) + 1;
}

//void SysGeoCorrect::ConvertResolutionToMeters(double& xResolutionDeg, double& yResolutionDeg, double centerLat) {
//    const double METERS_PER_DEGREE_LAT = 111320.0;
//    double metersPerDegreeLon = METERS_PER_DEGREE_LAT * std::cos(centerLat * M_PI / 180.0);
//
//    xResolutionDeg *= metersPerDegreeLon;
//    yResolutionDeg *= METERS_PER_DEGREE_LAT;
//}

void ResolutionToMeters::ConvertResolutionToMeters(double &xResolutionDeg, double &yResolutionDeg, double centerLon, double centerLat) {
    int zone = static_cast<int>(std::floor((centerLon + 180.0) / 6)) + 1;
    char utmProjDef[100];
    if (centerLat >= 0) {
        // north
        std::sprintf(utmProjDef, "+proj=utm +zone=%d +datum=WGS84 +units=m +no_defs", zone);
    } else {
        // south
        std::sprintf(utmProjDef, "+proj=utm +zone=%d +datum=WGS84 +units=m +no_defs +south", zone);
    }

    PJ* pj_geo = static_cast<PJ *>(pj_init_plus("+proj=longlat +datum=WGS84 +no_defs"));
    if (!pj_geo) {
        std::fprintf(stderr, "Error initializing geographic projection\n");
        std::exit(1);
    }

    PJ* pj_utm = static_cast<PJ *>(pj_init_plus(utmProjDef));
    if (!pj_utm) {
        std::fprintf(stderr, "Error initializing UTM projection\n");
        pj_free(pj_geo);
        std::exit(1);
    }

    double lon_rad = centerLon * DEG_TO_RAD;
    double lat_rad = centerLat * DEG_TO_RAD;

    double x_center = lon_rad;
    double y_center = lat_rad;
    double z_center = 0.0;

    if (pj_transform(pj_geo, pj_utm, 1, 0, &x_center, &y_center, &z_center)) {
        std::fprintf(stderr, "Error transforming center point\n");
        pj_free(pj_geo);
        pj_free(pj_utm);
        std::exit(1);
    }
    double easting_center = x_center;
    double northing_center = y_center;

    double x_dx = (centerLon + 0.0001) * DEG_TO_RAD;
    double y_dx = centerLat * DEG_TO_RAD;
    double z_dx = 0.0;
    if (pj_transform(pj_geo, pj_utm, 1, 0, &x_dx, &y_dx, &z_dx)) {
        std::fprintf(stderr, "Error transforming dx point\n");
        pj_free(pj_geo);
        pj_free(pj_utm);
        std::exit(1);
    }
    double metersPerDegreeLon = (x_dx - easting_center) / 0.0001;

    double x_dy = centerLon * DEG_TO_RAD;
    double y_dy = (centerLat + 0.0001) * DEG_TO_RAD;
    double z_dy = 0.0;
    if (pj_transform(pj_geo, pj_utm, 1, 0, &x_dy, &y_dy, &z_dy)) {
        std::fprintf(stderr, "Error transforming dy point\n");
        pj_free(pj_geo);
        pj_free(pj_utm);
        std::exit(1);
    }
    double metersPerDegreeLat = (y_dy - northing_center) / 0.0001;

    xResolutionDeg *= metersPerDegreeLon;
    yResolutionDeg *= metersPerDegreeLat;

    pj_free(pj_geo);
    pj_free(pj_utm);
}
