

/***
 * This file implements an algorithm which takes a GPS location, 
 * a satellite's TLE, and a datetime as input, and outputs a look 
 * angle direction between the GPS location and the satellite's 
 * position in its orbit
 */

#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>
#include <cmath>
#include <chrono>
#include <C:/Users/smith/Projects/sgp4/libsgp4/SGP4.h>
#include <C:/Users/smith/Projects/sgp4/libsgp4/Eci.h>
#include <C:/Program Files/Eigen/eigen-3.4.0/Eigen/Dense>
#include "C:/Users/smith/Projects/sgp4/libsgp4/CoordTopocentric.h"
#include "C:/Users/smith/Projects/sgp4/libsgp4/CoordGeodetic.h"
#include "C:/Users/smith/Projects/sgp4/libsgp4/Observer.h"

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
    //Get TLE from TLE.txt
    std::ifstream file("TLE.txt");
    if(!file.is_open()) {
        std::cerr << "Unable to open TLE file" << std::endl;
        return 1;
    }
    std::string line1 = "";
    std::string line2 = "";
    std::string line;
    int lineNumber = 0;

    while(std::getline(file, line)) {
        lineNumber++;
        if (lineNumber ==2) {
            line1 = line;
        } else if (lineNumber == 3) {
            line2 = line;
        }
    }
    file.close();
    
    // TLE data & 
    // Create satellite object
    libsgp4::Tle Tle(line1, line2);

    std::cout << Tle << std::endl;
 
    //Create Observer object
    libsgp4::CoordGeodetic observer_geo(ANTENNA_LAT, ANTENNA_LON, ANTENNA_ALT);
    libsgp4::Observer obs(51.507406923983446, -0.12773752212524414, 0.05);

    // Use specific epoch (J2000.0)
    libsgp4::DateTime epoch = Tle.Epoch();

    //Create SGP4 propagator
    libsgp4::SGP4 sgp4(Tle);

    // Get current time
    auto now = std::chrono::system_clock::now();
    auto now_time_t = std::chrono::system_clock::to_time_t(now);
    std::tm* now_tm = std::gmtime(&now_time_t);  // Use gmtime for UTC

    // Calculate and print azimuth and elevation for the next hour
    for (int i = 0; i < 900; ++i) {
        // Calculate Julian Date
        libsgp4::DateTime dt(now_tm->tm_year + 1900, now_tm->tm_mon + 1, now_tm->tm_mday,
                    now_tm->tm_hour, now_tm->tm_min, now_tm->tm_sec);
        
        double jd = dt.ToJulian();

        // Get satellite position
        libsgp4::Eci eci = sgp4.FindPosition(dt);

        // Get look angle
        libsgp4::CoordTopocentric topo = obs.GetLookAngle(eci);

        // Convert satellite position to geodetic coordinates
        libsgp4::CoordGeodetic geo = eci.ToGeodetic();

        // Calculate azimuth and elevation
        //auto [az, el] = calculateAzimuthElevation(eci, observer_geo);

        // Print results
        
        //std::cout << "Time: " << std::put_time(now_tm, "%Y-%m-%d %H:%M:%S") << " UTC\n";
        //std::cout << "Epoch Time: " << epoch << "\n";
        if (topo.elevation > 0.436) { // If the Elevation Angle is above 25 degrees
            std::cout << "Time at Observer: " << dt << std::endl;
            std::cout << topo << std::endl;
            std::cout << geo << " " << std::endl;
        }
        
        //std::cout << "Azimuth: " << std::fixed << std::setprecision(2) << az << "°, Elevation: " << el << "°\n\n";
        //std::cout << "Satellite Lat: " << eci.ToGeodetic().latitude << "°, Lon: " << eci.ToGeodetic().longitude << "°, Alt: " << eci.ToGeodetic().altitude << " km\n\n";
        
        // Increment time by 1 minute
        now_time_t += 60;  // Add 60 second
        now_tm = std::gmtime(&now_time_t);  // Update now_tm with new time
    }

    return 0;
}
