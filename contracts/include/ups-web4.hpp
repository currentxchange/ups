/*/

    These are just the Web4 constants we are using

/*/



    // --- Series Constants --- //
    namespace series {
    
    const std::vector<uint32_t> FIBONACCI = {1, 2, 3, 5, 8, 13, 21, 34, 55, 89, 144, 233, 377, 610, 987, 1597, 2584, 4181, 6765, 10946, 17711, 28657, 4294967295};
    const std::vector<uint32_t> SILVER = {1, 2, 5, 12, 29, 70, 169, 408, 985, 2378, 5741, 13860, 33461, 80782, 195025, 470832, 1136689, 2744210, 6625109, 15994428, 38613965, 93222358, 225058681, 54333972, 4294967295};

    const std::vector<uint32_t> TETRAHEDRAL = {1, 4, 10, 20, 35, 56, 84, 120, 165, 220, 286, 364, 455, 560, 680, 816, 969, 1140, 1330, 1540, 1771, 2024, 2300, 2600, 4294967295};
    const std::vector<uint32_t> OCTAHEDRAL = {1, 6, 19, 44, 85, 146, 231, 344, 489, 670, 891, 1156, 1471, 1842, 2275, 2776, 3351, 4006, 4747, 5580, 6511, 7546, 8691, 9952, 4294967295};
    const std::vector<uint32_t> HEXAHEDRAL = {1, 8, 27, 64, 125, 216, 343, 512, 729, 1000, 1331, 1728, 2197, 2744, 3375, 4096, 4913, 5832, 6859, 8000, 9261, 10648, 12167, 13824, 4294967295};
    const std::vector<uint32_t> ICOSAHEDRAL = {1, 12, 42, 92, 162, 252, 362, 492, 642, 812, 1002, 1212, 1442, 1692, 1962, 2252, 2562, 2892, 3242, 3612, 4002, 4412, 4842, 5292, 4294967295};
    const std::vector<uint32_t> DODECAHEDRAL = {1, 20, 84, 220, 455, 812, 1330, 2024, 2925, 4060, 5456, 7140, 9141, 11480, 14156, 17264, 20899, 25056, 29830, 35216, 41219, 47844, 55196, 63280, 4294967295};

    const std::vector<uint32_t> LUCAS = {2, 1, 3, 4, 7, 11, 18, 29, 47, 76, 123, 199, 322, 521, 843, 1364, 2207, 3571, 5778, 9349, 15127, 24476, 39603, 64079, 4294967295};
    const std::vector<uint32_t> TRIANGULAR = {1, 3, 6, 10, 15, 21, 28, 36, 45, 55, 66, 78, 91, 105, 120, 136, 153, 171, 190, 210, 231, 253, 276, 300, 4294967295};
    const std::vector<uint32_t> SQUARE = {1, 4, 9, 16, 25, 36, 49, 64, 81, 100, 121, 144, 169, 196, 225, 256, 289, 324, 361, 400, 441, 484, 529, 576, 4294967295};
    const std::vector<uint32_t> PENTAGONAL = {1, 5, 12, 22, 35, 51, 70, 92, 117, 145, 176, 210, 247, 287, 330, 376, 425, 477, 532, 590, 651, 715, 782, 852, 4294967295};
    const std::vector<uint32_t> HEXAGONAL = {1, 6, 15, 28, 45, 66, 91, 120, 153, 190, 231, 276, 325, 378, 435, 496, 561, 630, 703, 780, 861, 946, 1035, 1128, 4294967295};

    }


    // --- Return a Series --- //
    vector<uint32_t> getSeries(const string& seriesName){
        if (seriesName == "FIBONACCI") {
            return series::FIBONACCI;
        } else if (seriesName == "SILVER") {
            return series::SILVER;
        } else if (seriesName == "TETRAHEDRAL") {
            return series::TETRAHEDRAL;
        } else if (seriesName == "OCTAHEDRAL") {
            return series::OCTAHEDRAL;
        } else if (seriesName == "HEXAHEDRAL") {
            return series::HEXAHEDRAL;
        } else if (seriesName == "ICOSAHEDRAL") {
            return series::ICOSAHEDRAL;
        } else if (seriesName == "DODECAHEDRAL") {
            return series::DODECAHEDRAL;
        } else if (seriesName == "LUCAS") {
            return series::LUCAS;
        } else if (seriesName == "TRIANGULAR") {
            return series::TRIANGULAR;
        } else if (seriesName == "SQUARE") {
            return series::SQUARE;
        } else if (seriesName == "PENTAGONAL") {
            return series::PENTAGONAL;
        } else if (seriesName == "HEXAGONAL") {
            return series::HEXAGONAL;
        } else {
            return std::vector<uint32_t>();
        }
            
    }//END getSeries()



enum class Countries_ISO3 {
    AFG = 4,    // Afghanistan
    ALA = 248,  // Åland Islands
    ALB = 8,    // Albania
    DZA = 12,   // Algeria
    ASM = 16,   // American Samoa
    AND = 20,   // Andorra
    AGO = 24,   // Angola
    AIA = 660,  // Anguilla
    ATA = 10,   // Antarctica
    ATG = 28,   // Antigua and Barbuda
    ARG = 32,   // Argentina
    ARM = 51,   // Armenia
    ABW = 533,  // Aruba
    AUS = 36,   // Australia
    AUT = 40,   // Austria
    AZE = 31,   // Azerbaijan
    BHS = 44,   // Bahamas
    BHR = 48,   // Bahrain
    BGD = 50,   // Bangladesh
    BRB = 52,   // Barbados
    BLR = 112,  // Belarus
    BEL = 56,   // Belgium
    BLZ = 84,   // Belize
    BEN = 204,  // Benin
    BMU = 60,   // Bermuda
    BTN = 64,   // Bhutan
    BOL = 68,   // Bolivia (Plurinational State of)
    BES = 535,  // Bonaire, Sint Eustatius and Saba
    BIH = 70,   // Bosnia and Herzegovina
    BWA = 72,   // Botswana
    BVT = 74,   // Bouvet Island
    BRA = 76,   // Brazil
    IOT = 86,   // British Indian Ocean Territory
    VGB = 92,   // British Virgin Islands
    BRN = 96,   // Brunei Darussalam
    BGR = 100,  // Bulgaria
    BFA = 854,  // Burkina Faso
    BDI = 108,  // Burundi
    CPV = 132,  // Cabo Verde
    KHM = 116,  // Cambodia
    CMR = 120,  // Cameroon
    CAN = 124,  // Canada
    CYM = 136,  // Cayman Islands
    CAF = 140,  // Central African Republic
    TCD = 148,  // Chad
    CHL = 152,  // Chile
    CHN = 156,  // China
    HKG = 344,  // China, Hong Kong Special Administrative Region
    MAC = 446,  // China, Macao Special Administrative Region
    CXR = 162,  // Christmas Island
    CCK = 166,  // Cocos (Keeling) Islands
    COL = 170,  // Colombia
    COM = 174,  // Comoros
    COG = 178,  // Congo
    COK = 184,  // Cook Islands
    CRI = 188,  // Costa Rica
    CIV = 384,  // Côte d'Ivoire
    HRV = 191,  // Croatia
    CUB = 192,  // Cuba
    CUW = 531,  // Curaçao
    CYP = 196,  // Cyprus
    CZE = 203,  // Czechia
    PRK = 408,  // Democratic People's Republic of Korea
    COD = 180,  // Democratic Republic of the Congo
    DNK = 208,  // Denmark
    DJI = 262,  // Djibouti
    DMA = 212,  // Dominica
    DOM = 214,  // Dominican Republic
    ECU = 218,  // Ecuador
    EGY = 818,  // Egypt
    SLV = 222,  // El Salvador
    GNQ = 226,  // Equatorial Guinea
    ERI = 232,  // Eritrea
    EST = 233,  // Estonia
    SWZ = 748,  // Eswatini
    ETH = 231,  // Ethiopia
    FLK = 238,  // Falkland Islands (Malvinas)
    FRO = 234,  // Faroe Islands
    FJI = 242,  // Fiji
    FIN = 246,  // Finland
    FRA = 250,  // France
    GUF = 254,  // French Guiana
    PYF = 258,  // French Polynesia
    ATF = 260,  // French Southern Territories
    GAB = 266,  // Gabon
    GMB = 270,  // Gambia
    GEO = 268,  // Georgia
    DEU = 276,  // Germany
    GHA = 288,  // Ghana
    GIB = 292,  // Gibraltar
    GRC = 300,  // Greece
    GRL = 304,  // Greenland
    GRD = 308,  // Grenada
    GLP = 312,  // Guadeloupe
    GUM = 316,  // Guam
    GTM = 320,  // Guatemala
    GGY = 831,  // Guernsey
    GIN = 324,  // Guinea
    GNB = 624,  // Guinea-Bissau
    GUY = 328,  // Guyana
    HTI = 332,  // Haiti
    HMD = 334,  // Heard Island and McDonald Islands
    VAT = 336,  // Holy See
    HND = 340,  // Honduras
    HUN = 348,  // Hungary
    ISL = 352,  // Iceland
    IND = 356,  // India
    IDN = 360,  // Indonesia
    IRN = 364,  // Iran (Islamic Republic of)
    IRQ = 368,  // Iraq
    IRL = 372,  // Ireland
    IMN = 833,  // Isle of Man
    ISR = 376,  // Israel
    ITA = 380,  // Italy
    JAM = 388,  // Jamaica
    JPN = 392,  // Japan
    JEY = 832,  // Jersey
    JOR = 400,  // Jordan
    KAZ = 398,  // Kazakhstan
    KEN = 404,  // Kenya
    KIR = 296,  // Kiribati
    KWT = 414,  // Kuwait
    KGZ = 417,  // Kyrgyzstan
    LAO = 418,  // Lao People's Democratic Republic
    LVA = 428,  // Latvia
    LBN = 422,  // Lebanon
    LSO = 426,  // Lesotho
    LBR = 430,  // Liberia
    LBY = 434,  // Libya
    LIE = 438,  // Liechtenstein
    LTU = 440,  // Lithuania
    LUX = 442,  // Luxembourg
    MDG = 450,  // Madagascar
    MWI = 454,  // Malawi
    MYS = 458,  // Malaysia
    MDV = 462,  // Maldives
    MLI = 466,  // Mali
    MLT = 470,  // Malta
    MHL = 584,  // Marshall Islands
    MTQ = 474,  // Martinique
    MRT = 478,  // Mauritania
    MUS = 480,  // Mauritius
    MYT = 175,  // Mayotte
    MEX = 484,  // Mexico
    FSM = 583,  // Micronesia (Federated States of)
    MCO = 492,  // Monaco
    MNG = 496,  // Mongolia
    MNE = 499,  // Montenegro
    MSR = 500,  // Montserrat
    MAR = 504,  // Morocco
    MOZ = 508,  // Mozambique
    MMR = 104,  // Myanmar
    NAM = 516,  // Namibia
    NRU = 520,  // Nauru
    NPL = 524,  // Nepal
    NLD = 528,  // Netherlands (Kingdom of the)
    NCL = 540,  // New Caledonia
    NZL = 554,  // New Zealand
    NIC = 558,  // Nicaragua
    NER = 562,  // Niger
    NGA = 566,  // Nigeria
    NIU = 570,  // Niue
    NFK = 574,  // Norfolk Island
    MKD = 807,  // North Macedonia
    MNP = 580,  // Northern Mariana Islands
    NOR = 578,  // Norway
    OMN = 512,  // Oman
    PAK = 586,  // Pakistan
    PLW = 585,  // Palau
    PAN = 591,  // Panama
    PNG = 598,  // Papua New Guinea
    PRY = 600,  // Paraguay
    PER = 604,  // Peru
    PHL = 608,  // Philippines
    PCN = 612,  // Pitcairn
    POL = 616,  // Poland
    PRT = 620,  // Portugal
    PRI = 630,  // Puerto Rico
    QAT = 634,  // Qatar
    KOR = 410,  // Republic of Korea
    MDA = 498,  // Republic of Moldova
    REU = 638,  // Réunion
    ROU = 642,  // Romania
    RUS = 643,  // Russian Federation
    RWA = 646,  // Rwanda
    BLM = 652,  // Saint Barthélemy
    SHN = 654,  // Saint Helena
    KNA = 659,  // Saint Kitts and Nevis
    LCA = 662,  // Saint Lucia
    MAF = 663,  // Saint Martin (French Part)
    SPM = 666,  // Saint Pierre and Miquelon
    VCT = 670,  // Saint Vincent and the Grenadines
    WSM = 882,  // Samoa
    SMR = 674,  // San Marino
    STP = 678,  // Sao Tome and Principe
    SAU = 682,  // Saudi Arabia
    SEN = 686,  // Senegal
    SRB = 688,  // Serbia
    SYC = 690,  // Seychelles
    SLE = 694,  // Sierra Leone
    SGP = 702,  // Singapore
    SXM = 534,  // Sint Maarten (Dutch part)
    SVK = 703,  // Slovakia
    SVN = 705,  // Slovenia
    SLB = 90,   // Solomon Islands
    SOM = 706,  // Somalia
    ZAF = 710,  // South Africa
    SGS = 239,  // South Georgia and the South Sandwich Islands
    SSD = 728,  // South Sudan
    ESP = 724,  // Spain
    LKA = 144,  // Sri Lanka
    PSE = 275,  // State of Palestine
    SDN = 729,  // Sudan
    SUR = 740,  // Suriname
    SJM = 744,  // Svalbard and Jan Mayen Islands
    SWE = 752,  // Sweden
    CHE = 756,  // Switzerland
    SYR = 760,  // Syrian Arab Republic
    TJK = 762,  // Tajikistan
    THA = 764,  // Thailand
    TLS = 626,  // Timor-Leste
    TGO = 768,  // Togo
    TKL = 772,  // Tokelau
    TON = 776,  // Tonga
    TTO = 780,  // Trinidad and Tobago
    TUN = 788,  // Tunisia
    TUR = 792,  // Türkiye
    TKM = 795,  // Turkmenistan
    TCA = 796,  // Turks and Caicos Islands
    TUV = 798,  // Tuvalu
    UGA = 800,  // Uganda
    UKR = 804,  // Ukraine
    ARE = 784,  // United Arab Emirates
    GBR = 826,  // United Kingdom of Great Britain and Northern Ireland
    TZA = 834,  // United Republic of Tanzania
    UMI = 581,  // United States Minor Outlying Islands
    USA = 840,  // United States of America
    VIR = 850,  // United States Virgin Islands
    URY = 858,  // Uruguay
    UZB = 860,  // Uzbekistan
    VUT = 548,  // Vanuatu
    VEN = 862,  // Venezuela (Bolivarian Republic of)
    VNM = 704,  // Viet Nam
    WLF = 876,  // Wallis and Futuna Islands
    ESH = 732,  // Western Sahara
    YEM = 887,  // Yemen
    ZMB = 894,  // Zambia
    ZWE = 716   // Zimbabwe
};

enum class Continent_Subregions {
    WORLD = 1,
    SOUTHERN_ASIA = 34,
    EASTERN_ASIA = 30,
    SOUTH_EASTERN_ASIA = 35,
    EASTERN_AFRICA = 14,
    SOUTH_AMERICA = 5,
    WESTERN_AFRICA = 11,
    NORTHERN_AMERICA = 21,
    EASTERN_EUROPE = 151,
    WESTERN_ASIA = 145,
    NORTHERN_AFRICA = 15,
    WESTERN_EUROPE = 155,
    MIDDLE_AFRICA = 17,
    CENTRAL_AMERICA = 13,
    SOUTHERN_EUROPE = 39,
    NORTHERN_EUROPE = 154,
    CENTRAL_ASIA = 143,
    SOUTHERN_AFRICA = 18,
    CARIBBEAN = 29,
    AUSTRALIA_AND_NEW_ZEALAND = 53,
    MELANESIA = 54,
    POLYNESIA = 61,
    MICRONESIA = 57,
    ANTARCTICA = 10
};