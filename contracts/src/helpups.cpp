/*/ --- 

This file handles the dispatch of tokens, etc in a standardized way.
This allows us to be more flexible with what constitutes an up:
1) A token transfer with an up| memo
2) A token stansfer with just a name that's a content ID (no special memo)
3) An Up action on a contract that has been registered as a "content id" (no action for this yet)

/*/// ---



// --- DISPATCHER Checks + calls logup() updateiou() and updatetotal() --- //
void upsertup(uint32_t upscount, name upsender, uint64_t content_id, bool negative) {
      require_auth( upsender );
    // --- Check content Id valid --- //

    if (!negative){
        // --- Log the ups in ups table --- // 
        upsert_logup(upscount, upsender, content_id, negative);
        
        // --- Calls action to update the TOTALS table -- //
        upsert_total(upscount, upsender, content_id, negative);

        //  --- Call action to update IOU table ----- //
        upsert_ious(upscount, upsender, content_id, false);
    } //else call removal functions
}//END upsertup()

// --- Update running log of ups --- // TODO update to this contract
void upsert_logup(uint32_t upscount, name upsender, uint32_t content_id, bool negative){
  //NOTE negative should only be called for deletions (user gets removed from system)

  // --- Add record to _ups --- //
  _ups(get_self(), content_id); //WARN CHECK - is content_id right here? URGENT I really think this is wrong for all upserts
  auto ups_iterator = _ups.find(upid);
  uint32_t time_of_up = eosio::time_point_sec::sec_since_epoch();
  if( ups_iterator == _ups.end() )
  { // -- Make New Record
    _ups.emplace(upsender, [&]( auto& row ) {//URGENT This needs to be changed when we figure out the PK issue
      row.upid = _ups.available_primary_key();
      row.totalups = newups;
      row.tuid = momentu;
    });
  } 
  else 
  { // -- Update Record
    if (negative){
      _ups.modify(ups_iterator, upsender, [&]( auto& row ) {//URGENT This needs to be changed when we figure out the PK issue
          row.totalups -= newups;
      });
    } else {
      _ups.modify(ups_iterator, upsender, [&]( auto& row ) {//URGENT This needs to be changed when we figure out the PK issue
          row.totalups += newups;
      });
    }

    
  }//END if(results _ups) 
}

// --- Upsert _uppers and _totals --- // TODO update to this contract
void upsert_total(uint32_t &upscount, name &upsender, uint32_t &content_id, bool negative) {


  // --- Update / Insert _totals record of cumulative song Ups --- //
  _totals(get_self(), content_id);
  auto total_iterator = _totals.find(content_id);
  uint32_t time_of_up = eosio::time_point_sec::sec_since_epoch();
  if( total_iterator == _totals.end() )
  { // -- Make New Record
    _totals.emplace(upsender, [&]( auto& row ) {
      row.key = content_id;
      row.totalups = newups;
      row.updated = time_of_up;
    });
  } 
  else 
  { // -- Update Record 
    if(!negative){
      _totals.modify(total_iterator, upsender, [&]( auto& row ) {
        row.key = content_id;
        row.totalups += newups;
        row.updated = time_of_up;
      });
    } else { // Subtract the value from totals
      _totals.modify(total_iterator, upsender, [&]( auto& row ) {
        row.key = content_id;
        row.totalups -= newups;
        row.updated = time_of_up;
      });
    }
    
    
  }//END if(results _totals)

  // --- Update / Insert _uppers record --- //
  _uppers(get_self(), content_id);
  auto listener_iterator = _uppers.find(content_id);
  if( listener_iterator == _uppers.end() )
  {
    _uppers.emplace(upsender, [&]( auto& row ) {
      row.upsender = upsender;
      row.firstup = time_of_up;
      row.lastup = time_of_up;
      row.totalups = newups;
    });
  } 
  else 
  {
    _uppers.modify(listener_iterator, upsender, [&]( auto& row ) {
      row.lastup = time_point_sec::sec_since_epoch();
      row.totalups += newups;
    });
  }//END if(results _uppers)
}//END upsert_total()

// --- Upsert IOUs --- //
void upsert_ious(uint32_t upscount, name &upsender, uint64_t content_id, bool subtract){
  //CHECK Not using any auth, double check we already did that 

  check(has_auth(get_self()), "Only the contract can modify the ious table. ")

  // --- Add record to _ups --- // 
  _ious(get_self(), get_self().value); 
  auto ious_itr = _ious.find(iouid); 
  uint32_t time_of_up = time_point_sec::sec_since_epoch();
  uint32_t = find_tu();
  if( ious_itr == _ups.end())
  { // -- Make New Record
    _ious.emplace(upsender, [&]( auto& row ) {
      row.key = _ious.available_primary_key(); 
      row.upsender = upsender;
      row.content_id = content_id;
      row.tuid = timeunit;
      row.upcatcher = artistacc;
      row.upscount = upscount;
      row.initialized = time_of_up;
      row.updated = time_of_up;
    });
  } 
  else 
  { // -- Update Record
    _ious.modify(ious_itr, upsender, [&]( auto& row ) {
      row.upscount += upscount;
      row.upstype = upstype;// Can be from SOL -> BIGSOL
      row.updated = time_of_up;
    });
  }//END if(results _ups)
}//END upsert_ious()

// --- Send the beautiful people their tokens  --- //
void pay_iou(uint32_t maxpay = 0, name& receiver, bool paythem = true){

  check(receiver != ""_n, "We can't pay no one.");

  // --- Check that the contract is the receiver --- //
  config_t _config(get_self(), get_self().value);
  check(_config.exists(), "Configuration must be set first. How did you even get here?");
  auto conf = _config.get();

  // Find the receiver records is in the _ious table

  // --- Get the IOUs --- //
  ious_t _ious(get_self(), receiver.value);
  auto iou_itr = _ious.begin();
  check(iou_itr != _ious.end(), "You are all paid up. Send some Ups and come back");
  
  // --- Calculate Payments --- //
  uint32_t paid = 0;
  std::vector<uint64_t> ious_to_erase;

  // --- Iterate over the IOUs and accumulate payments until reaching maxpay or end of table --- //
  while(iou_itr != _ious.end() && (maxpay == 0 || paid < maxpay) && records_processed <= 12){
    auto& iou = iou_itr;
    paid += iou.upscount; 
    ious_to_erase.push_back(iou.iouid); // Track IOU IDs for deletion
    iou_itr++;
  }

    // Calculate the total amount to be paid
    asset total_payment = conf.one_reward_amount.amount * paid;

  // --- Pay the people --- //
  if(total_payment.amount > 0 && paythem){
    // Use the eosio.token transfer action to send the payment
    action(
        permission_level{get_self(), "active"_n},
        conf.reward_token_contract, 
        "transfer"_n,
        std::make_tuple(get_self(), receiver, total_payment, std::string("Rewards for "+receiver.to_string()))
    ).send();
  }

  // --- Erase paid IOUs from the table --- //
  for(auto& iouid: ious_to_erase){
    auto itr = _ious.find(iouid);
    if(itr != _ious.end()){
        _ious.erase(itr);
    }
  }
}//END pay_iou()



void removecontent(uint64_t content_id) {

  // delete from totals table, ups table, ious table NOTE currently in action not here, pass a 1 arg to the upsert's final parameter
    
}//END removecont()

// --- Handles adding both NFT content and URL content --- // TODO add to the new content_domain singleton
void addcontent(name& submitter, double latitude = 0.0, double longitude = 0.0, uint32_t continent_subregion_code = 0, uint32_t country_code = 0, const std::string& continent_subregion_name = "", const std::string& country_iso3 = "", uint32_t subdivision = 0, uint32_t postal_code = 0, string& url = "", name domain = ""_n, name collection = ""_n, uint32_t templateid = 0)
{ 

    // --- Check if submitter is in providers table --- //
    require_auth(submitter): 

    bool is_nft = (collection != ""_n) && !templateid;

    uint32_t subcontinent = 0;
    uint32_t country = 0;
    int32_t latitude_int = 0;
    int32_t longitude_int = 0;

    // --- Validate and format Latitude and Longitude --- //
    if (latitude == 0.0 && longitude == 0.0){
        vector<int32_t> formatted_coords = validate_and_format_coords({latitude, longitude});
        latitude_int = formatted_coords[0];
        longitude_int = formatted_coords[1];
    }

    // --- Validate the Continent Subregion as a string or an int --- //
    if (!continent_subregion_name.empty() || continent_subregion_code != 0) {
        subcontinent = is_valid_continent_subregion(continent_subregion_code, continent_subregion_name);
    } 

    // --- Validate the country as a string or an int --- //
    if (!country_name.empty() || country_code != 0) {
        country = is_valid_country(country_code, country_name);
    } 
    
    // --- Handle URL --- //
    if ( !is_nft && url != "" ) {
      name domain = parse_url(url);
      hash url_hash = parse_url(url, 1);
      hash url_chopped = parse_url(url, 0, 1);

      // --- Check if domain is registered --- //
      content_provider_singleton content_prov(get_self(), domain_parsed.value);
      check(content_prov.exists(), "Register the domain to start adding content for upvotes");

      // --- Check if content already exists --- //
        content_t contents(get_self(), get_self().value);
        auto gudhash = contents.get_index<"bygudahash"_n>();
        auto itr = gudhash.find(url_hash);

        check(itr == gudhash.end(), "Content already exists, you can sends ups now");

        // Insert new content
        contents.emplace(submitter, [&](auto& row) {
            row.id = contents.available_primary_key();
            row.domain = domain;
            row.submitter = submitter;
            row.link = url_chopped;
            row.external_id = 0;
            row.gudahash = url_hash;
            row.created = time_point_sec::sec_since_epoch();
            row.latitude = latitude_int; // CHANGE and see if it compiles 
            row.longitude = longitude_int;
            row.subcontinent = (subcontinent != 0) ? subcontinent : 1;
            row.country = country;
            row.subdivision = subdivision
            row.postal_code = postal_code;

    } else if ( is_nft ) {
      // --- Handle NFT --- //
      // --- Check the providers table --- //
      content_provider_singleton content_prov(get_self(), nft_collection.value);

      // --- Ensure the collection is not already registered --- //
      check(content_prov.exists(), "This collection is not registered. Call regnftcol first.");


      // --- Check if templateid is valid --- //
      templates_t templates_t(ATOMICASSETS_ACCOUNT, collection.value);
      auto template_itr = templates_t.find(templateid);
      check(template_itr != templates_t.end(), "Template does not exist");

      // --- Check if NFT already exists in content_t --- //
      content_t contents(get_self(), get_self().value);
      auto by_external_id_idx = contents.get_index<"byexternal"_n>(); // Assuming this is the secondary index for external_id
      auto nft_itr = by_external_id_idx.find(templateid);
      check(nft_itr == by_external_id_idx.end(), "NFT is already registered. Send Ups.");

      // Insert new NFT content
      contents.emplace(submitter, [&](auto& row) {
        row.id = contents.available_primary_key();
        row.domain = collection; // --- Using collection name as domain for NFTs
        row.submitter = submitter;
        row.external_id = templateid; // --- Set external_id to templateid for NFTs
        row.gudahash = checksum256(); 
        row.created = current_time_point();
        row.latitude = latitude_int; // CHANGE and see if it compiles 
        row.longitude = longitude_int;
        row.subcontinent = (subcontinent != 0) ? subcontinent : 1;
        row.country = country;
        row.subdivision = subdivision
        row.postal_code = postal_code;
      });


    } else {
      check(false, "This is not a valid URL or NFT");
    }

    // dont forget : row.id = _ups.available_primary_key();
}




// --- Returns the current Time Unit --- //
uint32_t find_tu(uint32_t& momentuin, uint32_t tu_length){
  // 1561139700 is the first Time Unit in Seconds
  uint32_t time_unit = floor(momentuin / tu_length);  // Divide by the length of a Time Unit in seconds
  return time_unit;
}

// --- Returns the current Time Unit --- //
uint32_t find_tu(uint32_t tu_length = 0){
  if (!tu_length){
    auto conf = check_config();
    tu_length = conf.timeunit;
  }
  uint32_t momentuin = eosio::time_point_sec::sec_since_epoch();
  uint32_t time_unit = floor(momentuin / tu_length);  // Divide by the length of a Time Unit in seconds
  return time_unit;
}

// --- Returns Name from Domain or url checksum256 if whole thang --- //
auto parse_url(const string& url, bool hash_whole = false, bool chopped_whole = false, bool chopped_domain = false) const { 
    // Find the start position after "://"
    auto start = url.find("://");
    if (start != string::npos) {
        start += 3; // Move past "://"
    } else {
        start = 0; // If "://" not found, start from the beginning
    }

    // Check if "www." is present after "://"
    auto www = url.find("www.", start);
    if (www == start) {
        start += 4; // Move past "www."
    }

    //  --- Extract the domain part after "://" and "www." --- //
    string domain_part;
    string domain_main = url.substr(start);

    // --- Stop at the slash --- // 
    auto end = domain_main.find('/');
    if (end != string::npos) {
        domain_part = domain_main.substr(0, end);
    }

    // --- If hash is needed return hash + done --- //
    if (hash_whole) {
        return sha256(domain_main, domain_main.size());
    } else if (chopped_whole){
      return domain_main;
    } else if (chopped_domain){
      return domain_part;
    }

    // Replace invalid characters with a deterministic mapping to letters starting with 'a'
    for (auto& c : domain_part) {
        check (static_cast<unsigned char>(c) > 127, "⚡️ Invalid domain name. Must be ASCII characters only.");
            // Make uppercase letters lowercase
        if (c >= 'A' && c <= 'Z') {
            c = c - 'A' + 'a';
        }

        if ((c < 'a' || c > 'z') && (c < '1' || c > '5') && c != '.') {
            if (c >= '6' && c <= '9') {
                // Map '6'-'9' directly to 'a'-'d'
                c = 'a' + (c - '6');
            } else {
                // Map other invalid characters to letters starting with 'e'
                unsigned char illegalCharValue = static_cast<unsigned char>(c) % 20; // Using modulo to spread the mapping
                c = 'e' + (illegalCharValue % (122 - 'e')); // Ensure mapping is within 'e'-'z'
            }
        }
    }

    // --- Return the domain part as a name --- //
    return name(domain_part);
}//END parse_url()

// --- Gets config object and ensures contract not paused --- //
auto check_config(bool ignore_empty = false) // --- RETURNS false or config type
{
    // --- Get config table --- //
    config_t conf_tbl(get_self(), get_self().value);

    bool existencial = conf_tbl.exists();

    // --- Ensure the rewards are set up --- //
    check(existencial || ignore_empty, "⚡️ An administrator needs to set up this contract before you can use it.");

    // --- Return a blank object or the config object --- //
    if (!existencial){
        const auto& conf = conf_tbl.get();

        // --- If both rewards and ups are paused, no go, shut down everything --- //
        check(!(conf.paused_rewards && !conf.paused_ups), "⚡️ Rewards and Ups are both currently frozen. Check back later.");

        return conf;

    } else return false; // --- Returns only when ignore empty is set to avoid check. 
}

// --- Check if user is authorized on NFT collection --- //
bool isAuthorized(name collection, name user)
    {
        auto itrCollection = atomicassets::collections.require_find(collection.value, "No collection with this name exists.");
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
string normalize_enum_name(const std::string& input) {
    std::string output = input;
    std::transform(output.begin(), output.end(), output.begin(), [](unsigned char c) { return std::toupper(c); });
    std::replace(output.begin(), output.end(), ' ', '_');
    return output;
}

/*/---
INFO this can be moved to the commented table in ups.hpp, or a web4.tetra contract that has the values and can be referenced
Pending testing

/*///----

uint32_t is_valid_continent_subregion(uint32_t code, const std::string& name = "") {
    if (!name.empty()) {
        std::string enum_name = normalize_enum_name(name);
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

uint32_t is_valid_country(uint32_t code, const std::string& name = "") {
    if (!name.empty()) {
        std::string enum_name = normalize_enum_name(name);
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
            case 834: return 834;
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

vector<int32_t> validate_and_format_coords(const vector<double>& coords) {
    check(coords.size() == 2, "Coordinates vector must contain two decimal numbers, first being latitude, second longitude.");
    double latitude = coords[0];
    double longitude = coords[1];

    // Validate Latitude and Longitude
    check(latitude >= -90.0 && latitude <= 90.0, "Latitude must be between -90 and 90.");
    check(longitude >= -180.0 && longitude <= 180.0, "Longitude must be between -180 and 180.");

    // Format to 4 decimal places and convert to integer with the decimal part
    int32_t formatted_latitude = static_cast<int32_t>(latitude * 10000); // Scaling to maintain 4 decimal digits
    int32_t formatted_longitude = static_cast<int32_t>(longitude * 10000); // Scaling to maintain 4 decimal digits

    return {formatted_latitude, formatted_longitude};
}
