#include <iostream>
#include <gdal.h>
#include <cpl_conv.h>
#include <gdal_priv.h>
#include <dirent.h>
#include <vector>
#include <string>
#include "ResolutionToMeters.h"

using namespace std;

vector<string> getTiffFiles(const string& folderPath) {
    vector<string> tiffFiles;
    DIR *dir;
    struct dirent *ent;

    if ((dir = opendir(folderPath.c_str())) != nullptr) {
        while ((ent = readdir(dir)) != nullptr) {
            string filename = ent->d_name;
            if (filename.size() > 4 &&
                (filename.substr(filename.size() - 4) == ".tif" ||
                 filename.substr(filename.size() - 4) == ".TIF")) {
                tiffFiles.push_back(folderPath + "/" + filename);
            }
        }
        closedir(dir);
    } else {
        cerr << "Could not open directory: " << folderPath << endl;
    }
    return tiffFiles;
}

void processTiff(const string& inputPath, const string& gdalwarp) {
    GDALAllRegister();
    string outputTiffPath = inputPath.substr(0, inputPath.length() - 4) + "_utm.tif";
    CPLSetConfigOption("GDAL_FILENAME_IS_UTF8", "NO");

    GDALDataset *pDataSet = (GDALDataset *)GDALOpen(inputPath.c_str(), GA_ReadOnly);
    if (!pDataSet) {
        cerr << "Failed to open: " << inputPath << endl;
        return;
    }

    int width = pDataSet->GetRasterXSize();
    int height = pDataSet->GetRasterYSize();
    double adfgeotransform[6];
    pDataSet->GetGeoTransform(adfgeotransform);
    double centerLon = adfgeotransform[0] + width / 2 * adfgeotransform[1];
    double centerLat = adfgeotransform[3] + height / 2 * adfgeotransform[5];

    ResolutionToMeters* RtoM;
    int utmZone = RtoM->CalculateUTMZone(centerLon, centerLat);
    bool isNorthHemisphere = (centerLat >= 0.0);

    string EPSG_NS;
    if (isNorthHemisphere) {
        EPSG_NS = "326" + to_string(utmZone);
    } else {
        EPSG_NS = "327" + to_string(utmZone);
    }

    cout << "Processing: " << inputPath << endl;
    cout << "Output path: " << outputTiffPath << endl;

    string command = gdalwarp + " -overwrite -t_srs " + "EPSG:" + EPSG_NS +
                     " -tr 100 100 " + inputPath + " " + outputTiffPath;
    system(command.c_str());

    GDALClose(pDataSet);
}

void work(const char *folderPath, string gdalwarp) {

    vector<string> tiffFiles = getTiffFiles(folderPath);
    if (tiffFiles.empty()) {
        cout << "No TIFF files found in: " << folderPath << endl;
        return;
    }

    for (const auto& tiffFile : tiffFiles) {
        processTiff(tiffFile, gdalwarp);
    }
}

int main(int argc, char *argv[]) {
    if(argc <= 2){
        std::cout << "=========================== gdalwarp Use Method ========================================" << std::endl;
        std::cout << "   : .exe -tif [tifFilePath]" << std::endl;
        std::cout << "   : .exe -path [dirPath]" << std::endl;
        return 0;
    }
    cout << "TIFF Batch Processor" << endl;
    string VRS_MAIN = getenv("MAIN");
    string gdal_data_dir = VRS_MAIN + "/gdal/share/gdal";
    string gdalwarp = VRS_MAIN + "/gdal/bin/gdalwarp";
    string proj_data_dir = VRS_MAIN + "/proj/share/proj";
    string proj_lib = VRS_MAIN + "/proj/lib";

    setenv("PROJ_LIB", proj_data_dir.c_str(), 1);
    const char* old_ld_path = getenv("LD_LIBRARY_PATH");
    string new_ld_path = (old_ld_path != nullptr) ?
                         (proj_lib + ":" + old_ld_path) : proj_lib;
    setenv("LD_LIBRARY_PATH", new_ld_path.c_str(), 1);
    setenv("GDAL_DATA", gdal_data_dir.c_str(), 1);
    string option = argv[1];
    if (option == "-tif") {
        processTiff(argv[2], gdalwarp);
    } else if (option == "-path"){
        work(argv[2], gdalwarp);
    }
    return 0;
}