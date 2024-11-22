

/***
 * This file implements an algorithm which takes a GPS location, 
 * a satellite's TLE, and a datetime as input, and outputs a look 
 * angle direction between the GPS location and the satellite's 
 * position in its orbit
 */

#include <fstream>
#include <string>
#include <cstring>
#include <chrono>
#include <vector>
#include "SGP4\libsgp4\SGP4.h"
#include "SGP4\libsgp4\Tle.h"
#include "SGP4\libsgp4\Eci.h"
#include "SGP4\libsgp4\CoordTopocentric.h"
#include "SGP4\libsgp4\CoordGeodetic.h"
#include "SGP4\libsgp4\Observer.h"

std::vector<libsgp4::Tle> createTLEVector(std::string fileName) {
    std::string line;
    std::vector<libsgp4::Tle> result;
    std::string one = "1";
    std::string two = "2";
    std::string line1;
    std::string line2;

    std::ifstream file(fileName);
    if(!file.is_open()) {
        std::cerr << "Unable to open file" << std::endl;
    }
    while (std::getline(file, line)) {
        if(line[0] == one[0]) {
            line1 = line;
        }
        else if(line[0] == two[0]) {
            line2 = line;
            libsgp4::Tle Tle(line1, line2);
            result.emplace_back(Tle);
        }
        else {
            line1 = "";
            line2 = "";
            continue;
        }
    }    
    return result;
}

libsgp4::Observer getGPSdata() {
    std::string line;
    char* element;
    std::string latitude;
    std::string longitute;
    std::string altitude;
    std::string gpgga = "$GPGGA";

    std::ifstream file("output.nmea");
    if(!file.is_open()) {
        std::cerr << "Unable to open file" << std::endl;
    }
    while (std::getline(file, line)) {
        if(line[4] == 'G') {
            auto fields = strtok(element,",");
        }
    }    
}

int main() {
    int observableSatellites = 0;
    //Get TLEs from TLE.txt and put into Array
    std::vector<libsgp4::Tle> TleVector = createTLEVector("TLE.txt");
    //Create Observer object
    libsgp4::Observer obs(51.507406923983446, -0.12773752212524414, 0.05);
    
    for(auto const &tle : TleVector) {    
        
        // Use specific epoch (J2000.0)
        libsgp4::DateTime epoch = tle.Epoch();

        //Create SGP4 propagator
        libsgp4::SGP4 sgp4(tle);

        // Get current time
        auto now = std::chrono::system_clock::now();
        auto now_time_t = std::chrono::system_clock::to_time_t(now);
        std::tm* now_tm = std::gmtime(&now_time_t);  // Use gmtime for UTC

        // Calculate and print azimuth and elevation for all TLEs with elevation above 25 degrees
        // Calculate Julian Date
        libsgp4::DateTime dt(now_tm->tm_year + 1900, now_tm->tm_mon + 1, now_tm->tm_mday,
                    now_tm->tm_hour, now_tm->tm_min, now_tm->tm_sec);

        // Get satellite position
        libsgp4::Eci eci = sgp4.FindPosition(dt);

        // Get look angles, azimuth and elevation
        libsgp4::CoordTopocentric topo = obs.GetLookAngle(eci);

        // Convert satellite position to geodetic coordinates
        libsgp4::CoordGeodetic geo = eci.ToGeodetic();

        // Print results
            if (topo.elevation > 0.436) { // If the Elevation Angle is above 25 degrees
                std::cout << tle << std::endl;
                std::cout << "Time at Observer: " << dt << std::endl;
                std::cout << topo << std::endl;
                std::cout << std::endl;
                observableSatellites++;
            }
    }
    std::cout << "Number of Observerable satellites: " << observableSatellites << std::endl;

    return 0;
}
