#include <eosio/eosio.hpp>
#include <eosio/system.hpp>
#include <eosio/asset.hpp>

// AtomicAssets
#include <eosio.token/eosio.token.hpp>
#include <gems/atomic.gems.hpp>
#include <utils/utils.hpp>
#include <pomelo/pomelo.hpp>

using namespace eosio;

class [[eosio::contract("redeem.gems")]] redeem : public eosio::contract {
public:
    using contract::contract;

    /**
     * ## TABLE `redeems`
     *
     * ### params
     *
     * - `{uint32_t} template_id` - (primary key) AtomicAsset NFT template ID
     * - `{extended_asset} quantity` - redeemable quantity
     * - `{bool} redirect_to_pomelo_grant` - redirect redeemable amount to an active Pomelo Grant
     *
     * ### example
     *
     * ```json
     * {
     *     "template_id": 123,
     *     "quantity": {"contract": "eosio.token", "quantity": "1.0000 EOS"},
     *     "redirect_to_pomelo_grant": true
     * }
     * ```
     */
    struct [[eosio::table("redeems")]] redeems_row {
        uint32_t                template_id;
        extended_asset          quantity;
        bool                    redirect_to_pomelo_grant;

        uint64_t primary_key() const { return template_id; }
    };
    typedef eosio::multi_index< "redeems"_n, redeems_row> redeems_table;

    [[eosio::action]]
    void setredeem( const uint32_t template_id, const asset quantity, const bool redirect_to_pomelo_grant );

    [[eosio::action]]
    void delredeem( const uint32_t template_id );

    [[eosio::action]]
    void test( const string memo );

    [[eosio::action]]
    void redeemlog( const name account, const uint64_t asset_id, const string nft_name, const asset quantity, const bool redirected_to_grant, const string memo);

    /**
     * Notify contract when AtomicAssets NFT token transfer notifiers relay contract
     */
    [[eosio::on_notify("atomicassets::transfer")]]
    void on_nft_transfer( const name from, const name to, const vector<uint64_t> asset_ids, const std::string memo );

    using redeemlog_action = eosio::action_wrapper<"redeemlog"_n, &redeem::redeemlog>;

private:
    void transfer( const name from, const name to, const extended_asset value, const string memo );
    void handle_pomelo_transfer( const name from, const extended_asset value, const string memo, string nft_name );
    name parse_pomelo_grant_name( string memo );
};