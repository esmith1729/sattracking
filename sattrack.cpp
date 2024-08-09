

/***
 * This file implements an algorithm which takes a GPS location, 
 * a satellite's TLE, and a datetime as input, and outputs a look 
 * angle direction between the GPS location and the satellite's 
 * position in its orbit
 */

#include <fstream>
#include <string>
#include <chrono>
#include <vector>
#include "SGP4\libsgp4\SGP4.h"
#include "SGP4\libsgp4\Eci.h"
#include "SGP4\libsgp4\CoordTopocentric.h"
#include "SGP4\libsgp4\CoordGeodetic.h"
#include "SGP4\libsgp4\Observer.h"

std::vector<int> getLineNumbers(std::string fileName) {
    int lineCount = 0;
    std::string line;
    std::vector<int> result;

    std::ifstream file(fileName);
    if(!file.is_open()) {
        std::cerr << "Unable to open file" << std::endl;
    }
    while (std::getline(file, line)) {
        lineCount++;
    }
    std::cout<< lineCount << std::endl;
    return result;
}

std::vector<libsgp4::Tle> createTLEArray(const std::vector<int>& lineNumbers, std::ifstream file) {
    std::vector<libsgp4::Tle> result;

    return result;
}

int main() {
    getLineNumbers("TLE.txt");
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
    
    // TLE data & Create satellite object
    libsgp4::Tle Tle(line1, line2);

    // Print out TLE information to terminal
    std::cout << Tle << std::endl;
 
    //Create Observer object
    libsgp4::Observer obs(51.507406923983446, -0.12773752212524414, 0.05);

    // Use specific epoch (J2000.0)
    libsgp4::DateTime epoch = Tle.Epoch();

    //Create SGP4 propagator
    libsgp4::SGP4 sgp4(Tle);

    // Get current time
    auto now = std::chrono::system_clock::now();
    auto now_time_t = std::chrono::system_clock::to_time_t(now);
    std::tm* now_tm = std::gmtime(&now_time_t);  // Use gmtime for UTC

    // Calculate and print azimuth and elevation for the next 900 mins
    for (int i = 0; i < 900; ++i) {
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
        
        //std::cout << "Time: " << std::put_time(now_tm, "%Y-%m-%d %H:%M:%S") << " UTC\n";
        //std::cout << "Epoch Time: " << epoch << "\n";
        if (topo.elevation > 0.436) { // If the Elevation Angle is above 25 degrees
            std::cout << "Time at Observer: " << dt << std::endl;
            std::cout << topo << std::endl;
            std::cout << geo << " " << std::endl;
        }
        
        // Increment time by 1 minute
        now_time_t += 60;  // Add 60 second
        now_tm = std::gmtime(&now_time_t);  // Update now_tm with new time
    }

    return 0;
}
