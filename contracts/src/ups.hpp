#include <eosio/eosio.hpp>
#include <eosio/asset.hpp>
#include <eosio/crypto.hpp>
#include <atomicassets-interface.hpp>
#include <ups-web4.hpp>


/*/ --- Why all these Indexes? ---//

You'll find that this contract has far too many indexes for most use cases.

If you don't plan to use them, consider deleting the indexes that are not used by the contract. 

/*/

using namespace std;
using namespace eosio;

class [[eosio::contract]] ups : public contract {
  
using contract::contract;

public:

struct content_provider {
  name domain;
  string raw_domain;
  uint32_t country; // Integer codes from ISO 3166 

  uint64_t primary_key() const { return static_cast<uint64_t>(domain.value); }
};

typedef singleton<name("contentprov"), content_provider> content_provider_singleton;

/*/ --- Note on Locations ----

  The user can input a text based representation of location, while tables always store an integer-based location.

  Storing integers improves performance and allows indexing, but does decrease the human-readability of the tables. 
  

  --- Data Sanity Checks ---

  This contract does provide checks with the nation as well as the continents subregion fields. 
  An invalid continent subregion will result in storing a 1 indicating a worldwide region. 
  This value of one indicate in the world is not standard, but used as a default within the contract so that all content is scoped to some region.

  There is no check that a provided country matches the continent subregion, they are completely separate. 

  There is no checks for subdivision or postal_code. This would be too costly. You must implement this on the frontend. 

  --- Latitiude and Longitude ---

  To avoid working with floats, all latitude and longitude values are rounded to a four digit precision and stored in tables as an integer by multiplying the value by 10,000.

/*/

TABLE content {
  uint64_t contentid;
  name domain;
  name submitter;
  string link;
  uint32_t external_id;
  checksum256 gudahash;
  uint32_t created;
  int32_t latitude; // stored as the decimal value to .0000 * 10000
  int32_t longitude;
  uint32_t subcontinent; // codes from M49
  uint32_t country; // codes from ISO 3166 
  uint32_t subdivision;
  uint32_t postal_code;

  uint64_t primary_key() const { return contentid; } 
  uint64_t by_domain() const { return domain.value; } 
  uint64_t by_external_id() const { return static_cast<uint64_t>(external_id); }
  uint64_t by_submitter() const { return submitter.value; }
  checksum256 by_gudahash() const { return gudahash; }
  uint64_t by_subcontinent() const { return static_cast<uint64_t>(subcontinent); }
  uint64_t by_country() const { return static_cast<uint64_t>(country); }
  uint64_t by_subdivision() const { return static_cast<uint64_t>(subdivision); }
  uint64_t by_postal_code() const { return static_cast<uint64_t>(postal_code); }
  uint64_t by_lat_lng() const { return (static_cast<uint64_t>(latitude) << 32) | longitude; }
 
};

using content_t = multi_index<"content"_n, content,
  indexed_by<"bydomain"_n, const_mem_fun<content, uint64_t, &content::by_domain>>,
  indexed_by<"bysubmitter"_n, const_mem_fun<content, uint64_t, &content::by_submitter>>,
  indexed_by<"byextid"_n, const_mem_fun<content, uint64_t, &content::by_external_id>>,
  indexed_by<"bygudahash"_n, const_mem_fun<content, checksum256, &content::by_gudahash>>,
  indexed_by<"bysubconteni"_n, const_mem_fun<content, uint64_t, &content::by_subcontinent>>,
  indexed_by<"bycountry"_n, const_mem_fun<content, uint64_t, &content::by_country>>,
  indexed_by<"bysubdiv"_n, const_mem_fun<content, uint64_t, &content::by_subdivision>>,
  indexed_by<"bypostalcode"_n, const_mem_fun<content, uint64_t, &content::by_postal_code>>,
  indexed_by<"bylatlng"_n, const_mem_fun<content, uint64_t, &content::by_lat_lng>>
>;



  TABLE ups_log { 
    uint64_t upid; 
    uint64_t contentid;
    name upsender;
    uint32_t totalups; 
    uint32_t tuid;
  
    uint64_t primary_key() const { return upid; }
    uint64_t by_contentid() const { return contentid; }
    uint64_t by_upsender() const { return upsender.value; } 
    uint64_t by_ups() const { return static_cast<uint64_t>(totalups); }
    uint64_t by_tuid() const { return static_cast<uint64_t>(tuid); }
  };
  
  using upslog_t = multi_index<name("upslog"), ups_log,
    eosio::indexed_by<"bycontentid"_n, eosio::const_mem_fun<ups_log, uint64_t, &ups_log::by_contentid>>,
    eosio::indexed_by<"byupsender"_n, eosio::const_mem_fun<ups_log, uint64_t, &ups_log::by_upsender>>,
    eosio::indexed_by<"byups"_n, eosio::const_mem_fun<ups_log, uint64_t, &ups_log::by_ups>>,
    eosio::indexed_by<"bytuid"_n, eosio::const_mem_fun<ups_log, uint64_t, &ups_log::by_tuid>>
  >;
  
  TABLE totals {
    uint64_t contentid;
    uint32_t totalups; 
    uint32_t updated;
    
    uint64_t primary_key() const { return contentid; }
  };
  
  using totals_t = multi_index<name("totals"), totals>;
  
  
  // --- Activity stats for uppers (accounts sending upvotes) --- //

  TABLE uppers {
    name upsender;
    uint32_t firstup;
    uint32_t lastup;
    uint32_t totalups;
    uint32_t claimable; //NOTE this value is not used in calculation of rewards, just for visibility.
    uint64_t primary_key() const { return upsender.value; }
  };
  
  using uppers_t = multi_index<name("uppers"), uppers>;


  // --- Store record of who to pay --- // 
  TABLE ious {
    uint64_t contentid;
    uint32_t upscount;
    uint32_t initiated;
    uint32_t updated; 

    uint64_t by_upscount() const { return static_cast<uint64_t>(upscount); }
    uint64_t by_initiated() const { return static_cast<uint64_t>(initiated); }
    uint64_t by_updated() const { return static_cast<uint64_t>(updated); }
    
    uint64_t primary_key() const { return contentid; }
  };

  using ious_t = multi_index<name("ious"), ious,
    eosio::indexed_by<"byupscount"_n, eosio::const_mem_fun<ious, uint64_t, &ious::by_upscount>>,
    eosio::indexed_by<"byinitiated"_n, eosio::const_mem_fun<ious, uint64_t, &ious::by_initiated>>,
    eosio::indexed_by<"byupdated"_n, eosio::const_mem_fun<ious, uint64_t, &ious::by_updated>>
  >;

  // --- Contract Macro statistics + needed cleanup --- // 
  TABLE internallog { 
    uint32_t lastpay; // Last time the payment was called for all 
    uint32_t lastfullpay; 
    vector<name> purgatory; // Accounts in limbo due to partial deletions, repeat calls to removeupper() to finish 
    vector<uint64_t> purg_content;

    uint64_t primary_key() const { return static_cast<uint64_t>(lastpay); } 
  };
  
  typedef singleton<name("internallog"), internallog> internallog_t;


  TABLE config {
      name up_token_contract;
      symbol up_token_symbol;
      name reward_token_contract;
      symbol reward_token_symbol;
      asset one_up_amount;
      asset one_reward_amount;
      uint32_t reward_multiplier_percent;
      uint32_t timeunit;
      bool pay_submitter;
      bool pay_upsender;
      bool paused_rewards;
      bool paused_ups;
  };

  // --- Declare Config Singleton --- //
  typedef singleton<name("config"), config> config_t;
 

  // --- Helper Functions --- //
  void upsertup(uint32_t upscount, name upsender, uint64_t contentid, bool negative);
  void logup(uint32_t upscount, name upsender, uint64_t contentid);
  void remvcontent(uint64_t contentid);
  void addcontent(name submitter, double latitude, double longitude, uint32_t continent_subregion_code, uint32_t country_code, const string& continent_subregion_name, const string& country_iso3, uint32_t subdivision , uint32_t postal_code, const string url, name domain, name collection, int32_t templateid);
  void upsertup_url(uint32_t upscount, name upsender, string url);
  
  void upsertup_nft(uint32_t upscount, name upsender, name collection, int32_t templateid);
  void upsert_logup(uint32_t upscount, name upsender, uint64_t contentid, bool negative);
  void upsert_total(uint32_t upscount, name upsender, uint64_t contentid, bool negative);
  void upsert_ious(uint32_t upscount, name upsender, uint64_t contentid, bool subtract);
  void pay_iou(uint32_t maxpayments, name receiver, bool paythem);

  // --- Functions that help the helper functions --- //
  uint32_t find_tu(uint32_t momentuin, uint32_t tu_length);
  uint32_t find_tu(uint32_t tu_length);
  string chopped_url(const string url);
  checksum256 url_hash(const string url);
  name url_domain_name(const string url);
  config check_config();
  bool isAuthorized(name collection, name user);
  string normalize_enum_name(const string& input);
  uint32_t is_valid_continent_subregion(uint32_t code, const string& name);
  uint32_t is_valid_country(uint32_t code, const string country_iso3);
  vector<int32_t> validate_and_format_coords(const vector<double>& coords);


  /*/ --- Naming conventions for table use --- // 
  ious_t _ious;
  upslog_t _upslog;
  uppers_t _uppers;
  totals_t _totals;
  content_t _content;
  /*/

  
  [[eosio::on_notify("*::transfer")]] // CHECK REQUIRES correct contract for SOL/BLUX Listens for any token transfer
  void up_catch( const name from, const name to, const asset quantity, const string memo );
  
  ACTION payup(name upsender); // User's call to pay themselves, works for contract also

  ACTION removeupper(name upsender);

  ACTION remvcontent(uint64_t contentid, name collection, uint32_t template_id);
  
  ACTION updatecont(name& submitter, uint64_t contentid, double latitude, double longitude, uint32_t continent_subregion_code, uint32_t country_code , const string& continent_subregion_name, const string& country_iso3, uint32_t subdivision, uint32_t postal_code);

  ACTION regdomain(const name& submitter, const string& url, const string& country_iso3);

  ACTION setconfig(name up_token_contract, symbol up_token_symbol, name reward_token_contract, symbol reward_token_symbol, asset one_up_amount, asset one_reward_amount, bool pay_submitter, bool pay_upsender, uint32_t reward_multiplier_percent, uint32_t timeunit);

  ACTION regnftcol(const name& submitter, const name& nft_collection, string& country_iso3);

  ACTION addurl( name submitter, const string& url, const name& domain, double latitude, double longitude, uint32_t continent_subregion_code, uint32_t country_code, const string& continent_subregion_name, const string& country_iso3, uint32_t subdivision, uint32_t postal_code);
    
  ACTION addnft(name& submitter, double latitude, double longitude, uint32_t continent_subregion_code, uint32_t country_code, const string& continent_subregion_name, const string& country_iso3, uint32_t subdivision, uint32_t postal_code, const name& collection, const uint32_t& templateid);
  
  ACTION pauserewards(bool pause);

  ACTION pauseups(bool pause);

};//END contract ups