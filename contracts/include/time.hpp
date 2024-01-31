/*/ 

This header contains common time spans stored as enumerated lists to use in C++ contracts for WAX blockchains.

/*/

enum class In {
    /*/ --- Gregorian Times --- /*/
    Second = 1,                  // Second
    Minute = 60,                 // Minute
    FiveMinutes = 300,           // 5 Minutes
    TenMinutes = 600,            // 10 Minutes
    FifteenMinutes = 900,        // 15 Minutes
    Hour = 3600,                 // Hour
    Day = 86400,                 // Day
    Week = 604800,               // Week
    Month = 2592000,             // Month (approximated as 30 days)
    Year = 31536000              // Year (approximated as 365 days, ignoring leap years)

    /*/ --- Celestial Orbits --- /*/
    MercuryOrbit = 4976640,     // One orbit of Mercury around the Sun (approximated as 57.6 days)
    VenusOrbit = 21012480,      // One orbit of Venus around the Sun (approximated as 243.2 days)
    MarsOrbit = 59318780,        // One orbit of Mars around the Sun (approximated as 686.56 days)
    JupiterOrbit = 374371200,   // One orbit of Jupiter around the Sun (approximated as 4,333 days)
    SaturnOrbit = 929577600,    // One orbit of Saturn around the Sun (approximated as 30,687 days)
    UranusOrbit = 2651357000,    // One orbit of Uranus around the Sun (approximated as 30687 days)
    NeptuneOrbit = 5200416000,  // One orbit of Neptune around the Sun (approximated as 60190 days)
    PlutoOrbit = 7824384000,     // One orbit of Pluto around the Sun (approximated as 90560 days)
    AsteroidBeltOrbit = 12614400 // Approx orbit of an asteroid in the main belt (approximated as 1460 days )

    /*/ --- Celestial Days in Seconds --- /*/
    MercuryDay = 5063040,   // One day on Mercury (approximated as 58.6 Earth days)
    VenusDay = 20995200,    // One day on Venus (approximated as 243 Earth days)
    MarsDay = 88560,        // One day on Mars (approximated as 24.6 hours)
    JupiterDay = 35640,     // One day on Jupiter (approximated as 9.9 hours)
    SaturnDay = 38520,      // One day on Saturn (approximated as 10.7 hours)
    UranusDay = 61920,      // One day on Uranus (approximated as 17.2 hours)
    NeptuneDay = 57960,     // One day on Neptune (approximated as 16.1 hours)

};
