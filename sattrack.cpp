

/***
 * This file implements an algorithm which takes a GPS location, 
 * a satellite's TLE, and a datetime as input, and outputs a look 
 * angle direction between the GPS location and the satellite's 
 * position in its orbit
 */

#include <iostream>
#include <iomanip>
#include <cmath>
#include <chrono>
#include </home/esmith/Projects/sgp4/libsgp4/SGP4.h>
#include </home/esmith/Projects/sgp4/libsgp4/Eci.h>
#include </home/esmith/Projects/eigen/Eigen/Dense>
#include "/home/esmith/Projects/sgp4/libsgp4/CoordTopocentric.h"
#include "/home/esmith/Projects/sgp4/libsgp4/CoordGeodetic.h"
#include "/home/esmith/Projects/sgp4/libsgp4/Observer.h"

// Constants
const double PI = 3.14159265358979323846;
const double DEG_TO_RAD = PI / 180.0;
const double RAD_TO_DEG = 180.0 / PI;

// Antenna location (example: Oxford, England)
const double ANTENNA_LAT = 51.7520;  // degrees North
const double ANTENNA_LON = -1.2577;  // degrees East
const double ANTENNA_ALT = 0.065;    // km above sea level

// Antenna operating frequency
const double FREQ_GHZ = 20.0;  // GHz

// Earth parameters
const double EARTH_RADIUS = 6378.137;  // km
const double EARTH_FLATTENING = 1.0 / 298.257223563;

// Function to calculate azimuth and elevation
std::pair<double, double> calculateAzimuthElevation(const libsgp4::Eci& eci) {
    // Convert antenna location to ECEF
    double phi = ANTENNA_LAT * DEG_TO_RAD;
    double lambda = ANTENNA_LON * DEG_TO_RAD;
    double h = ANTENNA_ALT;
    double a = EARTH_RADIUS;
    double f = EARTH_FLATTENING;

    double N = a / std::sqrt(1 - f * (2 - f) * std::pow(std::sin(phi), 2));
    double ant_x = (N + h) * std::cos(phi) * std::cos(lambda);
    double ant_y = (N + h) * std::cos(phi) * std::sin(lambda);
    double ant_z = ((1 - f) * (1 - f) * N + h) * std::sin(phi);

    // Calculate topocentric coordinates
    double rx = eci.Position().x - ant_x;
    double ry = eci.Position().y - ant_y;
    double rz = eci.Position().z - ant_z;

    // Convert to local tangent coordinates
    double sin_lat = std::sin(phi);
    double cos_lat = std::cos(phi);
    double sin_lon = std::sin(lambda);
    double cos_lon = std::cos(lambda);

    double top_s = sin_lat * cos_lon * rx + sin_lat * sin_lon * ry - cos_lat * rz;
    double top_e = -sin_lon * rx + cos_lon * ry;
    double top_z = cos_lat * cos_lon * rx + cos_lat * sin_lon * ry + sin_lat * rz;

    // Calculate azimuth and elevation
    double az = std::atan2(-top_e, top_s) + PI;
    double el = std::asin(top_z / std::sqrt(top_s * top_s + top_e * top_e + top_z * top_z));

    // Convert to degrees
    double az_deg = az * RAD_TO_DEG;
    double el_deg = el * RAD_TO_DEG;

    return std::make_pair(az_deg, el_deg);
}


int main() {
    // TLE data for ISS
    //std::string line1 = "1 25544U 98067A   20344.52777778  .00016717  00000-0  10270-3 0  9000";
    //std::string line2 = "2 25544  51.6442  21.5556 0002000  0.0000  0.0000 15.49120000  9000";
    std::string line1 = "1 47281U 20100Z   24198.85747037  .00000008  00000+0 -12866-4 0  9995";
    std::string line2 = "2 47281  87.8956 130.1349 0001887  62.3860 297.7459 13.12446009172875";


    // Create satellite object
    libsgp4::Tle Tle(line1, line2);

    // Get current time
    auto now = std::chrono::system_clock::now();
    std::time_t now_c = std::chrono::system_clock::to_time_t(now);
    struct tm* timeinfo = std::gmtime(&now_c);

    // Calculate and print azimuth and elevation for the next hour
    for (int i = 0; i < 60; ++i) {
        // Calculate Julian Date
        libsgp4::DateTime dt(timeinfo->tm_year + 1900, timeinfo->tm_mon + 1, timeinfo->tm_mday,
                    timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);
        double jd = dt.ToJulian();

        // Get satellite position
        libsgp4::SGP4 sgp4(Tle);
        libsgp4::Eci eci = sgp4.FindPosition(jd);

        // Calculate azimuth and elevation
        auto [az, el] = calculateAzimuthElevation(eci);

        // Print results
        std::cout << "Time: " << std::put_time(timeinfo, "%Y-%m-%d %H:%M:%S") << " UTC\n";
        std::cout << "Azimuth: " << std::fixed << std::setprecision(2) << az << "°, Elevation: " << el << "°\n\n";

        // Increment time by 1 minute
        timeinfo->tm_min += 1;
        std::mktime(timeinfo);
    }

    return 0;
}
