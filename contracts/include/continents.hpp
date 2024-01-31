/*/ 

This header contains efficient enumerations of continents to use in C++ contracts for WAX blockchains.

Enums are not explicitly numbered because the UN does not produce standardizes numbers for ISO 3166 nor M49 (Antarctica has no code)

/*/


enum class Continents {
    AFRICA,
    ANTARCTICA,
    ASIA,
    EUROPE,
    NORTH_AMERICA,
    OCEANIA,
    SOUTH_AMERICA
};


enum class Regions_M49 {
    AFRICA,
    AMERICAS,
    ANTARCTICA,
    ASIA,
    EUROPE,
    OCEANIA
};

enum class Continent_Subregions {
    WORLD,                  // World
    SOUTHERN_ASIA,          // Southern Asia
    EASTERN_ASIA,           // Eastern Asia
    SOUTH_EASTERN_ASIA,     // South-eastern Asia
    EASTERN_AFRICA,         // Eastern Africa
    SOUTH_AMERICA,          // South America
    WESTERN_AFRICA,         // Western Africa
    NORTHERN_AMERICA,       // Northern America
    EASTERN_EUROPE,         // Eastern Europe
    WESTERN_ASIA,           // Western Asia
    NORTHERN_AFRICA,        // Northern Africa
    WESTERN_EUROPE,         // Western Europe
    MIDDLE_AFRICA,          // Middle Africa
    CENTRAL_AMERICA,        // Central America
    SOUTHERN_EUROPE,        // Southern Europe
    NORTHERN_EUROPE,        // Northern Europe
    CENTRAL_ASIA,           // Central Asia
    SOUTHERN_AFRICA,        // Southern Africa
    CARIBBEAN,              // Caribbean
    AUSTRALIA_AND_NEW_ZEALAND, // Australia and New Zealand
    MELANESIA,              // Melanesia
    POLYNESIA,              // Polynesia
    MICRONESIA,             // Micronesia
    ANTARCTICA              // Antarctica
};

