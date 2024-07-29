

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
/**std::pair<double, double> calculateAzimuthElevation(const libsgp4::Eci& eci) {
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
}*/

std::pair<double, double> calculateAzimuthElevation(const libsgp4::Eci& eci, const libsgp4::CoordGeodetic& observer_geo) {
    // Create an observer at the antenna location
    libsgp4::Observer observer(observer_geo);

    // Get the look angle from the observer to the satellite
    libsgp4::CoordTopocentric topo = observer.GetLookAngle(eci);

    // Convert to degrees
    double az_deg = topo.azimuth * RAD_TO_DEG;
    double el_deg = topo.elevation * RAD_TO_DEG;

    return std::make_pair(az_deg, el_deg);
}


int main() {
    // TLE data 
    std::string line1 = "1 55821U 23029AB  24201.16146330  .00000300  00000+0  77384-3 0  9994";
    std::string line2 = "2 55821  87.8941  68.6073 0002168  71.0045 289.1320 13.14501882 68376";


    // Create satellite object
    libsgp4::Tle Tle(line1, line2);

    //Create Observer object
    libsgp4::CoordGeodetic observer_geo(ANTENNA_LAT, ANTENNA_LON, ANTENNA_ALT);

    // Use specific epoch (J2000.0)
    libsgp4::DateTime epoch = Tle.Epoch();

    //Create SGP4 propagator
    libsgp4::SGP4 sgp4(Tle);

    // Get current time
    auto now = std::chrono::system_clock::now();
    auto now_time_t = std::chrono::system_clock::to_time_t(now);
    std::tm* now_tm = std::gmtime(&now_time_t);  // Use gmtime for UTC

    // Calculate and print azimuth and elevation for the next hour
    for (int i = 0; i < 120; ++i) {
        // Calculate Julian Date
        //libsgp4::DateTime dt(now_tm->tm_year + 1900, now_tm->tm_mon + 1, now_tm->tm_mday,
        //            now_tm->tm_hour, now_tm->tm_min, now_tm->tm_sec);
        libsgp4::DateTime dt = epoch.AddMinutes(i);
        //double jd = dt.ToJulian();

        // Get satellite position
        libsgp4::Eci eci = sgp4.FindPosition(dt);

        // Convert satellite position to geodetic coordinates
        libsgp4::CoordGeodetic geo = eci.ToGeodetic();

        // Calculate azimuth and elevation
        auto [az, el] = calculateAzimuthElevation(eci, observer_geo);

        // Print results
        std::cout << "Time since epoch: " << i << " minutes\n";
        std::cout << "Time: " << std::put_time(now_tm, "%Y-%m-%d %H:%M:%S") << " UTC\n";
        std::cout << "Azimuth: " << std::fixed << std::setprecision(2) << az << "°, Elevation: " << el << "°\n\n";
        std::cout << "Satellite Lat: " << eci.ToGeodetic().latitude << "°, Lon: " << eci.ToGeodetic().longitude << "°, Alt: " << eci.ToGeodetic().altitude << " km\n\n";
        
        // Increment time by 1 minute
        now_time_t += 60;  // Add 60 seconds
        now_tm = std::gmtime(&now_time_t);  // Update now_tm with new time
    }

    return 0;
}
