#include "helphelpups.cpp"


/*/ --- 

This file handles the dispatch of tokens, etc in a standardized way.
This allows us to be more flexible with what constitutes an up:
1) A token transfer with an up| memo
2) A token stansfer with just a name that's a content ID (no special memo)
3) An Up action on a contract that has been registered as a "content id" (no action for this yet)

/*/// ---



// --- DISPATCHER Checks + calls logup() updateiou() and updatetotal() --- //
void ups::upsertup(uint32_t upscount, name upsender, uint64_t contentid, bool negative = 0) {
      require_auth( upsender );
    // --- Check content Id valid --- //
    if (!negative){// TODO remove this + test
        // --- Log the ups in ups table --- // 
        ups::upsert_logup(upscount, upsender, contentid, negative);
        
        // --- Calls action to update the TOTALS table -- //
        ups::upsert_total(upscount, upsender, contentid, negative);

        //  --- Call action to update IOU table ----- //
        ups::upsert_ious(upscount, upsender, contentid, false);
    } //TODO else call removal functions (same with subtract bool)
}//END upsertup()

// --- ROUTER prepares and calls upsertup() --- //
void ups::upsertup_url(uint32_t upscount, name upsender, string url ) {
    // Extract the domain name from the URL to use as scope for the content table
    //inline auto get_domain = parse_url(url, 0, 0, 1); 
    name domain = url_domain_name(url);
    //delete get_domain;
    

    // Use the domain to scope the content table
    content_t contents(get_self(), get_self().value);

    // --- Get the hash of the URL --- //
    checksum256 url_hash = ups::url_hash(url);

    //auto parse_url(const string& url) -> checksum256;

    // Search for the content by its hash within the scoped content table
    auto by_gudahash_index = contents.get_index<"bygudahash"_n>();
    auto content_itr = by_gudahash_index.lower_bound(url_hash);
    check(content_itr != by_gudahash_index.end(), "⚡️ Linked content not found. Please register the provider + content URL before sending ups.");

    uint64_t contentid = content_itr->contentid;
    upsertup(upscount, upsender, contentid, false);

}//END upsertup_url()


// --- ROUTER prepares and calls upsertup() --- //
void ups::upsertup_nft(uint32_t upscount, name upsender, name collection, int32_t templateid) {

  // --- Ensure collection matches for correct contentid --- //
  content_t content_tbl(get_self(), get_self().value);
  auto by_external_id_idx = content_tbl.get_index<"byextid"_n>();
  auto content_itr = by_external_id_idx.find(templateid);
  check(content_itr != by_external_id_idx.end() && content_itr->domain == collection, "⚡️ NFT content not found or collection mismatch");

  // Pass the contentid to upsertup to update the ups
  upsertup(upscount, upsender, content_itr->contentid, 0);

}//END upsertup_nft()

// --- Update running log of ups --- // TODO update to this contract
void ups::upsert_logup(uint32_t upscount, name upsender, uint64_t contentid, bool negative){
  //NOTE negative should only be called for deletions (user gets removed from system)

  uint32_t now_tu = find_tu(eosio::current_time_point().sec_since_epoch());

  // --- Add record to _upslog --- //
  upslog_t _upslog(get_self(), get_self().value);
  auto by_contentid_idx = _upslog.get_index<"bycontentid"_n>();
  auto ups_itr = by_contentid_idx.lower_bound(contentid);


//TODO let user cancel their up befote timeunit expires, just action that calls this function with negative flag
//TODO double-check we are updating the other totals table
  // Iterate through entries to find a match with upsender and now_tu
  bool found_up = false;
  for (; ups_itr != by_contentid_idx.end(); ++ups_itr) {
      if (ups_itr->upsender == upsender && ups_itr->tuid == now_tu) {
          // Matching entry found, update its totalups
          by_contentid_idx.modify(ups_itr, get_self(), [&](auto& row) {
              row.totalups = negative ? row.totalups - upscount : row.totalups + upscount;
          });
          found_up = true;
          break; // Exit the function after updating
      }
  }

  if( !found_up )
  { // -- Make New Record
    _upslog.emplace(get_self(), [&]( auto& row ) {
      row.upid = _upslog.available_primary_key();
      row.contentid = contentid;
      row.upsender = upsender;
      row.totalups = upscount;
      row.tuid = now_tu;
    });
  } 

}

// --- Upsert _uppers and _totals --- // TODO update to this contract
void ups::upsert_total(uint32_t upscount, name upsender, uint64_t contentid, bool negative){

  // --- Update / Insert _totals record of cumulative song Ups --- //
  totals_t _totals(get_self(), contentid);
  auto total_iterator = _totals.begin();
  uint32_t time_of_up = eosio::current_time_point().sec_since_epoch();
  if( total_iterator == _totals.end())
  { // -- Make New Record
    _totals.emplace(get_self(), [&]( auto& row ) {
      row.contentid = contentid;
      row.totalups = upscount;
      row.updated = time_of_up;
    });
  } 
  else 
  { // -- Update Record 
    if(!negative){
      _totals.modify(total_iterator, get_self(), [&]( auto& row ) {
        row.contentid = contentid;
        row.totalups += upscount;
        row.updated = time_of_up;
      });
    } else { // Subtract the value from totals
      _totals.modify(total_iterator, get_self(), [&]( auto& row ) {
        row.contentid = contentid;
        row.totalups -= upscount;
        row.updated = time_of_up;
      });
    }

  }//END if(results _totals)

  // --- Update / Insert _uppers record --- //
  uppers_t _uppers(get_self(), get_self().value);
  auto listener_iterator = _uppers.find(contentid);
  if( listener_iterator == _uppers.end() )
  {
    _uppers.emplace(get_self(), [&]( auto& row ) {
      row.upsender = upsender;
      row.firstup = time_of_up;
      row.lastup = time_of_up;
      row.claimable = row.claimable + upscount;
      row.totalups = upscount;
    });
  } 
  else 
  {
    _uppers.modify(listener_iterator, get_self(), [&]( auto& row ) {
      row.lastup = time_of_up;
      row.totalups += upscount;
      row.claimable = row.claimable + upscount;
    });
  }//END if(results _uppers)
}//END upsert_total()

// --- Upsert IOUs --- //
void ups::upsert_ious(uint32_t upscount, name upsender, uint64_t contentid, bool subtract) {
    //require_auth(get_self()); //CHECK this would make it fail unless we call it?

    // --- Access the config table and ensure rewards are happening --- //
    config conf = check_config(); 
    if (!conf.pay_submitter && !conf.pay_upsender){return;}

    // --- Find submitter name to pay them --- //
    content_t _contents(get_self(), get_self().value);
    auto content_itr = _contents.find(contentid);
    eosio::check(content_itr != _contents.end(), "⚡️ Content not found");
    name submitter = content_itr->submitter;

    // --- Find current time sec --- //
    uint32_t current_time = eosio::current_time_point().sec_since_epoch();

    // --- Upsert IOU for a given receiver and sender --- //
      auto upsert_iou = [&](name receiver, bool subtract) {
        ious_t ious_table(get_self(), receiver.value);
        auto iou_idx = ious_table.get_index<"bycontentid"_n>();
        auto iou_itr = iou_idx.lower_bound(contentid);

        if (iou_itr != iou_idx.end()) {
            // Found matching IOU record, update it
            iou_idx.modify(iou_itr, get_self(), [&](auto& row) {
                row.upscount = subtract ? row.upscount - upscount : row.upscount + upscount;
                row.updated = current_time;
            });
            return;
        }
        // --- Insert new IOU --- //
        if (!subtract) {
            ious_table.emplace(get_self(), [&](auto& row) {
                row.iouid = ious_table.available_primary_key();
                row.contentid = contentid;
                row.upcatcher = receiver;
                row.upscount = upscount;
                row.initiated = current_time;
                row.updated = current_time;
            });
        }
    };//END lambda

    // --- Pay the good people --- //
    if (conf.pay_submitter) {
        upsert_iou(submitter, subtract);
    }
    if (conf.pay_upsender && submitter != upsender) {
        upsert_iou(upsender, subtract);
    }

}//END upsert_ious()


// --- Send the beautiful people their tokens  --- //
void ups::pay_iou(uint32_t maxpayments = 19, name receiver = ""_n, bool paythem = true){

  check(receiver != ""_n, "⚡️ We can't pay no one.");

  // --- Check that the rewards aren't paused --- //
  config conf = check_config();
  check(!conf.paused_rewards, "⚡️ Rewards are currently paused. Check back later.");

  // Find the receiver records is in the ious_table table

  // --- Get the IOUs --- //
  ups::ious_t ious_table(get_self(), receiver.value); 
  auto iou_itr = ious_table.begin();
  check(iou_itr != ious_table.end(), "You are all paid up. Send some Ups and come back ⚡️ ");
  
  // --- Calculate Payments --- //
  uint32_t paid = 0;
  std::vector<uint64_t> ious_to_erase;

  uint32_t records_processed = 0;
  // --- Iterate over the IOUs and accumulate payments until reaching maxpay or end of table --- //
  while(iou_itr != ious_table.end() && records_processed <= maxpayments){
    paid += iou_itr->upscount; 
    ious_to_erase.push_back(iou_itr->iouid); // Track IOU IDs for deletion
    iou_itr++;
  }

    // --- Calculate the total reward amount --- //
    asset total_payment = conf.one_reward_amount;
    total_payment *= paid * (conf.reward_multiplier_percent / 100);

      // --- Erase paid IOUs from the table --- //
  for(auto& iouid: ious_to_erase){
    auto itr = ious_table.find(iouid);
    if(itr != ious_table.end()){
        ious_table.erase(itr);
    }
  }

  // --- Pay the people --- //
  if(total_payment.amount > 0 && paythem){
    // Use the eosio.token transfer action to send the payment
    action(
        permission_level{get_self(), "active"_n},
        conf.reward_token_contract, 
        "transfer"_n,
        std::make_tuple(get_self(), get_self(), total_payment, string("Rewards for "+receiver.to_string()))
    ).send();
  }
}//END pay_iou()


// --- Handles adding both NFT content and URL content --- // TODO add to the new content_domain singleton
void ups::addcontent(name submitter, double latitude = 0.0, double longitude = 0.0, uint32_t continent_subregion_code = 0, uint32_t country_code = 0, const string& continent_subregion_name = "", const string& country_iso3 = "", uint32_t subdivision = 0, uint32_t postal_code = 0, const string url = "", name domain = ""_n, name collection = ""_n, int32_t templateid = 0)
{ 
    // --- Check if submitter is in providers table --- //
    require_auth(submitter);

    bool is_nft = (collection != ""_n) && !templateid;

    uint32_t subcontinent = 0;
    uint32_t country = 0;
    int32_t latitude_int = 0;
    int32_t longitude_int = 0;

    // --- Validate and format Latitude and Longitude --- //
    if (latitude != 0.0 || longitude != 0.0){
        vector<int32_t> formatted_coords = validate_and_format_coords({latitude, longitude});
        latitude_int = formatted_coords[0];
        longitude_int = formatted_coords[1];
    } 

    // --- Validate the Continent Subregion as a string or an int --- //
    if (!continent_subregion_name.empty() || continent_subregion_code != 0) {
        subcontinent = is_valid_continent_subregion(continent_subregion_code, continent_subregion_name);
    } 

    // --- Validate the country as a string or an int --- //
    if (!country_iso3.empty() || country_code != 0) {
        country = is_valid_country(country_code, country_iso3);
    } 
    
    // --- Handle URL --- //
    if ( !is_nft && url != "" ) {
      name domain = url_domain_name(url);
      checksum256 new_hash = url_hash(url);
      string url_chopped = chopped_url(url);

      // --- Check if domain is registered --- //
      content_provider_singleton content_prov(get_self(), domain.value);
      check(content_prov.exists(), "⚡️ Register the domain to start adding content for upvotes");

      // --- Check if content already exists --- //
        content_t contents(get_self(), get_self().value);
        auto gudhash = contents.get_index<"bygudahash"_n>();
        auto itr = gudhash.find(new_hash);

        check(itr == gudhash.end(), "⚡️ Content exists, send Ups now");

        // --- Insert NFT into content table -- //
        contents.emplace(get_self(), [&](auto& row) {
            row.contentid = contents.available_primary_key();
            row.domain = domain;
            row.submitter = submitter;
            row.link = url_chopped;
            row.external_id = 0;
            row.gudahash = new_hash;
            row.created = eosio::current_time_point().sec_since_epoch();
            row.latitude = latitude_int; // CHANGE and see if it compiles 
            row.longitude = longitude_int;
            row.subcontinent = (subcontinent != 0) ? subcontinent : 1;//Default to made-up world subcontinent
            row.country = country;
            row.subdivision = subdivision;
            row.postal_code = postal_code;
        });

    } else if ( is_nft ) {
      // --- Handle NFT --- //
      // --- Check the providers table --- //
      content_provider_singleton content_prov(get_self(), collection.value);

      // --- Ensure the collection is not already registered --- //
      check(content_prov.exists(), "⚡️ This collection is not registered. Use regnftcol first.");

      // --- Check if templateid is valid --- //
      atomicassets::templates_t templates_tbl(atomicassets::ATOMICASSETS_ACCOUNT, collection.value); /// CHECK mangled, should be 
      auto template_itr = templates_tbl.find(templateid);
      check(template_itr != templates_tbl.end(), "⚡️ Template does not exist");

      // --- Check if NFT already exists in content_t --- //
      ups::content_t _content(get_self(), get_self().value);
      auto by_external_id_idx = _content.get_index<"byextid"_n>(); // Assuming this is the secondary index for external_id
      auto nft_itr = by_external_id_idx.find(templateid);
      check(nft_itr == by_external_id_idx.end(), "⚡️ NFT is already registered. Send Ups.");

      // --- Find current time sec --- //
      uint32_t current_time = eosio::current_time_point().sec_since_epoch();

      // Insert new NFT content
      _content.emplace(get_self(), [&](auto& row) {
        row.contentid = _content.available_primary_key();
        row.domain = collection; // --- Using collection name as domain for NFTs
        row.submitter = submitter;
        row.external_id = templateid; // --- Set external_id to templateid for NFTs
        row.gudahash = checksum256(); 
        row.created = current_time;
        row.latitude = latitude_int; // CHANGE and see if it compiles 
        row.longitude = longitude_int;
        row.subcontinent = (subcontinent != 0) ? subcontinent : 1;
        row.country = country;
        row.subdivision = subdivision;
        row.postal_code = postal_code;
      });

    } else {
      check(0, "⚡️ This is not a valid URL or NFT");
    }
}


