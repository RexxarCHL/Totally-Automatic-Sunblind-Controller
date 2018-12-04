#include "tasc.h"

char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

// Define the latitude and longitude of Baltimore for sunrise/sunset calculation
// Data from https://www.latlong.net/place/baltimore-md-usa-1882.html
const float LAT = 39.299236;
const float LONG = -76.609383;

void setup_rtc() {
    Serial.println("Initializing RTC...");

    if (! rtc.begin()) {
        Serial.println("Couldn't find RTC");
        while (1);
    }

    if (! rtc.initialized()) {
        Serial.println("RTC is NOT running!");
        // following line sets the RTC to the date & time this sketch was compiled
        rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
        // This line sets the RTC with an explicit date & time, for example to set
        // January 21, 2014 at 3am you would call:
        // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
    }

    Serial.println("RTC initialized");
}

DateTime get_current_time() {
    /* Use now.year(), now.month(), now.day(), now.hour(), now.minute(), now.second(), now.unixtime()
     * to access the time
     */
    DateTime now = rtc.now();
    return now;
}

void adjust_rtc_time(int year, int month, int day, int hour, int minute, int second) {
    rtc.adjust(DateTime(year, month, day, hour, minute, second));
}

bool is_night(DateTime now) {
    /* Sunrise/sunset calculation method from 
     * https://www.r-bloggers.com/approximate-sunrise-and-sunset-times/
     */

    // Data needed for sunrise/sunset calculation
    const float d = (float) date_to_days(now.month(), now.day()); // Get the number of days from the start of the year
    const float R = 6378.0; // The radius of the earth, in km
    const float r = 149598000; // The distance to the sun, in km
    const float epsilon = degrees_to_radians(23.45); // Radians between the xy-plane and the ecliptic plane
    const float L = degrees_to_radians(LAT); // Convert observer's latitude to radians

    int long_sign = (LONG > 0)? 1 : -1;
    int timezone = -4 * ((int)abs(LONG) % 15) * long_sign;

    float theta = 2 * M_PI / 365.25 * (d - 80);
    float z_s = r * sin(theta) * sin(epsilon);
    float r_p = sqrt(pow(r, 2) - pow(z_s, 2));
    float t0 = 1440 / (2 * M_PI) * acos((R - z_s*sin(L)) / (r_p * cos(L)));

    // a kludge adjustment for the radius of the sun
    float that = t0 + 5.0;

    // Adjust "noon" for the fact that the earth's orbit is not circular:
    float n = 720 - 10*sin(4*M_PI*(d-80) / 365.25) + 8*sin(2*M_PI*d / 365.25);

    // now sunrise and sunset are:
    float sunrise = (n - that + timezone) / 60;
    float sunset = (n + that + timezone) / 60;

    if (now.hour() > sunset)
        return true; // After sunset
    else if (now.hour() < sunrise)
        return true; // Before sunrise

    return false; // After sunrise and before sunset
}

float degrees_to_radians(float degrees) {
    return M_PI * degrees / 180.0;
}

int date_to_days(int month, int day) {
    int days = day;
    // month -= 1; // Don't count the current month
    while (--month > 0) {
        if (month == 2)
            days += 28;
        else if (month == 1 || month == 3 || month == 7 || 
                 month == 8 || month == 10 || month == 12)
            days += 31;
        else
            days += 30;
    };

    return days;
}