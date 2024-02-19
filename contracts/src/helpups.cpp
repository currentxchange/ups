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

  // delete from totals table, ups table, ious table NOTE currently in action not here
    
}//END removecont()

// --- Handles adding both NFT content and URL content --- // TODO add to the new content_domain singleton
void addcontent(name& submitter, vector<float> latlng = {0.0,0.0}, const vector<uint32_t>& tetra_locode = {0, 0}, string& url = "", name domain = ""_n, name collection = ""_n, uint32_t templateid = 0) { 

    // --- Check if submitter is in providers table --- //
    require_auth(submitter): 

    bool is_nft = (collection != ""_n) && !templateid;

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

        // --- Validate and format Latitude and Longitude --- //
        auto formatted_coords = validate_and_format_coords({latitude, longitude});
        latitude = formatted_coords[0];
        longitude = formatted_coords[1];


        // Insert new content
        contents.emplace(submitter, [&](auto& row) {
            row.id = contents.available_primary_key();
            row.domain = domain;
            row.submitter = submitter;
            row.link = url_chopped;
            row.external_id = 0;
            row.gudahash = url_hash;
            row.created = time_point_sec::sec_since_epoch();
            row.latlng = latlng;
            row.tetra_loc = tetra_locode;
        });

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
        
        //row.latlng = latlng;
        //row.tetra_loc = tetra_locode;
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

bool is_valid_continent_subregion(uint32_t code, const std::string& name = "") {
    if (!name.empty()) {
        std::string enum_name = normalize_enum_name(name);
        if (enum_name == "WORLD") return true;
        else if (enum_name == "SOUTHERN_ASIA") return true;
        else if (enum_name == "EASTERN_ASIA") return true;
        else if (enum_name == "SOUTH_EASTERN_ASIA") return true;
        else if (enum_name == "EASTERN_AFRICA") return true;
        else if (enum_name == "SOUTH_AMERICA") return true;
        else if (enum_name == "WESTERN_AFRICA") return true;
        else if (enum_name == "NORTHERN_AMERICA") return true;
        else if (enum_name == "EASTERN_EUROPE") return true;
        else if (enum_name == "WESTERN_ASIA") return true;
        else if (enum_name == "NORTHERN_AFRICA") return true;
        else if (enum_name == "WESTERN_EUROPE") return true;
        else if (enum_name == "MIDDLE_AFRICA") return true;
        else if (enum_name == "CENTRAL_AMERICA") return true;
        else if (enum_name == "SOUTHERN_EUROPE") return true;
        else if (enum_name == "NORTHERN_EUROPE") return true;
        else if (enum_name == "CENTRAL_ASIA") return true;
        else if (enum_name == "SOUTHERN_AFRICA") return true;
        else if (enum_name == "CARIBBEAN") return true;
        else if (enum_name == "AUSTRALIA_AND_NEW_ZEALAND") return true;
        else if (enum_name == "MELANESIA") return true;
        else if (enum_name == "POLYNESIA") return true;
        else if (enum_name == "MICRONESIA") return true;
        else if (enum_name == "ANTARCTICA") return true;
        else return false;
    } else {
        switch(code) {
            case 1:
            case 34:
            case 30:
            case 35:
            case 14:
            case 5:
            case 11:
            case 21:
            case 151:
            case 145:
            case 15:
            case 155:
            case 17:
            case 13:
            case 39:
            case 154:
            case 143:
            case 18:
            case 29:
            case 53:
            case 54:
            case 61:
            case 57:
            case 10:
                return true;
            default: return false;
        }
    }
}

bool is_valid_country(uint32_t code = 0, const std::string& name = "") {
    if (!name.empty()) {
        std::string enum_name = normalize_enum_name(name);
        if (enum_name == "AFG") return true;
        else if (enum_name == "ALA") return true;
        else if (enum_name == "ALB") return true;
        else if (enum_name == "DZA") return true;
        else if (enum_name == "ASM") return true;
        else if (enum_name == "AND") return true;
        else if (enum_name == "AGO") return true;
        else if (enum_name == "AIA") return true;
        else if (enum_name == "ATA") return true;
        else if (enum_name == "ATG") return true;
        else if (enum_name == "ARG") return true;
        else if (enum_name == "ARM") return true;
        else if (enum_name == "ABW") return true;
        else if (enum_name == "AUS") return true;
        else if (enum_name == "AUT") return true;
        else if (enum_name == "AZE") return true;
        else if (enum_name == "BHS") return true;
        else if (enum_name == "BHR") return true;
        else if (enum_name == "BGD") return true;
        else if (enum_name == "BRB") return true;
        else if (enum_name == "BLR") return true;
        else if (enum_name == "BEL") return true;
        else if (enum_name == "BLZ") return true;
        else if (enum_name == "BEN") return true;
        else if (enum_name == "BMU") return true;
        else if (enum_name == "BTN") return true;
        else if (enum_name == "BOL") return true;
        else if (enum_name == "BES") return true;
        else if (enum_name == "BIH") return true;
        else if (enum_name == "BWA") return true;
        else if (enum_name == "BVT") return true;
        else if (enum_name == "BRA") return true;
        else if (enum_name == "IOT") return true;
        else if (enum_name == "VGB") return true;
        else if (enum_name == "BRN") return true;
        else if (enum_name == "BGR") return true;
        else if (enum_name == "BFA") return true;
        else if (enum_name == "BDI") return true;
        else if (enum_name == "CPV") return true;
        else if (enum_name == "KHM") return true;
        else if (enum_name == "CMR") return true;
        else if (enum_name == "CAN") return true;
        else if (enum_name == "CYM") return true;
        else if (enum_name == "CAF") return true;
        else if (enum_name == "TCD") return true;
        else if (enum_name == "CHL") return true;
        else if (enum_name == "CHN") return true;
        else if (enum_name == "HKG") return true;
        else if (enum_name == "MAC") return true;
        else if (enum_name == "CXR") return true;
        else if (enum_name == "CCK") return true;
        else if (enum_name == "COL") return true;
        else if (enum_name == "COM") return true;
        else if (enum_name == "COG") return true;
        else if (enum_name == "COD") return true;
        else if (enum_name == "COK") return true;
        else if (enum_name == "CRI") return true;
        else if (enum_name == "CIV") return true;
        else if (enum_name == "HRV") return true;
        else if (enum_name == "CUB") return true;
        else if (enum_name == "CUW") return true;
        else if (enum_name == "CYP") return true;
        else if (enum_name == "CZE") return true;
        else if (enum_name == "DNK") return true;
        else if (enum_name == "DJI") return true;
        else if (enum_name == "DMA") return true;
        else if (enum_name == "DOM") return true;
        else if (enum_name == "ECU") return true;
        else if (enum_name == "EGY") return true;
        else if (enum_name == "SLV") return true;
        else if (enum_name == "GNQ") return true;
        else if (enum_name == "ERI") return true;
        else if (enum_name == "EST") return true;
        else if (enum_name == "SWZ") return true;
        else if (enum_name == "ETH") return true;
        else if (enum_name == "FLK") return true;
        else if (enum_name == "FRO") return true;
        else if (enum_name == "FJI") return true;
        else if (enum_name == "FIN") return true;
        else if (enum_name == "FRA") return true;
        else if (enum_name == "GUF") return true;
        else if (enum_name == "PYF") return true;
        else if (enum_name == "ATF") return true;
        else if (enum_name == "GAB") return true;
        else if (enum_name == "GMB") return true;
        else if (enum_name == "GEO") return true;
        else if (enum_name == "DEU") return true;
        else if (enum_name == "GHA") return true;
        else if (enum_name == "GIB") return true;
        else if (enum_name == "GRC") return true;
        else if (enum_name == "GRL") return true;
        else if (enum_name == "GRD") return true;
        else if (enum_name == "GLP") return true;
        else if (enum_name == "GUM") return true;
        else if (enum_name == "GTM") return true;
        else if (enum_name == "GGY") return true;
        else if (enum_name == "GIN") return true;
        else if (enum_name == "GNB") return true;
        else if (enum_name == "GUY") return true;
        else if (enum_name == "HTI") return true;
        else if (enum_name == "HMD") return true;
        else if (enum_name == "VAT") return true;
        else if (enum_name == "HND") return true;
        else if (enum_name == "HUN") return true;
        else if (enum_name == "ISL") return true;
        else if (enum_name == "IND") return true;
        else if (enum_name == "IDN") return true;
        else if (enum_name == "IRN") return true;
        else if (enum_name == "IRQ") return true;
        else if (enum_name == "IRL") return true;
        else if (enum_name == "IMN") return true;
        else if (enum_name == "ISR") return true;
        else if (enum_name == "ITA") return true;
        else if (enum_name == "JAM") return true;
        else if (enum_name == "JPN") return true;
        else if (enum_name == "JEY") return true;
        else if (enum_name == "JOR") return true;
        else if (enum_name == "KAZ") return true;
        else if (enum_name == "KEN") return true;
        else if (enum_name == "KIR") return true;
        else if (enum_name == "PRK") return true;
        else if (enum_name == "KOR") return true;
        else if (enum_name == "KWT") return true;
        else if (enum_name == "KGZ") return true;
        else if (enum_name == "LAO") return true;
        else if (enum_name == "LVA") return true;
        else if (enum_name == "LBN") return true;
        else if (enum_name == "LSO") return true;
        else if (enum_name == "LBR") return true;
        else if (enum_name == "LBY") return true;
        else if (enum_name == "LIE") return true;
        else if (enum_name == "LTU") return true;
        else if (enum_name == "LUX") return true;
        else if (enum_name == "MDG") return true;
        else if (enum_name == "MWI") return true;
        else if (enum_name == "MYS") return true;
        else if (enum_name == "MDV") return true;
        else if (enum_name == "MLI") return true;
        else if (enum_name == "MLT") return true;
        else if (enum_name == "MHL") return true;
        else if (enum_name == "MTQ") return true;
        else if (enum_name == "MRT") return true;
        else if (enum_name == "MUS") return true;
        else if (enum_name == "MYT") return true;
        else if (enum_name == "MEX") return true;
        else if (enum_name == "FSM") return true;
        else if (enum_name == "MCO") return true;
        else if (enum_name == "MNG") return true;
        else if (enum_name == "MNE") return true;
        else if (enum_name == "MSR") return true;
        else if (enum_name == "MAR") return true;
        else if (enum_name == "MOZ") return true;
        else if (enum_name == "MMR") return true;
        else if (enum_name == "NAM") return true;
        else if (enum_name == "NRU") return true;
        else if (enum_name == "NPL") return true;
        else if (enum_name == "NLD") return true;
        else if (enum_name == "NCL") return true;
        else if (enum_name == "NZL") return true;
        else if (enum_name == "NIC") return true;
        else if (enum_name == "NER") return true;
        else if (enum_name == "NGA") return true;
        else if (enum_name == "NIU") return true;
        else if (enum_name == "NFK") return true;
        else if (enum_name == "MNP") return true;
        else if (enum_name == "NOR") return true;
        else if (enum_name == "OMN") return true;
        else if (enum_name == "PAK") return true;
        else if (enum_name == "PLW") return true;
        else if (enum_name == "PSE") return true;
        else if (enum_name == "PAN") return true;
        else if (enum_name == "PNG") return true;
        else if (enum_name == "PRY") return true;
        else if (enum_name == "PER") return true;
        else if (enum_name == "PHL") return true;
        else if (enum_name == "PCN") return true;
        else if (enum_name == "POL") return true;
        else if (enum_name == "PRT") return true;
        else if (enum_name == "PRI") return true;
        else if (enum_name == "QAT") return true;
        else if (enum_name == "REU") return true;
        else if (enum_name == "ROU") return true;
        else if (enum_name == "RUS") return true;
        else if (enum_name == "RWA") return true;
        else if (enum_name == "BLM") return true;
        else if (enum_name == "SHN") return true;
        else if (enum_name == "KNA") return true;
        else if (enum_name == "LCA") return true;
        else if (enum_name == "MAF") return true;
        else if (enum_name == "SPM") return true;
        else if (enum_name == "VCT") return true;
        else if (enum_name == "WSM") return true;
        else if (enum_name == "SMR") return true;
        else if (enum_name == "STP") return true;
        else if (enum_name == "SAU") return true;
        else if (enum_name == "SEN") return true;
        else if (enum_name == "SRB") return true;
        else if (enum_name == "SYC") return true;
        else if (enum_name == "SLE") return true;
        else if (enum_name == "SGP") return true;
        else if (enum_name == "SXM") return true;
        else if (enum_name == "SVK") return true;
        else if (enum_name == "SVN") return true;
        else if (enum_name == "SLB") return true;
        else if (enum_name == "SOM") return true;
        else if (enum_name == "ZAF") return true;
        else if (enum_name == "SGS") return true;
        else if (enum_name == "SSD") return true;
        else if (enum_name == "ESP") return true;
        else if (enum_name == "LKA") return true;
        else if (enum_name == "SDN") return true;
        else if (enum_name == "SUR") return true;
        else if (enum_name == "SJM") return true;
        else if (enum_name == "SWE") return true;
        else if (enum_name == "CHE") return true;
        else if (enum_name == "SYR") return true;
        else if (enum_name == "TWN") return true;
        else if (enum_name == "TJK") return true;
        else if (enum_name == "TZA") return true;
        else if (enum_name == "THA") return true;
        else if (enum_name == "TLS") return true;
        else if (enum_name == "TGO") return true;
        else if (enum_name == "TKL") return true;
        else if (enum_name == "TON") return true;
        else if (enum_name == "TTO") return true;
        else if (enum_name == "TUN") return true;
        else if (enum_name == "TUR") return true;
        else if (enum_name == "TKM") return true;
        else if (enum_name == "TCA") return true;
        else if (enum_name == "TUV") return true;
        else if (enum_name == "UGA") return true;
        else if (enum_name == "UKR") return true;
        else if (enum_name == "ARE") return true;
        else if (enum_name == "GBR") return true;
        else if (enum_name == "USA") return true;
        else if (enum_name == "UMI") return true;
        else if (enum_name == "URY") return true;
        else if (enum_name == "UZB") return true;
        else if (enum_name == "VUT") return true;
        else if (enum_name == "VEN") return true;
        else if (enum_name == "VNM") return true;
        else if (enum_name == "VGB") return true;
        else if (enum_name == "VIR") return true;
        else if (enum_name == "WLF") return true;
        else if (enum_name == "ESH") return true;
        else if (enum_name == "YEM") return true;
        else if (enum_name == "ZMB") return true;
        else if (enum_name == "ZWE") return true;
        else return false;
    } else if (code) {
        switch(code) {
            case 4: return true;    // Afghanistan
            case 248: return true;  // Åland Islands
            case 8: return true;    // Albania
            case 12: return true;   // Algeria
            case 16: return true;   // American Samoa
            case 20: return true;   // Andorra
            case 24: return true;   // Angola
            case 660: return true;  // Anguilla
            case 10: return true;   // Antarctica
            case 28: return true;   // Antigua and Barbuda
            case 32: return true;   // Argentina
            case 51: return true;   // Armenia
            case 533: return true;  // Aruba
            case 36: return true;   // Australia
            case 40: return true;   // Austria
            case 31: return true;   // Azerbaijan
            case 44: return true;   // Bahamas
            case 48: return true;   // Bahrain
            case 50: return true;   // Bangladesh
            case 52: return true;   // Barbados
            case 112: return true;  // Belarus
            case 56: return true;   // Belgium
            case 84: return true;   // Belize
            case 204: return true;  // Benin
            case 60: return true;   // Bermuda
            case 64: return true;   // Bhutan
            case 68: return true;   // Bolivia (Plurinational State of)
            case 535: return true;  // Bonaire, Sint Eustatius and Saba
            case 70: return true;   // Bosnia and Herzegovina
            case 72: return true;   // Botswana
            case 74: return true;   // Bouvet Island
            case 76: return true;   // Brazil
            case 86: return true;   // British Indian Ocean Territory
            case 92: return true;   // British Virgin Islands
            case 96: return true;   // Brunei Darussalam
            case 100: return true;  // Bulgaria
            case 854: return true;  // Burkina Faso
            case 108: return true;  // Burundi
            case 132: return true;  // Cabo Verde
            case 116: return true;  // Cambodia
            case 120: return true;  // Cameroon
            case 124: return true;  // Canada
            case 136: return true;  // Cayman Islands
            case 140: return true;  // Central African Republic
            case 148: return true;  // Chad
            case 152: return true;  // Chile
            case 156: return true;  // China
            case 344: return true;  // China, Hong Kong Special Administrative Region
            case 446: return true;  // China, Macao Special Administrative Region
            case 162: return true;  // Christmas Island
            case 166: return true;  // Cocos (Keeling) Islands
            case 170: return true;  // Colombia
            case 174: return true;  // Comoros
            case 178: return true;  // Congo
            case 184: return true;  // Cook Islands
            case 188: return true;  // Costa Rica
            case 384: return true;  // Côte d'Ivoire
            case 191: return true;  // Croatia
            case 192: return true;  // Cuba
            case 531: return true;  // Curaçao
            case 196: return true;  // Cyprus
            case 203: return true;  // Czechia
            case 408: return true;  // Democratic People's Republic of Korea
            case 180: return true;  // Democratic Republic of the Congo
            case 208: return true;  // Denmark
            case 262: return true;  // Djibouti
            case 212: return true;  // Dominica
            case 214: return true;  // Dominican Republic
            case 218: return true;  // Ecuador
            case 818: return true;  // Egypt
            case 222: return true;  // El Salvador
            case 226: return true;  // Equatorial Guinea
            case 232: return true;  // Eritrea
            case 233: return true;  // Estonia
            case 748: return true;  // Eswatini
            case 231: return true;  // Ethiopia
            case 238: return true;  // Falkland Islands (Malvinas)
            case 234: return true;  // Faroe Islands
            case 242: return true;  // Fiji
            case 246: return true;  // Finland
            case 250: return true;  // France
            case 254: return true;  // French Guiana
            case 258: return true;  // French Polynesia
            case 260: return true;  // French Southern Territories
            case 266: return true;  // Gabon
            case 270: return true;  // Gambia
            case 268: return true;  // Georgia
            case 276: return true;  // Germany
            case 288: return true;  // Ghana
            case 292: return true;  // Gibraltar
            case 300: return true;  // Greece
            case 304: return true;  // Greenland
            case 308: return true;  // Grenada
            case 312: return true;  // Guadeloupe
            case 316: return true;  // Guam
            case 320: return true;  // Guatemala
            case 831: return true;  // Guernsey
            case 324: return true;  // Guinea
            case 624: return true;  // Guinea-Bissau
            case 328: return true;  // Guyana
            case 332: return true;  // Haiti
            case 334: return true;  // Heard Island and McDonald Islands
            case 336: return true;  // Holy See
            case 340: return true;  // Honduras
            case 348: return true;  // Hungary
            case 352: return true;  // Iceland
            case 356: return true;  // India
            case 360: return true;  // Indonesia
            case 364: return true;  // Iran (Islamic Republic of)
            case 368: return true;  // Iraq
            case 372: return true;  // Ireland
            case 833: return true;  // Isle of Man
            case 376: return true;  // Israel
            case 380: return true;  // Italy
            case 388: return true;  // Jamaica
            case 392: return true;  // Japan
            case 832: return true;  // Jersey
            case 400: return true;  // Jordan
            case 398: return true;  // Kazakhstan
            case 404: return true;  // Kenya
            case 296: return true;  // Kiribati
            case 414: return true;  // Kuwait
            case 417: return true;  // Kyrgyzstan
            case 418: return true;  // Lao People's Democratic Republic
            case 428: return true;  // Latvia
            case 422: return true;  // Lebanon
            case 426: return true;  // Lesotho
            case 430: return true;  // Liberia
            case 434: return true;  // Libya
            case 438: return true;  // Liechtenstein
            case 440: return true;  // Lithuania
            case 442: return true;  // Luxembourg
            case 450: return true;  // Madagascar
            case 454: return true;  // Malawi
            case 458: return true;  // Malaysia
            case 462: return true;  // Maldives
            case 466: return true;  // Mali
            case 470: return true;  // Malta
            case 584: return true;  // Marshall Islands
            case 474: return true;  // Martinique
            case 478: return true;  // Mauritania
            case 480: return true;  // Mauritius
            case 175: return true;  // Mayotte
            case 484: return true;  // Mexico
            case 583: return true;  // Micronesia (Federated States of)
            case 492: return true;  // Monaco
            case 496: return true;  // Mongolia
            case 499: return true;  // Montenegro
            case 500: return true;  // Montserrat
            case 504: return true;  // Morocco
            case 508: return true;  // Mozambique
            case 104: return true;  // Myanmar
            case 516: return true;  // Namibia
            case 520: return true;  // Nauru
            case 524: return true;  // Nepal
            case 528: return true;  // Netherlands (Kingdom of the)
            case 540: return true;  // New Caledonia
            case 554: return true;  // New Zealand
            case 558: return true;  // Nicaragua
            case 562: return true;  // Niger
            case 566: return true;  // Nigeria
            case 570: return true;  // Niue
            case 574: return true;  // Norfolk Island
            case 807: return true;  // North Macedonia
            case 580: return true;  // Northern Mariana Islands
            case 578: return true;  // Norway
            case 512: return true;  // Oman
            case 586: return true;  // Pakistan
            case 585: return true;  // Palau
            case 591: return true;  // Panama
            case 598: return true;  // Papua New Guinea
            case 600: return true;  // Paraguay
            case 604: return true;  // Peru
            case 608: return true;  // Philippines
            case 612: return true;  // Pitcairn
            case 616: return true;  // Poland
            case 620: return true;  // Portugal
            case 630: return true;  // Puerto Rico
            case 634: return true;  // Qatar
            case 498: return true;  // Republic of Moldova
            case 638: return true;  // Réunion
            case 642: return true;  // Romania
            case 643: return true;  // Russian Federation
            case 646: return true;  // Rwanda
            case 652: return true;  // Saint Barthélemy
            case 654: return true;  // Saint Helena
            case 659: return true;  // Saint Kitts and Nevis
            case 662: return true;  // Saint Lucia
            case 663: return true;  // Saint Martin (French Part)
            case 666: return true;  // Saint Pierre and Miquelon
            case 670: return true;  // Saint Vincent and the Grenadines
            case 882: return true;  // Samoa
            case 674: return true;  // San Marino
            case 678: return true;  // Sao Tome and Principe
            case 682: return true;  // Saudi Arabia
            case 686: return true;  // Senegal
            case 688: return true;  // Serbia
            case 690: return true;  // Seychelles
            case 694: return true;  // Sierra Leone
            case 702: return true;  // Singapore
            case 534: return true;  // Sint Maarten (Dutch part)
            case 703: return true;  // Slovakia
            case 705: return true;  // Slovenia
            case 90: return true;   // Solomon Islands
            case 706: return true;  // Somalia
            case 710: return true;  // South Africa
            case 239: return true;  // South Georgia and the South Sandwich Islands
            case 728: return true;  // South Sudan
            case 724: return true;  // Spain
            case 144: return true;  // Sri Lanka
            case 275: return true;  // State of Palestine
            case 729: return true;  // Sudan
            case 740: return true;  // Suriname
            case 744: return true;  // Svalbard and Jan Mayen Islands
            case 752: return true;  // Sweden
            case 756: return true;  // Switzerland
            case 760: return true;  // Syrian Arab Republic
            case 762: return true;  // Tajikistan
            case 764: return true;  // Thailand
            case 626: return true;  // Timor-Leste
            case 768: return true;  // Togo
            case 772: return true;  // Tokelau
            case 776: return true;  // Tonga
            case 780: return true;  // Trinidad and Tobago
            case 788: return true;  // Tunisia
            case 792: return true;  // Türkiye
            case 795: return true;  // Turkmenistan
            case 796: return true;  // Turks and Caicos Islands
            case 798: return true;  // Tuvalu
            case 800: return true;  // Uganda
            case 804: return true;  // Ukraine
            case 784: return true;  // United Arab Emirates
            case 826: return true;  // United Kingdom of Great Britain and Northern Ireland
            case 834: return true;  // United Republic of Tanzania
            case 581: return true;  // United States Minor Outlying Islands
            case 840: return true;  // United States of America
            case 850: return true;  // United States Virgin Islands
            case 858: return true;  // Uruguay
            case 860: return true;  // Uzbekistan
            case 548: return true;  // Vanuatu
            case 862: return true;  // Venezuela (Bolivarian Republic of)
            case 704: return true;  // Viet Nam
            case 876: return true;  // Wallis and Futuna Islands
            case 732: return true;  // Western Sahara
            case 887: return true;  // Yemen
            case 894: return true;  // Zambia
            case 716: return true;  // Zimbabwe
            default: return false;
        } else {
          check(0, "Please provide a integer code or a 3-digit country code")
        }
    }
}


vector<float> validate_and_format_coords(const std::vector<float>& coords) {
    check(coords.size() == 2, "Coordinates vector must contain two decimal numbers, first being latitude, second longitude.");
    float latitude = coords[0];
    float longitude = coords[1];

    // Validate Latitude and Longitude
    check(latitude >= -90.0f && latitude <= 90.0f, "Latitude must be between -90 and 90.");
    check(longitude >= -180.0f && longitude <= 180.0f, "Longitude must be between -180 and 180.");

    // Format to 4 decimal places
    latitude = static_cast<float>(static_cast<int>(latitude * 10000)) / 10000.0f;
    longitude = static_cast<float>(static_cast<int>(longitude * 10000)) / 10000.0f;

    return {latitude, longitude};
}