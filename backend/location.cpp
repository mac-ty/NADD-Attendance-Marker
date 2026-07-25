#include "location.h"
#include <cmath>


static const double PI = 3.14159265358979323846;

double calculateDistance(double lat1, double lng1, double lat2, double lng2) {
    const double earthRadius = 6371000;

    double lat1Rad = lat1 * PI / 180.0;
    double lat2Rad = lat2 * PI / 180.0;
    double deltaLat = (lat2 - lat1) * PI / 180.0;
    double deltaLng = (lng2 - lng1) * PI / 180.0;

    double a = sin(deltaLat / 2) * sin(deltaLat / 2) + 
                cos(lat1Rad) * cos(lat2Rad) * 
                sin(deltaLng / 2) * sin(deltaLng / 2);
    
    double c =  2 * atan2(sqrt(a), sqrt(1 - a));

    double R = earthRadius * c;

    return R;
}