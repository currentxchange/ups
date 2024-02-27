#include "ups.hpp"
#include "helpups.cpp"


ACTION ups::payup(name upsender = ""_n) {
    /*/ --- Require only the upsender to be able to claim rewards [Optional] --- //
    Commented out, action allows for anyone to call the action for contract to pay other people. Otherwise a person would be unable to claim if they were out of CPU. 
    /*/
    //check((has_auth(upsender) || has_auth(get_self())) , "Please put your account name.")
    // --- Send to the payment dispatcher --- //
    pay_iou(0, upsender);
    
}

ACTION ups::updatecont(name& submitter, uint64_t contentid, double latitude = 0.0, double longitude = 0.0, uint32_t continent_subregion_code = 0, uint32_t country_code = 0, const string& continent_subregion_name = "", const string& country_iso3 = "", uint32_t subdivision = 0, uint32_t postal_code = 0){

    // --- Get the content --- //
    content_t _contents(get_self(), get_self().value);
    auto itr = _contents.find(contentid);
    check(itr != _contents.end(), "⚡️ Content with the specified ID does not exist.");

    check(has_auth(itr->submitter) || has_auth(get_self()) , "⚡️ Only the submitter "+ itr->submitter.to_string() +" can update content.");

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
    if (!country_iso3.empty() || country_code != 0) {
        country = is_valid_country(country_code, country_iso3);
    } 

    // --- Update the content record --- //
    _contents.modify(itr, get_self(), [&](auto& row) {
        row.latitude = (latitude_int != 0) ? latitude : row.latitude; // CHANGE and see if it compiles 
        row.longitude = (longitude_int != 0) ? longitude : row.longitude;
        row.subcontinent = (subcontinent != 0) ? subcontinent : row.subcontinent;
        row.country = (country != 0) ? country : row.country;
        row.subdivision = (subdivision != 0) ? subdivision : row.subdivision;
        row.postal_code = (postal_code != 0) ? postal_code : row.postal_code;
    });

}//END updatecont()



ACTION ups::regdomain(const name& submitter, const string& url, const string& country_iso3) {

    check((has_auth(submitter) || has_auth(get_self())) , "Please put your account name as the submitter.");

    // ---- Get a name from the URL --- //
    name domain_parsed = url_domain_name(url);
    string domain_chopped = chopped_url(url);

    // ---- Check if domain is already registered --- //
    content_provider_singleton content_prov(get_self(), domain_parsed.value);
    eosio::check(!content_prov.exists(), "Content provider already registered for this domain");

    // --- Assign country code from is_valid_country() --- //
    uint32_t country_code = is_valid_country(0, country_iso3);
   
    // ---- Register new content provider --- //
    content_provider prov_data;
    prov_data.domain = domain_parsed;    
    prov_data.raw_domain = domain_chopped;
    prov_data.country = country_code;

    // Save the content provider information
    content_prov.set(prov_data, submitter);//CHECK
}

/*/----
  This functionality isn't included, but can be added   
  Optionally, you can scope the config table to the name of a domain (content provider liek a web platform or a NFT collection)
  Allows each content provider to have their own token for Ups + rewards

ACTION ups::configdomain(const name& submitter, const string& url, const name& up_token_contract, const symbol& up_token_symbol, const name& reward_token_contract, const symbol& reward_token_symbol, const asset& one_up_amount, const asset& one_reward_amount) {

}
/*///---

//TODO fix 
ACTION ups::regnftcol(const name& submitter, const name& collection, string& country) {
    // --- Check if collection exists + user is authorized  --- //
    check(has_auth(submitter), "The content submitter must sign."); 
    auto itrCollection = atomicassets::collections.require_find(collection.value, "No collection with this name exists.");

    // --- Require collection owners to register collection --- //
    //check(isAuthorized(collection, submitter), "Submitter is not authorized for this collection.");

    // -- Assign country code from is_valid_country() --- //
    uint32_t country_code = is_valid_country(0, country);

    // --- Check the providers table --- //
    content_provider_singleton content_prov(get_self(), collection.value);

    // --- Ensure the collection is not already registered --- //
    check(!content_prov.exists(), "Content provider already registered for this domain");

    // --- Register the collection --- //
    content_provider new_provider;
    new_provider.domain = collection;
    new_provider.raw_domain = collection.to_string();
    new_provider.country = country_code;

    content_prov.set(new_provider, submitter); // --- Submitter pays to register CHECK if this is correct or scope
}

ACTION ups::addurl(name submitter, const string& url, const name& domain, double latitude = 0.0, double longitude = 0.0, uint32_t continent_subregion_code = 0, uint32_t country_code = 0, const string& continent_subregion_name = "", const string& country_iso3 = "", uint32_t subdivision = 0, uint32_t postal_code = 0) { 
    
    name collection = ""_n;
    int32_t templateid = 0;
    config conf = check_config();

    // --- Uncomment to allow free registration of content --- //
    //TODO debug real memo 
    //check(/*/has_auth(submitter) || /*/has_auth(get_self()), "Add linked content by sending " +conf.one_up_amount +" " + conf.up_token_symbol +" with memo url|<your url>" ); //CHECK If this is the correct memo with the updated upcatcher
   check(/*/has_auth(submitter) || /*/has_auth(get_self()), "Add linked content by sending Up with memo url|<your url>" ); //CHECK If this is the correct memo with the updated upcatcher

    // --- Call dispatcher function --- // 
    addcontent(submitter, latitude, longitude, continent_subregion_code, country_code, continent_subregion_name, country_iso3, subdivision, postal_code, url, domain, collection, templateid);

    return;
}//END addurl

ACTION ups::addnft(name& submitter, double latitude = 0.0, double longitude = 0.0, uint32_t continent_subregion_code = 0, uint32_t country_code = 0, const string& continent_subregion_name = "", const string& country_iso3 = "", uint32_t subdivision = 0, uint32_t postal_code = 0, const name& collection = ""_n, const uint32_t& templateid = 0) { 
    string url = ""; 
    name domain = ""_n; 
    config conf = check_config();
    
    // --- Uncomment to allow free registration of content --- //
    //check(has_auth(get_self()), "Add NFTs to be ranked by sending " conf.one_up_amount.to_string() +" " + conf.up_token_symbol.to_string()+" with memo url|<your url>" ); //CHECK If this is the correct memo with the updated upcatcher
    check(has_auth(get_self()), "Add NFTs to be ranked by sending token with memo url|<your url>" ); //CHECK If this is the correct memo with the updated upcatcher
    // TODO Debug memo vars 


    // --- Call dispatcher function --- // 
    addcontent(submitter, latitude, longitude, continent_subregion_code, country_code, continent_subregion_name, country_iso3, subdivision, postal_code, url, domain, collection, templateid);

    return;
}//END addnft


//TODO WARN needs update to remove the up records
ACTION ups::remvcontent(uint64_t contentid = 0, name collection = ""_n, uint32_t template_id = 0) {

    content_t contents(get_self(), get_self().value); // Access the content table

    // If contentid is provided, remove by contentid
    if (contentid != 0) {
        auto itr = contents.find(contentid);
        check(itr != contents.end(), "Content with this ID does not exist.");

        check((has_auth(itr->submitter) || has_auth(get_self())) , "Only the submitter or contract can remove the content.");

        contents.erase(itr); // Remove the content from the table
    } 
    // If contentid is not set, use the collection name and template_id
    else if (collection != ""_n && template_id != 0) {
        auto by_template = contents.get_index<"byextid"_n>(); 
        auto temp_itr = by_template.find(((uint64_t) contentid));

        bool found = false;
        for (; temp_itr != by_template.end(); ++temp_itr) {
            if (temp_itr->domain == collection) {
                by_template.erase(temp_itr);
                found = true;
                break; 
            }
        }

        check(found, "Nothing found for the specified collection and template ID.");
    } else {
        check(false, "Either contentid or both collection and template_id must be provided.");
    }
}//END remvcontent()


// WARN must add checks to payment actions to avoid paying ""_n
//CHECK we removed upsender from index, ensure logic remains sound
ACTION ups::removeupper(name upsender) {
    check(has_auth(upsender) || has_auth(get_self()), "Only "+upsender.to_string()+" can reset their account"); // Ensure only the contract can call this action 

    // --- Get the IOUs --- //
    ious_t _ious(get_self(), upsender.value);
    auto upcatcher_itr = _ious.begin();
    int count = 0;

    while (upcatcher_itr != _ious.end() && count < 99) {
        _ious.erase(upcatcher_itr);
        upcatcher_itr++;
        count++;
    }

    // --- Get content where upsender is the submitter --- //
    content_t _contents(get_self(), get_self().value);
    auto submitter_idx = _contents.get_index<"bysubmitter"_n>();
    auto submitter_itr = submitter_idx.find(upsender.value);

    while (submitter_itr != submitter_idx.end() && submitter_itr->submitter == upsender ) {
        submitter_idx.modify(submitter_itr, get_self(), [&](auto& row) {
            row.submitter = ""_n; // Set the submitter to an empty name
        });
        submitter_itr++;
    }

    // Check if there are no more records in the ious table related to the upsender
    if ( upcatcher_itr == _ious.end()) {
        // Access the uppers table and remove the upsender
        uppers_t uppers(get_self(), get_self().value);
        auto upper_itr = uppers.find(upsender.value);
        if (upper_itr != uppers.end()) {
            uppers.erase(upper_itr);
        } else { // --- Add the user to Purgatory so oracle can remove them TODO add readme explacountry about why this is needed

            internallog_t internal_log(get_self(), get_self().value);
            check(internal_log.exists(), "Contract has not been set up");

            // Fetch the existing internal log record
            auto log = internal_log.get();

            // Check if the upsender is already in purgatory
            if (find(log.purgatory.begin(), log.purgatory.end(), upsender) == log.purgatory.end()) {
                // Add the upsender to purgatory
                log.purgatory.push_back(upsender);

                // Save the updated internal log record
                internal_log.set(log, get_self());
            }
        }
}
}//END removeupper()

ACTION ups::pauserewards(bool pause) {
    // --- Action must be authorized by the contract itself --- //
    check(has_auth(get_self()), "Only contract owner can pause the rewards."); 

    // --- Access the config singleton --- //
    config_t _config(get_self(), get_self().value); 

    // --- Check if the config exists --- //
    check(_config.exists(), "Call setconfig() then come back.");

    // --- Get the existing config --- //
    ups::config conf = _config.get();

    // --- Update the paused_rewards status --- //
    conf.paused_rewards = pause;

    // --- Set the updated config back to the singleton --- //
    _config.set(conf, get_self()); 

}

ACTION ups::pauseups(bool pause) {
    // --- Action must be authorized by the contract itself --- //
    check(has_auth(get_self()), "Only contract owner can pause the Ups."); 

    // --- Access the config singleton --- //
    config_t _config(get_self(), get_self().value); 

    // --- Check if the config exists --- //
    check(_config.exists(), "Call setconfig() then come back.");

    // --- Get the existing config --- //
    ups::config conf = _config.get();

    // --- Update the paused_ups status --- //
    conf.paused_ups = pause;

    // --- Set the updated config back to the singleton --- //
    _config.set(conf, get_self()); 
}

// --- Action to set configuration --- //
ACTION ups::setconfig(name up_token_contract, symbol up_token_symbol, name reward_token_contract, symbol reward_token_symbol, asset one_up_amount, asset one_reward_amount, bool pay_submitter, bool pay_upsender, uint32_t reward_multiplier_percent, uint32_t timeunit) {
    // --- Ensure the action is authorized by the contract itself --- //
    check(has_auth(get_self()), "Only contract owner can set the config"); 

    // --- Access the config singleton --- //
    config_t conf_tbl(get_self(), get_self().value);
    bool existencial = conf_tbl.exists();

    // --- Token Checks --- //
    check(reward_token_symbol.is_valid() && up_token_symbol.is_valid(), "Invalid token symbol");
    check(one_up_amount.amount > 0 && one_reward_amount.amount > 0, "Quantity of reward and up must be positive, you can pause rewards for 0 by calling pauserewards");
    check((is_account(up_token_contract) && is_account(reward_token_contract)), "Contract account(s) doesn't exist");

    if (existencial){
        config old_conf = conf_tbl.get();
        if (old_conf.timeunit != timeunit){
            // --- Can't change time unit after ups have been made as it's used for reward calculations --- //
            ups::upslog_t _ups(get_self(), get_self().value);
            bool dundidit = (_ups.begin() != _ups.end());
            check (!dundidit, "You cannot adjust the timeunit after Ups have been made. Whoops.");
        }
    }

    // --- Create new config object --- //
    config new_conf;

    //OPTIONAL we could set default values, but small benefit to only 1 person

    // --- Set new configuration values --- //
    new_conf.up_token_contract = up_token_contract;
    new_conf.up_token_symbol = up_token_symbol;
    new_conf.reward_token_contract = reward_token_contract;
    new_conf.reward_token_symbol = reward_token_symbol;
    new_conf.one_up_amount = one_up_amount; // --- One up and one reward are linked to one unit of time per user
    new_conf.one_reward_amount = one_reward_amount; // --- Keep this in mind if planning rewards + inflatioon
    new_conf.reward_multiplier_percent = 100; // --- Increase or decrease rewards per time unit without destabalizing relationship. WARN may mean claims get paif current rate without calc of change tim 
    new_conf.timeunit = timeunit; // This cannot change once  Ups are made. 
    new_conf.pay_submitter = pay_submitter;
    new_conf.pay_upsender = pay_upsender;
    new_conf.paused_rewards = false; // --- Defaults to not be paused, can't set pause from setconfig
    new_conf.paused_ups = false;

    // --- Set the new config in the singleton --- //
    conf_tbl.set(new_conf, get_self()); 

    // --- Find current time unit --- //
    auto current_time = eosio::current_time_point().sec_since_epoch();


    // --- Set up the internal log --- //
    internallog_t internlog(get_self(), get_self().value);
    internallog internlog_data = internlog.get_or_create(get_self(), internallog{//CHECK does this need to set a variable?
        .lastpay = current_time,
        .lastfullpay = current_time,
        .purgatory = vector<name>(), 
        .purg_content = vector<uint64_t>()
    });
}


// --- Receive Tokenized Ups + Put them in Tables --- //
[[eosio::on_notify("*::transfer")]] void ups::up_catch( const name from, const name to, const asset quantity, const string memo )
{  

    if (from == _self || to != _self) return;
    
    // --- Checks + Set up Variables --- //
    config conf = check_config();
    check(!conf.paused_ups, "Ups are currently paused.");
    check(get_first_receiver() == conf.up_token_contract, "This isn't the correct Up token.");
    uint64_t up_quantity; 
    check(up_quantity = static_cast<uint32_t>(quantity.amount / conf.one_up_amount.amount), "Please send exact amount, a multiple of "+ conf.one_up_amount.to_string());
    check(up_quantity >= 1, "Your Up was too small. Send at least "+  conf.one_up_amount.to_string());

    //TODO add refund of any token that isn't divisible

    name content_name; // --- To parse URL if needed 

    string sanitized_memo;
    for (char c : memo) {
        if (c != ' ') {
            sanitized_memo += c;
        }
    }


    // --- Check for '|' in memo --- //
    size_t delimiter_pos = sanitized_memo.find('|');
    if (delimiter_pos != string::npos) {
        // --- Split memo into function name and parameter --- //
        string memo_man = sanitized_memo.substr(0, delimiter_pos); // first part of URL
        string parameter = sanitized_memo.substr(delimiter_pos + 1); // second part of URL
        name domain;
 
        // --- Call the function based on memo_man --- //
        if (memo_man == "up") {
            // --- Send up using contentID --- //
            uint64_t contentid;
            check(contentid = std::stoull(parameter), "Content ID is a number, send the memo as: up|<contentid> ");
            upsertup(up_quantity, from, contentid, 0);
            return;
        } else if (memo_man == "url" || memo_man == "upurl" || memo_man == "link") { 

            upsertup_url(up_quantity, from, parameter);
            return;
        } else if (memo_man == "nft" ||memo_man == "upnft" ) {
            int32_t templateid;
            delimiter_pos = parameter.find('|');
            check(delimiter_pos != string::npos, "Please send the memo as: nft|collection|templateid ");
            name collection = name(parameter.substr(0, delimiter_pos)); // first part 

        
            check(templateid = std::stol(parameter.substr(delimiter_pos + 1)), "Template ID isn't a number. Please send the memo as: nft|<collection>|<templateid>") ; // second part of URL
            upsertup_nft(up_quantity, from, collection, templateid);

            return;
        } else if (memo_man == "addurl" ||memo_man == "addlink" ) {
            ups::addcontent(from, 0.0, 0.0, 0, 0, "", "", 0, 0, parameter, ""_n, ""_n, 0);
            return;
        } else if (memo_man == "addnft") { // format nft|collection|tokenid  
            int32_t templateid;
            delimiter_pos = parameter.find('|');
            check(delimiter_pos != string::npos, "Please send the memo as: addnft|<collection>|<templateid> ");
            // --- Split memo into collection name and template id --- //
            name collection = name(parameter.substr(0, delimiter_pos)); // first part
            
            check(templateid = std::stol(parameter.substr(delimiter_pos + 1)), "Template ID isn't a number. Please send the memo as: nft|<collection>|<templateid>") ; // second part of URL
            ups::addcontent(from, 0.0, 0.0, 0, 0, "", "", 0, 0, "", ""_n, collection, templateid);
            return;
        } /*/else if (memo.size() <= 12) {
            domain = parse_url(parameter);
            // --- Check if content is registered in _content --- //

        }/*/ else {
            // Handle unknown memo
            check (0, "Unknown memo, send contentid with up| or url| register/upvote or reg| to register");
        }
    } else {
        /// --- --- // 
        check(0, "⚡️ Unknown memo, Up with: up|<contentid>, upurl|<link>, upnft|<collection>|<templateid>, or register content with addurl|<url>, addnft|<collection>|<templateid>");
    }
  
} // END token transfer listener


