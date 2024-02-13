/*/ ---

This file handles the dispatch of tokens, etc in a standardized way.
This allows us to be more flexible with what constitutes an up:
1) A token transfer with an up| memo
2) A token stansfer with just a name that's a content ID (no special memo)
3) An Up action on a contract that has been registered as a "content id"

/*/// ---



// --- DISPATCHER Checks + calls logup() updateiou() and updatetotal() --- //
void ups::upsertup(uint32_t upscount, name upsender, uint64_t content_id, bool negative) {

    // --- Check content Id valid

    if (!negative){
        // --- Log the ups in ups table --- // 
        upsert_logup(upscount, upsender, content_id, negative);
        
        // --- Calls action to update the TOTALS table -- //
        upsert_total(upscount, upsender, content_id, negative);

        //  --- Call action to update IOU table ----- //
        ious::updateiou(upscount, upsender, content_id, false);
    }
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
  require_auth( upsender );

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
      row.lastup = eosio::time_point_sec::sec_since_epoch();
      row.totalups += newups;
    });
  }//END if(results _uppers)
}//END upsert_total()


void removeiou(name sender, name receiver) {
    
}

void upsertupper(uint32_t upscount, name upsender) {
    
}

void removeupper(name upsender) {
    
}

void removecont(uint64_t content_id) {
    
}

void addcontent(name& submitter, string& url) {
    name domain = parse_url_for_domain(url);

    // dont forget :       row.id = _ups.available_primary_key();
}

void deepremvcont(uint64_t content_id) {
    
}


// --- Returns the current Time Unit --- //
uint32_t find_tu(uint32_t& momentuin, uint32_t tu_length){
  // 1561139700 is the first Time Unit in Seconds
  uint32_t time_unit = floor((momentuin - 1561139700) / tu_length);  // Divide by the length of a Time Unit in seconds
  return time_unit;
}

// --- Returns the current Time Unit --- //
uint32_t find_tu(uint32_t tu_length){
  uint32_t momentuin = eosio::time_point_sec::sec_since_epoch();
  uint32_t time_unit = floor((momentuin - 1561139700) / tu_length);  // Divide by the length of a Time Unit in seconds
  return time_unit;
}

// --- Get content_id from iouid (concat of tuid + content_id) --- //
uint32_t iouid_to_contentid(uint32_t& iouid){ //
  uint32_t content_id = (uint32_t) iouid;
  return content_id;
}

// --- Get Tuid from iouif (concat of tuid + content_id) --- //
uint32_t iouid_to_tuid(uint32_t& iouid){
  uint32_t tuid = (uint32_t iouid>>32);
  return tuid;
}


name parse_url_for_domain(const string& url) const {
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

    // Extract the domain part after "://" and "www."
    string domain_part = url.substr(start);

    // Find the first slash after the domain part to ensure only the domain is included
    auto end = domain_part.find('/');
    if (end != string::npos) {
        domain_part = domain_part.substr(0, end);
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
}

// --- Gets config object and ensures contract not paused --- //
auto check_config(bool ignore_empty = false) // --- RETURNS false or config type
{
    // --- Get config table --- //
    config_table conf_tbl(get_self(), get_self().value);

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

};
