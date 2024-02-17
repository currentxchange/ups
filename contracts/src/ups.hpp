#include <eosio/eosio.hpp>
#include <eosio/asset.hpp>
#include <atomicassets-interface.hpp>
#include <atomicassets-interface.hpp>

/*/
#include <checkformat.hpp>
#include <atomicdata.hpp>
/*/

using namespace eosio;
using namespace std;
using namespace atomicdata;

class [[eosio::contract]] ups : public contract {
  
using contract::contract;
public: 

struct content_provider {
  name domain;
  string raw_domain;
  vector<uint32_t> tetra_loc; // --- Requires web4 headers {Continent_Subregions M49, Nation Alpha 3, Subdivision e.g. state, Postal code}

  uint64_t primary_key() const { return domain.value; }
};

typedef singleton<name("content_provider"), content_provider> content_provider_singleton;

/*/ --- SCOPED to name domain --- //
Tetra_loc contains numeric codes for the Continent Subregions (M49), Country (ISO 3166 alpha-3), state (Standardized by nation, .hpp in development), and postal code (if applicable, also in development)
| Option | Double insertion into anothr scope allow for global cu
--- Curation of content by location ----
Find the top content in each area by indexes provided.
- Init table scoped to name domain e.g. youtube
- Use index for a location by_tetraloc2 for nation e.g. COL
- Returns all the content for Colombia
- Then read the totals table for the # of ups
/*/

TABLE content_table { // CHECK final decision to use bitshift or auto increment
  uint64_t id;
  name domain;
  name submitter;
  string link;
  uint32_t external_id;
  checksum256 gudahash;
  time_point_sec created;
  vector<float> latlng({0.0,0.0});//CHECK I don't think this is how you set default values 
  vector<uint32_t> tetra_loc({0,0});

  uint64_t primary_key() const { return id; } //CHECK use this to return the bitshift
  uint64_t by_domain() const { return domain.value; } //CHECK if needed with scoping
  uint64_t by_external_id() const { return domain.value; }
  checksum256 by_gudahash() const { return (uint64_t) external_id; }
  uint64_t by_latitude() const { return static_cast<uint64_t>(latlng[0] * 10000); } 
  uint64_t by_longitude() const { return static_cast<uint64_t>(latlng[1] * 10000); }
  uint64_t by_tetraloc1() const { return (uint64_t) (tetra_loc[0]); }
  uint64_t by_tetraloc2() const { return (uint64_t) (tetra_loc[1]); }

};

using content_table_index = multi_index<name("content"), content_table,
  indexed_by<"bydomain"_n, const_mem_fun<content_table, uint64_t, &content_table::by_domain>>,
  indexed_by<"byextid"_n, const_mem_fun<content_table, uint64_t, &content_table::by_external_id>>,
  indexed_by<"bygudahash"_n, const_mem_fun<content_table, checksum256, &content_table::by_gudahash>>,
  indexed_by<"bylatitude"_n, const_mem_fun<content_table, uint64_t, &content_table::by_latitude>>,
  indexed_by<"bylongitude"_n, const_mem_fun<content_table, uint64_t, &content_table::by_longitude>>,
  indexed_by<"bytetra1"_n, const_mem_fun<content_table, uint64_t, &content_table::by_tetraloc1>>,
  indexed_by<"bytetra2"_n, const_mem_fun<content_table, uint64_t, &content_table::by_tetraloc2>>,

>;

  TABLE ups { 
    uint64_t upid; 
    uint64_t content_id;
    uint32_t totalups; 
    uint32_t tuid;
  
    uint64_t primary_key() const { return upid; }
    uint64_t by_content_id() const { return content_id; }
    uint64_t by_ups() const { return (uint64_t) totalups; }
    uint64_t by_tuid() const { return (uint64_t) tuid; }
  };
  
  using ups_table = multi_index<name("ups"), ups,
    eosio::indexed_by<"bycontentid"_n, eosio::const_mem_fun<ups, uint64_t, &ups::by_content_id>>,
    eosio::indexed_by<"byups"_n, eosio::const_mem_fun<ups, uint64_t, &ups::by_ups>>,
    eosio::indexed_by<"bytuid"_n, eosio::const_mem_fun<ups, uint64_t, &ups::by_tuid>>
  >;
  
  TABLE totals {
    uint64_t content_id;
    uint32_t totalups; 
    time_point_sec updated;
    
    uint64_t primary_key() const { return content_id; }
  };
  
  using totals_table = multi_index<name("totals"), totals>;
  
  
  // --- Activity stats for uppers (For future awards) --- //
  TABLE uppers {
    name upsender;
    time_point_sec firstup;
    time_point_sec lastup;
    uint32_t totalups;
    uint32_t claimable; //TODO ensure this is reflected in the totals 
    uint64_t primary_key() const { return upsender.value; }
  };
  
  using uppers_table = multi_index<name("uppers"), uppers>;



  TABLE schemas_s {
      name schema_name;
      vector <FORMAT> format;

      uint64_t primary_key() const { return schema_name.value; }
  };

  
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
    uint64_t by_tuid() const { return (uint64_t) tuid; }
    uint64_t by_upscount() const { return (uint64_t) upscount.sec_since_epoch(); }
    uint64_t by_initiated() const { return (uint64_t) initiated.sec_since_epoch(); }
    uint64_t by_updated() const { return (uint64_t) updated; }
  };

  using ious_table = multi_index<name("ious"), ious,
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

    uint64_t primary_key() const { return (uint64_t) lastpay; } //WARN CHECK if this is singleton (it isn't, fix it)
  };
  
  using internallog_table = multi_index<name("internallog"), internallog>;
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
  > locations_table;

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
  typedef singleton<name("config"), config> config_table;

  private:  

  void upsertup(uint32_t upscount, name upsender, uint64_t content_id); //DISPATCHER
  void logup(uint32_t upscount name upsender, uint64_t content_id); 
  void removeiou(name sender, name receiver); // Receiver or sender can be set to dummy value to delete all for a user
  void updateupper(uint32_t upscount name upsender);
  void removecontent(uint64_t content_id); // Removes all IOUs for nft + nft record (minimal)
  void deepremvcont(uint64_t content_id); // Removes all records of Ups for this content  
  void addcontent(name& submitter, string& url, name domain = false, name collection = false, uint32_t templateid = false);
  
  // --- Declare the _tables for later use --- // 
  ious_table _ious;
  ups_table _ups;
  uppers_table _uppers;
  totals_table _totals;
  internallog_table _internallog;
  content_table _content;



  
public:
  
  [[eosio::on_notify("*::transfer")]] // CHECK REQUIRES correct contract for SOL/BLUX Listens for any token transfer
  void up_catch( const name from, const name to, const asset quantity, const std::string memo );
  
  ACTION payup(name upsender = false); // User's call to pay themselves

  ACTION removeupper(name upsender);
  
  ACTION updatecont(uint64_t content_id);

  ACTION regdomain(const name& submitter, const string& url);

  //ACTION configdomain(const name& submitter, const string& url, const name& up_token_contract, const symbol& up_token_symbol, const name& reward_token_contract, const symbol& reward_token_symbol, const asset& one_up_amount, const asset& one_reward_amount);

  ACTION regnftcol(const name& submitter, const name& nft_collection);

  ACTION addurl(const name domain, const name& submitter, const string& url); //CHECK if need the domain name

  ACTION pauserewards(bool pause);

  ACTION pauseups(bool pause);
