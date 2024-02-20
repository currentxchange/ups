#include <eosio/eosio.hpp>
#include <eosio/asset.hpp>
#include <atomicassets-interface.hpp>
#include <ups-web4.hpp>

/*/
#include <checkformat.hpp>
#include <atomicdata.hpp>

eosio-cpp -I eosio -I ../include -abigen -o ups.wasm ups.cpp --no-missing-ricardian-clause
/*/

using namespace std;
using namespace eosio;

class [[eosio::contract]] ups : public contract {
  
using contract::contract;
public: 

struct content_provider {
  name domain;
  string raw_domain;
  uint32_t country; // codes from ISO 3166 alpha-3

  uint64_t primary_key() const { return domain.value; }
};

typedef singleton<name("contentprov"), content_provider> content_provider_singleton;

/*/ --- SCOPED to name domain --- //
Tetra_loc contains numeric codes for the Continent Subregions (M49), Country (ISO 3166 alpha-3), state (Standardized by country, .hpp in development), and postal code (if applicable, also in development)
| Option | Double insertion into anothr scope allow for global cu
--- Curation of content by location ----
Find the top content in each area by indexes provided.
- Init table scoped to name domain e.g. youtube
- Use index for a location, like country
- Then read the totals table for the # of ups
/*/

TABLE content {
  uint64_t id;
  name domain;
  name submitter;
  string link;
  uint32_t external_id;
  checksum256 gudahash;
  time_point_sec created;
  int32_t latitude; // stored as the decimal value to .0000 * 10000
  int32_t longitude;
  uint32_t subcontinent; // codes from M49
  uint32_t country; // codes from ISO 3166 alpha-3
  uint32_t subdivision;
  uint32_t postal_code;

  uint64_t primary_key() const { return id; } //CHECK use this to return the bitshift
  uint64_t by_domain() const { return domain.value; } //CHECK if needed with scoping
  uint64_t by_external_id() const { return static_cast<uint64_t>(external_id); }
  checksum256 by_gudahash() const { return gudahash; }
  uint64_t by_subcontinent() const { return static_cast<uint64_t>(subcontinent); }
  uint64_t by_country() const { return static_cast<uint64_t>(country); }
  uint64_t by_subdivision() const { return static_cast<uint64_t>(subdivision); }
  uint64_t by_postal_code() const { return static_cast<uint64_t>(postal_code); }
  uint64_t by_lat_lng() const { return (uint64_t{latitude} << 32) | longitude; }
 
};

using content_t = multi_index<"content"_n, content,
  indexed_by<"bydomain"_n, const_mem_fun<content, uint64_t, &content::by_domain>>,
  indexed_by<"byextid"_n, const_mem_fun<content, uint64_t, &content::by_external_id>>,
  indexed_by<"bygudahash"_n, const_mem_fun<content, checksum256, &content::by_gudahash>>,
  indexed_by<"bysubconteni"_n, const_mem_fun<content, uint64_t, &content::by_subcontinent>>,
  indexed_by<"bycountry"_n, const_mem_fun<content, uint64_t, &content::by_country>>,
  indexed_by<"bysubdiv"_n, const_mem_fun<content, uint64_t, &content::by_subdivision>>,
  indexed_by<"bypostalcode"_n, const_mem_fun<content, uint64_t, &content::by_postal_code>>,
  indexed_by<"bylatlng"_n, const_mem_fun<content, uint64_t, &content::by_lat_lng>>
>;

TABLE content_domain {
    uint64_t content_id;
    name domain;
    uint64_t primary_key() const { return content_id; }
};
typedef singleton<name("contdomain"), content_domain> content_domain_t;

  TABLE ups_log { 
    uint64_t upid; 
    uint64_t content_id;
    uint32_t totalups; 
    uint32_t tuid;
  
    uint64_t primary_key() const { return upid; }
    uint64_t by_content_id() const { return content_id; }
    uint64_t by_ups() const { return static_cast<uint64_t>(totalups); }
    uint64_t by_tuid() const { return static_cast<uint64_t>(tuid); }
  };
  
  using upslog_t = multi_index<name("upslog"), ups_log,
    eosio::indexed_by<"bycontentid"_n, eosio::const_mem_fun<ups_log, uint64_t, &ups_log::by_content_id>>,
    eosio::indexed_by<"byups"_n, eosio::const_mem_fun<ups_log, uint64_t, &ups_log::by_ups>>,
    eosio::indexed_by<"bytuid"_n, eosio::const_mem_fun<ups_log, uint64_t, &ups_log::by_tuid>>
  >;
  
  TABLE totals {
    uint64_t content_id;
    uint32_t totalups; 
    time_point_sec updated;
    
    uint64_t primary_key() const { return content_id; }
  };
  
  using totals_t = multi_index<name("totals"), totals>;
  
  
  // --- Activity stats for uppers (For future awards) --- //
  TABLE uppers {
    name upsender;
    time_point_sec firstup;
    time_point_sec lastup;
    uint32_t totalups;
    uint32_t claimable; //TODO ensure this is reflected in the totals 
    uint64_t primary_key() const { return upsender.value; }
  };
  
  using uppers_t = multi_index<name("uppers"), uppers>;


  // --- Store record of who to pay --- // 
  // CHECK (in .cpp) that we are paying both the upsender + upcatcher
  // CHECK that we are using the indexes to get the upsender, etc
  TABLE ious {
    uint64_t iouid;
    uint64_t content_id;
    uint32_t tuid;
    name upsender;
    name upcatcher;
    uint32_t upscount;
    time_point_sec initiated;
    time_point_sec updated; 
    uint64_t primary_key() const { return iouid; }
    uint64_t by_upcatcher() const { return upcatcher.value; }
    uint64_t by_upsender() const { return upsender.value; }
    uint64_t by_content_id() const { return content_id; }
    uint64_t by_tuid() const { return static_cast<uint64_t>(tuid); }
    uint64_t by_upscount() const { return static_cast<uint64_t>(upscount); }
    uint64_t by_initiated() const { return static_cast<uint64_t>(initiated.sec_since_epoch()); }
    uint64_t by_updated() const { return static_cast<uint64_t>(updated.sec_since_epoch()); }
  };

  using ious_t = multi_index<name("ious"), ious,
    eosio::indexed_by<"byupcatcher"_n, eosio::const_mem_fun<ious, uint64_t, &ious::by_upcatcher>>,
    eosio::indexed_by<"byupsender"_n, eosio::const_mem_fun<ious, uint64_t, &ious::by_upsender>>,
    eosio::indexed_by<"byupscount"_n, eosio::const_mem_fun<ious, uint64_t, &ious::by_upscount>>,
    eosio::indexed_by<"bycontentid"_n, eosio::const_mem_fun<ious, uint64_t, &ious::by_content_id>>,
    eosio::indexed_by<"bytuid"_n, eosio::const_mem_fun<ious, uint64_t, &ious::by_tuid>>,
    eosio::indexed_by<"byinitiated"_n, eosio::const_mem_fun<ious, uint64_t, &ious::by_initiated>>,
    eosio::indexed_by<"byupdated"_n, eosio::const_mem_fun<ious, uint64_t, &ious::by_updated>>
  >;

  
  TABLE internallog { // track Macro statistics for each token 
    time_point_sec lastpay; // Last time the payment was called for all 
    time_point_sec lastfullpay; 
    vector<name> purgatory; // Accounts in limbo due to partial deletions, call removeupper() to finish 
    vector<uint64_t> purg_content;

    uint64_t primary_key() const { return static_cast<uint64_t>(lastpay.sec_since_epoch()); } //WARN CHECK if this is singleton (it isn't, fix it)
  };
  
  typedef singleton<name("internallog"), internallog> internallog_t;
  //using internallog_t = multi_index<name("internallog"), internallog>;
/*/--- Alternate location if other way isn't efficient
  TABLE location {
    uint64_t    id;            // Unique ID for each location
    uint32_t    int_code;      // Integer code for the location
    string      iso_alpha3;    // ISO Alpha-3 code
    string      iso_alpha2;    // ISO Alpha-2 code
    string      location_name; // Name of the location
    uint32_t    level;         // Level of location (e.g., country, state)

    uint64_t primary_key() const { return id; }
    uint64_t by_int_code() const { return int_code; } // Secondary index for int_code
    uint64_t by_level() const { return level; }

    EOSLIB_SERIALIZE(location, (id)(int_code)(iso_alpha3)(iso_alpha2)(location_name)(level))
  };

  typedef eosio::multi_index<"locations"_n, location,
      indexed_by<"byintcode"_n, const_mem_fun<location, uint64_t, &location::by_int_code>>, // Index for int_code
      indexed_by<"bylevel"_n, const_mem_fun<location, uint64_t, &location::by_level>>
  > locations_t;

/*/

  TABLE config {
      name up_token_contract;
      symbol up_token_symbol;
      name reward_token_contract;
      symbol reward_token_symbol;
      asset one_up_amount;
      asset one_reward_amount;
      double reward_multiplier;
      uint32_t timeunit;
      bool paused_rewards;
      bool paused_ups;
  };

  // --- Declare Config Singleton --- //
  typedef singleton<name("config"), config> config_t;
 

  // --- Helper Functions --- //
  void upsertup(uint32_t upscount, name upsender, uint64_t content_id, bool negative);
  void logup(uint32_t upscount, name upsender, uint64_t content_id);
  void updateupper(uint32_t upscount, name upsender);
  void removecontent(uint64_t content_id);
  void addcontent(name submitter, string url, name domain, name collection, uint32_t templateid);
  void upsertup_url(uint32_t upscount, name upsender, string url);
  void upsertup_nft(uint32_t upscount, name upsender, int32_t templateid);
  void upsert_logup(uint32_t upscount, name upsender, uint64_t content_id, bool negative);
  void upsert_total(uint32_t upscount, name upsender, uint64_t content_id, bool negative);
  void upsert_ious(uint32_t upscount, name upsender, uint64_t content_id, bool subtract);
  void pay_iou(uint32_t maxpay, name receiver, bool paythem);

  // --- Functions that help the helper functions --- //
  uint32_t find_tu(uint32_t momentuin, uint32_t tu_length);
  auto parse_url(const string& url, bool hash_whole, bool chopped_whole, bool chopped_domain);
  auto check_config(bool ignore_empty);
  bool isAuthorized(name collection, name user);
  string normalize_enum_name(const std::string& input);
  uint32_t is_valid_continent_subregion(uint32_t code, const std::string& name);
  uint32_t is_valid_country(uint32_t code, const std::string& name);
  vector<int32_t> validate_and_format_coords(const vector<double>& coords);


  // --- Declare the _ts for later use --- // 
  ious_t _ious;
  ups_log _ups;
  uppers_t _uppers;
  totals_t _totals;
  content_t _content;
  

  
  [[eosio::on_notify("*::transfer")]] // CHECK REQUIRES correct contract for SOL/BLUX Listens for any token transfer
  void up_catch( const name from, const name to, const asset quantity, const std::string memo );
  
  ACTION payup(name upsender); // User's call to pay themselves

  ACTION removeupper(name upsender);
  
  ACTION updatecont(uint64_t content_id);

  ACTION regdomain(const name& submitter, const string& url);

  //ACTION configdomain(const name& submitter, const string& url, const name& up_token_contract, const symbol& up_token_symbol, const name& reward_token_contract, const symbol& reward_token_symbol, const asset& one_up_amount, const asset& one_reward_amount);

  ACTION regnftcol(const name& submitter, const name& nft_collection);

  ACTION addurl(const name domain, const name& submitter, const string& url); //CHECK if need the domain name

  ACTION pauserewards(bool pause);

  ACTION pauseups(bool pause);

};//END contract ups