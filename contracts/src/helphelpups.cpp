
// --- Gets config object and ensures contract not paused --- //
ups::config ups::check_config() // --- RETURNS false or config type
{
    // --- Get config table --- //
    config_t conf_tbl(get_self(), get_self().value);

    bool existencial = conf_tbl.exists();

    // --- Ensure the rewards are set up --- //
    check(existencial, "⚡️ An administrator needs to set up this contract before you can use it.");

    // --- Return a blank object or the config object --- //

    const auto& conf = conf_tbl.get();

    // --- If both rewards and ups are paused, no go, shut down everything --- //
    check(!(conf.paused_rewards && !conf.paused_ups), "⚡️ Rewards and Ups are both currently paused. Check back later.");

    return conf;
}



// --- Returns the current Time Unit --- //
uint32_t ups::find_tu(uint32_t momentuin, uint32_t tu_length){
  // 1561139700 is the first Time Unit in Seconds
  uint32_t time_unit = (momentuin / tu_length);  // Divide by the length of a Time Unit in seconds
  return time_unit;
}

// --- Returns the current Time Unit --- //
uint32_t ups::find_tu(uint32_t tu_length = 0){
  if (!tu_length){
    config conf = ups::check_config();
    tu_length = conf.timeunit;
  }
  uint32_t momentuin = eosio::current_time_point().sec_since_epoch();
  uint32_t time_unit = (momentuin / tu_length);  // Divide by the length of a Time Unit in seconds
  return time_unit;
}

checksum256 ups::url_hash(const string url) {

    string newurl = chopped_url(url);
    return eosio::sha256(newurl.data(), newurl.size());
}

// --- Returns URL after removing the protocol part and "www." --- //

//TODO update to not include the domain in the link? Weigh benefits / costs
string ups::chopped_url(const string url) {
    auto start = url.find("://");
    if (start != string::npos) {
        start += 3; // Move past "://"
    } else {
        start = 0; // If "://" not found, start from the beginning
    }

    auto www = url.find("www.", start);
    if (www == start) {
        start += 4; // Move past "www."
    }

    return url.substr(start);
}

// --- Returns just the domain part of the URL as a name --- //
name ups::url_domain_name(const string url) {

    string domain_main = chopped_url(url);
    auto end = domain_main.find('/');
    string domain_part = (end != string::npos) ? domain_main.substr(0, end) : domain_main;

    // Replace invalid characters with a deterministic mapping to letters starting with 'a'
    for (auto& c : domain_part) {
        if (static_cast<unsigned char>(c) > 127) {
            // Skip invalid characters
            continue;
        }

        // Make uppercase letters lowercase
        if (c >= 'A' && c <= 'Z') {
            c = c - 'A' + 'a';
        }

        // Map invalid characters to letters
        if ((c < 'a' || c > 'z') && (c < '1' || c > '5') && c != '.') {
            c = 'e' + (static_cast<unsigned char>(c) % 20) % (122 - 'e'); // Mapping is within 'e'-'z'
        }
    }

    // --- Truncate any characters after 12th character --- //
    if (domain_part.size() > 12) {
        domain_part = domain_part.substr(0, 12);
    }

    return name(domain_part);
}


// --- Check if user is authorized on NFT collection --- //
bool ups::isAuthorized(name collection, name user)
    {
        auto itrCollection = atomicassets::collections.require_find(collection.value, "⚡️ No collection with this name exists.");
        bool authorized = false;
        vector<name> authAccounts = itrCollection->authorized_accounts;
        for (auto it = authAccounts.begin(); it != authAccounts.end() && !authorized; it++)
        {
        if (user == name(*it))
        {
            authorized = true;
            break;
        }
        }
        return authorized;
    }//END isAuthorized()

// --- Converts a normal string to the name of a enum entry --- //
char to_upper(char c) {
    return std::toupper(static_cast<unsigned char>(c));
}

string ups::normalize_enum_name(const string& input) {
    string output = input;
    std::transform(output.begin(), output.end(), output.begin(), to_upper);
    std::replace(output.begin(), output.end(), ' ', '_');
    return output;
}
/*/---
INFO this can be moved to the commented table in ups.hpp, or a web4.tetra contract that has the values and can be referenced
Pending testing

/*///----

uint32_t ups::is_valid_continent_subregion(uint32_t code, const string& name = "") {
    if (!name.empty()) {
        string enum_name = normalize_enum_name(name);
        if (enum_name == "WORLD") return 1;
        else if (enum_name == "SOUTHERN_ASIA") return 34;
        else if (enum_name == "EASTERN_ASIA") return 30;
        else if (enum_name == "SOUTH_EASTERN_ASIA") return 35;
        else if (enum_name == "EASTERN_AFRICA") return 14;
        else if (enum_name == "SOUTH_AMERICA") return 5;
        else if (enum_name == "WESTERN_AFRICA") return 11;
        else if (enum_name == "NORTHERN_AMERICA") return 21;
        else if (enum_name == "EASTERN_EUROPE") return 151;
        else if (enum_name == "WESTERN_ASIA") return 145;
        else if (enum_name == "NORTHERN_AFRICA") return 15;
        else if (enum_name == "WESTERN_EUROPE") return 155;
        else if (enum_name == "MIDDLE_AFRICA") return 17;
        else if (enum_name == "CENTRAL_AMERICA") return 13;
        else if (enum_name == "SOUTHERN_EUROPE") return 39;
        else if (enum_name == "NORTHERN_EUROPE") return 154;
        else if (enum_name == "CENTRAL_ASIA") return 143;
        else if (enum_name == "SOUTHERN_AFRICA") return 18;
        else if (enum_name == "CARIBBEAN") return 29;
        else if (enum_name == "AUSTRALIA_AND_NEW_ZEALAND") return 53;
        else if (enum_name == "MELANESIA") return 54;
        else if (enum_name == "POLYNESIA") return 61;
        else if (enum_name == "MICRONESIA") return 57;
        else if (enum_name == "ANTARCTICA") return 10;
        else return 0; // Invalid name
    } else {
        switch(code) {
            case 1: return 1;   // WORLD
            case 34: return 34; // SOUTHERN_ASIA
            case 30: return 30; // EASTERN_ASIA
            case 35: return 35; // SOUTH_EASTERN_ASIA
            case 14: return 14; // EASTERN_AFRICA
            case 5: return 5;   // SOUTH_AMERICA
            case 11: return 11; // WESTERN_AFRICA
            case 21: return 21; // NORTHERN_AMERICA
            case 151: return 151; // EASTERN_EUROPE
            case 145: return 145; // WESTERN_ASIA
            case 15: return 15;   // NORTHERN_AFRICA
            case 155: return 155; // WESTERN_EUROPE
            case 17: return 17;   // MIDDLE_AFRICA
            case 13: return 13;   // CENTRAL_AMERICA
            case 39: return 39;   // SOUTHERN_EUROPE
            case 154: return 154; // NORTHERN_EUROPE
            case 143: return 143; // CENTRAL_ASIA
            case 18: return 18;   // SOUTHERN_AFRICA
            case 29: return 29;   // CARIBBEAN
            case 53: return 53;   // AUSTRALIA_AND_NEW_ZEALAND
            case 54: return 54;   // MELANESIA
            case 61: return 61;   // POLYNESIA
            case 57: return 57;   // MICRONESIA
            case 10: return 10;   // ANTARCTICA
            default: return 0;    // Invalid code
        }
    }
}

uint32_t ups::is_valid_country(uint32_t code, const string country_iso3) {
    if (!country_iso3.empty()) {
        string enum_name = normalize_enum_name(country_iso3);
        if (enum_name == "AFG") return 4;
        else if (enum_name == "ALA") return 248;
        else if (enum_name == "ALB") return 8;
        else if (enum_name == "DZA") return 12;
        else if (enum_name == "ASM") return 16;
        else if (enum_name == "AND") return 20;
        else if (enum_name == "AGO") return 24;
        else if (enum_name == "AIA") return 660;
        else if (enum_name == "ATA") return 10;
        else if (enum_name == "ATG") return 28;
        else if (enum_name == "ARG") return 32;
        else if (enum_name == "ARM") return 51;
        else if (enum_name == "ABW") return 533;
        else if (enum_name == "AUS") return 36;
        else if (enum_name == "AUT") return 40;
        else if (enum_name == "AZE") return 31;
        else if (enum_name == "BHS") return 44;
        else if (enum_name == "BHR") return 48;
        else if (enum_name == "BGD") return 50;
        else if (enum_name == "BRB") return 52;
        else if (enum_name == "BLR") return 112;
        else if (enum_name == "BEL") return 56;
        else if (enum_name == "BLZ") return 84;
        else if (enum_name == "BEN") return 204;
        else if (enum_name == "BMU") return 60;
        else if (enum_name == "BTN") return 64;
        else if (enum_name == "BOL") return 68;
        else if (enum_name == "BES") return 535;
        else if (enum_name == "BIH") return 70;
        else if (enum_name == "BWA") return 72;
        else if (enum_name == "BVT") return 74;
        else if (enum_name == "BRA") return 76;
        else if (enum_name == "IOT") return 86;
        else if (enum_name == "VGB") return 92;
        else if (enum_name == "BRN") return 96;
        else if (enum_name == "BGR") return 100;
        else if (enum_name == "BFA") return 854;
        else if (enum_name == "BDI") return 108;
        else if (enum_name == "CPV") return 132;
        else if (enum_name == "KHM") return 116;
        else if (enum_name == "CMR") return 120;
        else if (enum_name == "CAN") return 124;
        else if (enum_name == "CYM") return 136;
        else if (enum_name == "CAF") return 140;
        else if (enum_name == "TCD") return 148;
        else if (enum_name == "CHL") return 152;
        else if (enum_name == "CHN") return 156;
        else if (enum_name == "HKG") return 344;
        else if (enum_name == "MAC") return 446;
        else if (enum_name == "CXR") return 162;
        else if (enum_name == "CCK") return 166;
        else if (enum_name == "COL") return 170;
        else if (enum_name == "COM") return 174;
        else if (enum_name == "COG") return 178;
        else if (enum_name == "COD") return 180;
        else if (enum_name == "COK") return 184;
        else if (enum_name == "CRI") return 188;
        else if (enum_name == "CIV") return 384;
        else if (enum_name == "HRV") return 191;
        else if (enum_name == "CUB") return 192;
        else if (enum_name == "CUW") return 531;
        else if (enum_name == "CYP") return 196;
        else if (enum_name == "CZE") return 203;
        else if (enum_name == "DNK") return 208;
        else if (enum_name == "DJI") return 262;
        else if (enum_name == "DMA") return 212;
        else if (enum_name == "DOM") return 214;
        else if (enum_name == "ECU") return 218;
        else if (enum_name == "EGY") return 818;
        else if (enum_name == "SLV") return 222;
        else if (enum_name == "GNQ") return 226;
        else if (enum_name == "ERI") return 232;
        else if (enum_name == "EST") return 233;
        else if (enum_name == "SWZ") return 748;
        else if (enum_name == "ETH") return 231;
        else if (enum_name == "FLK") return 238;
        else if (enum_name == "FRO") return 234;
        else if (enum_name == "FJI") return 242;
        else if (enum_name == "FIN") return 246;
        else if (enum_name == "FRA") return 250;
        else if (enum_name == "GUF") return 254;
        else if (enum_name == "PYF") return 258;
        else if (enum_name == "ATF") return 260;
        else if (enum_name == "GAB") return 266;
        else if (enum_name == "GMB") return 270;
        else if (enum_name == "GEO") return 268;
        else if (enum_name == "DEU") return 276;
        else if (enum_name == "GHA") return 288;
        else if (enum_name == "GIB") return 292;
        else if (enum_name == "GRC") return 300;
        else if (enum_name == "GRL") return 304;
        else if (enum_name == "GRD") return 308;
        else if (enum_name == "GLP") return 312;
        else if (enum_name == "GUM") return 316;
        else if (enum_name == "GTM") return 320;
        else if (enum_name == "GGY") return 831;
        else if (enum_name == "GIN") return 324;
        else if (enum_name == "GNB") return 624;
        else if (enum_name == "GUY") return 328;
        else if (enum_name == "HTI") return 332;
        else if (enum_name == "HMD") return 334;
        else if (enum_name == "VAT") return 336;
        else if (enum_name == "HND") return 340;
        else if (enum_name == "HUN") return 348;
        else if (enum_name == "ISL") return 352;
        else if (enum_name == "IND") return 356;
        else if (enum_name == "IDN") return 360;
        else if (enum_name == "IRN") return 364;
        else if (enum_name == "IRQ") return 368;
        else if (enum_name == "IRL") return 372;
        else if (enum_name == "IMN") return 833;
        else if (enum_name == "ISR") return 376;
        else if (enum_name == "ITA") return 380;
        else if (enum_name == "JAM") return 388;
        else if (enum_name == "JPN") return 392;
        else if (enum_name == "JEY") return 832;
        else if (enum_name == "JOR") return 400;
        else if (enum_name == "KAZ") return 398;
        else if (enum_name == "KEN") return 404;
        else if (enum_name == "KIR") return 296;
        else if (enum_name == "PRK") return 408;
        else if (enum_name == "KOR") return 410;
        else if (enum_name == "KWT") return 414;
        else if (enum_name == "KGZ") return 417;
        else if (enum_name == "LAO") return 418;
        else if (enum_name == "LVA") return 428;
        else if (enum_name == "LBN") return 422;
        else if (enum_name == "LSO") return 426;
        else if (enum_name == "LBR") return 430;
        else if (enum_name == "LBY") return 434;
        else if (enum_name == "LIE") return 438;
        else if (enum_name == "LTU") return 440;
        else if (enum_name == "LUX") return 442;
        else if (enum_name == "MDG") return 450;
        else if (enum_name == "MWI") return 454;
        else if (enum_name == "MYS") return 458;
        else if (enum_name == "MDV") return 462;
        else if (enum_name == "MLI") return 466;
        else if (enum_name == "MLT") return 470;
        else if (enum_name == "MHL") return 584;
        else if (enum_name == "MTQ") return 474;
        else if (enum_name == "MRT") return 478;
        else if (enum_name == "MUS") return 480;
        else if (enum_name == "MYT") return 175;
        else if (enum_name == "MEX") return 484;
        else if (enum_name == "FSM") return 583;
        else if (enum_name == "MCO") return 492;
        else if (enum_name == "MNG") return 496;
        else if (enum_name == "MNE") return 499;
        else if (enum_name == "MSR") return 500;
        else if (enum_name == "MAR") return 504;
        else if (enum_name == "MOZ") return 508;
        else if (enum_name == "MMR") return 104;
        else if (enum_name == "NAM") return 516;
        else if (enum_name == "NRU") return 520;
        else if (enum_name == "NPL") return 524;
        else if (enum_name == "NLD") return 528;
        else if (enum_name == "NCL") return 540;
        else if (enum_name == "NZL") return 554;
        else if (enum_name == "NIC") return 558;
        else if (enum_name == "NER") return 562;
        else if (enum_name == "NGA") return 566;
        else if (enum_name == "NIU") return 570;
        else if (enum_name == "NFK") return 574;
        else if (enum_name == "MNP") return 580;
        else if (enum_name == "NOR") return 578;
        else if (enum_name == "OMN") return 512;
        else if (enum_name == "PAK") return 586;
        else if (enum_name == "PLW") return 585;
        else if (enum_name == "PSE") return 275;
        else if (enum_name == "PAN") return 591;
        else if (enum_name == "PNG") return 598;
        else if (enum_name == "PRY") return 600;
        else if (enum_name == "PER") return 604;
        else if (enum_name == "PHL") return 608;
        else if (enum_name == "PCN") return 612;
        else if (enum_name == "POL") return 616;
        else if (enum_name == "PRT") return 620;
        else if (enum_name == "PRI") return 630;
        else if (enum_name == "QAT") return 634;
        else if (enum_name == "REU") return 638;
        else if (enum_name == "ROU") return 642;
        else if (enum_name == "RUS") return 643;
        else if (enum_name == "RWA") return 646;
        else if (enum_name == "BLM") return 652;
        else if (enum_name == "SHN") return 654;
        else if (enum_name == "KNA") return 659;
        else if (enum_name == "LCA") return 662;
        else if (enum_name == "MAF") return 663;
        else if (enum_name == "SPM") return 666;
        else if (enum_name == "VCT") return 670;
        else if (enum_name == "WSM") return 882;
        else if (enum_name == "SMR") return 674;
        else if (enum_name == "STP") return 678;
        else if (enum_name == "SAU") return 682;
        else if (enum_name == "SEN") return 686;
        else if (enum_name == "SRB") return 688;
        else if (enum_name == "SYC") return 690;
        else if (enum_name == "SLE") return 694;
        else if (enum_name == "SGP") return 702;
        else if (enum_name == "SXM") return 534;
        else if (enum_name == "SVK") return 703;
        else if (enum_name == "SVN") return 705;
        else if (enum_name == "SLB") return 90;
        else if (enum_name == "SOM") return 706;
        else if (enum_name == "ZAF") return 710;
        else if (enum_name == "SGS") return 239;
        else if (enum_name == "SSD") return 728;
        else if (enum_name == "ESP") return 724;
        else if (enum_name == "LKA") return 144;
        else if (enum_name == "SDN") return 729;
        else if (enum_name == "SUR") return 740;
        else if (enum_name == "SJM") return 744;
        else if (enum_name == "SWE") return 752;
        else if (enum_name == "CHE") return 756;
        else if (enum_name == "SYR") return 760;
        else if (enum_name == "TWN") return 158;
        else if (enum_name == "TJK") return 762;
        else if (enum_name == "TZA") return 834;
        else if (enum_name == "THA") return 764;
        else if (enum_name == "TLS") return 626;
        else if (enum_name == "TGO") return 768;
        else if (enum_name == "TKL") return 772;
        else if (enum_name == "TON") return 776;
        else if (enum_name == "TTO") return 780;
        else if (enum_name == "TUN") return 788;
        else if (enum_name == "TUR") return 792;
        else if (enum_name == "TKM") return 795;
        else if (enum_name == "TCA") return 796;
        else if (enum_name == "TUV") return 798;
        else if (enum_name == "UGA") return 800;
        else if (enum_name == "UKR") return 804;
        else if (enum_name == "ARE") return 784;
        else if (enum_name == "GBR") return 826;
        else if (enum_name == "USA") return 840;
        else if (enum_name == "UMI") return 581;
        else if (enum_name == "URY") return 858;
        else if (enum_name == "UZB") return 860;
        else if (enum_name == "VUT") return 548;
        else if (enum_name == "VEN") return 862;
        else if (enum_name == "VNM") return 704;
        else if (enum_name == "VGB") return 92;
        else if (enum_name == "VIR") return 850;
        else if (enum_name == "WLF") return 876;
        else if (enum_name == "ESH") return 732;
        else if (enum_name == "YEM") return 887;
        else if (enum_name == "ZMB") return 894;
        else if (enum_name == "ZWE") return 716;
        else return 0; // Invalid name
    } else {
        switch(code) {
            case 4: return 4;
            case 248: return 248;
            case 8: return 8;
            case 12: return 12;
            case 16: return 16;
            case 20: return 20;
            case 24: return 24;
            case 660: return 660;
            case 10: return 10;
            case 28: return 28;
            case 32: return 32;
            case 51: return 51;
            case 533: return 533;
            case 36: return 36;
            case 40: return 40;
            case 31: return 31;
            case 44: return 44;
            case 48: return 48;
            case 50: return 50;
            case 52: return 52;
            case 112: return 112;
            case 56: return 56;
            case 84: return 84;
            case 204: return 204;
            case 60: return 60;
            case 64: return 64;
            case 68: return 68;
            case 535: return 535;
            case 70: return 70;
            case 72: return 72;
            case 74: return 74;
            case 76: return 76;
            case 86: return 86;
            case 92: return 92;
            case 96: return 96;
            case 100: return 100;
            case 854: return 854;
            case 108: return 108;
            case 132: return 132;
            case 116: return 116;
            case 120: return 120;
            case 124: return 124;
            case 136: return 136;
            case 140: return 140;
            case 148: return 148;
            case 152: return 152;
            case 156: return 156;
            case 344: return 344;
            case 446: return 446;
            case 162: return 162;
            case 166: return 166;
            case 170: return 170;
            case 174: return 174;
            case 178: return 178;
            case 180: return 180;
            case 184: return 184;
            case 188: return 188;
            case 384: return 384;
            case 191: return 191;
            case 192: return 192;
            case 531: return 531;
            case 196: return 196;
            case 203: return 203;
            case 208: return 208;
            case 262: return 262;
            case 212: return 212;
            case 214: return 214;
            case 218: return 218;
            case 818: return 818;
            case 222: return 222;
            case 226: return 226;
            case 232: return 232;
            case 233: return 233;
            case 748: return 748;
            case 231: return 231;
            case 238: return 238;
            case 234: return 234;
            case 242: return 242;
            case 246: return 246;
            case 250: return 250;
            case 254: return 254;
            case 258: return 258;
            case 260: return 260;
            case 266: return 266;
            case 270: return 270;
            case 268: return 268;
            case 276: return 276;
            case 288: return 288;
            case 292: return 292;
            case 300: return 300;
            case 304: return 304;
            case 308: return 308;
            case 312: return 312;
            case 316: return 316;
            case 320: return 320;
            case 831: return 831;
            case 324: return 324;
            case 624: return 624;
            case 328: return 328;
            case 332: return 332;
            case 334: return 334;
            case 336: return 336;
            case 340: return 340;
            case 348: return 348;
            case 352: return 352;
            case 356: return 356;
            case 360: return 360;
            case 364: return 364;
            case 368: return 368;
            case 372: return 372;
            case 833: return 833;
            case 376: return 376;
            case 380: return 380;
            case 388: return 388;
            case 392: return 392;
            case 832: return 832;
            case 400: return 400;
            case 398: return 398;
            case 404: return 404;
            case 296: return 296;
            case 408: return 408;
            case 410: return 410;
            case 414: return 414;
            case 417: return 417;
            case 418: return 418;
            case 428: return 428;
            case 422: return 422;
            case 426: return 426;
            case 430: return 430;
            case 434: return 434;
            case 438: return 438;
            case 440: return 440;
            case 442: return 442;
            case 450: return 450;
            case 454: return 454;
            case 458: return 458;
            case 462: return 462;
            case 466: return 466;
            case 470: return 470;
            case 584: return 584;
            case 474: return 474;
            case 478: return 478;
            case 480: return 480;
            case 175: return 175;
            case 484: return 484;
            case 583: return 583;
            case 492: return 492;
            case 496: return 496;
            case 499: return 499;
            case 500: return 500;
            case 504: return 504;
            case 508: return 508;
            case 104: return 104;
            case 516: return 516;
            case 520: return 520;
            case 524: return 524;
            case 528: return 528;
            case 540: return 540;
            case 554: return 554;
            case 558: return 558;
            case 562: return 562;
            case 566: return 566;
            case 570: return 570;
            case 574: return 574;
            case 807: return 807;
            case 580: return 580;
            case 578: return 578;
            case 512: return 512;
            case 586: return 586;
            case 585: return 585;
            case 591: return 591;
            case 598: return 598;
            case 600: return 600;
            case 604: return 604;
            case 608: return 608;
            case 612: return 612;
            case 616: return 616;
            case 620: return 620;
            case 630: return 630;
            case 634: return 634;
            case 498: return 498;
            case 638: return 638;
            case 642: return 642;
            case 643: return 643;
            case 646: return 646;
            case 652: return 652;
            case 654: return 654;
            case 659: return 659;
            case 662: return 662;
            case 663: return 663;
            case 666: return 666;
            case 670: return 670;
            case 882: return 882;
            case 674: return 674;
            case 678: return 678;
            case 682: return 682;
            case 686: return 686;
            case 688: return 688;
            case 690: return 690;
            case 694: return 694;
            case 702: return 702;
            case 534: return 534;
            case 703: return 703;
            case 705: return 705;
            case 90: return 90;
            case 706: return 706;
            case 710: return 710;
            case 239: return 239;
            case 728: return 728;
            case 724: return 724;
            case 144: return 144;
            case 275: return 275;
            case 729: return 729;
            case 740: return 740;
            case 744: return 744;
            case 752: return 752;
            case 756: return 756;
            case 760: return 760;
            case 158: return 158;
            case 762: return 762;
            case 834: return 834;
            case 764: return 764;
            case 626: return 626;
            case 768: return 768;
            case 772: return 772;
            case 776: return 776;
            case 780: return 780;
            case 788: return 788;
            case 792: return 792;
            case 795: return 795;
            case 796: return 796;
            case 798: return 798;
            case 800: return 800;
            case 804: return 804;
            case 784: return 784;
            case 826: return 826;
            case 581: return 581;
            case 840: return 840;
            case 850: return 850;
            case 858: return 858;
            case 860: return 860;
            case 548: return 548;
            case 862: return 862;
            case 704: return 704;
            case 876: return 876;
            case 732: return 732;
            case 887: return 887;
            case 894: return 894;
            case 716: return 716;
            default: return 0;
        }
    }
}

vector<int32_t> ups::validate_and_format_coords(const vector<double>& coords) {
    check(coords.size() == 2, "⚡️ Coordinates vector must contain two decimal numbers, first being latitude, second longitude.");
    double latitude = coords[0];
    double longitude = coords[1];

    // Validate Latitude and Longitude
    check(latitude >= -90.0 && latitude <= 90.0, "⚡️ Latitude must be between -90 and 90.");
    check(longitude >= -180.0 && longitude <= 180.0, "⚡️ Longitude must be between -180 and 180.");

    // Format to 4 decimal places and convert to integer with the decimal part
    int32_t formatted_latitude = static_cast<int32_t>(latitude * 10000); // Scaling to maintain 4 decimal digits
    int32_t formatted_longitude = static_cast<int32_t>(longitude * 10000); // Scaling to maintain 4 decimal digits

    //vector<int32_t> formatted_coords(formatted_latitude, formatted_longitude);
    vector<int32_t> formatted_coords = {formatted_latitude, formatted_longitude};

    return formatted_coords;
}