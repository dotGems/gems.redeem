#include <eosio/eosio.hpp>
#include <eosio/system.hpp>
#include <eosio/asset.hpp>

// AtomicAssets
#include <eosio.token/eosio.token.hpp>
#include <gems/atomic.gems.hpp>

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
     * - `{bool} pomelo_grant` - send redeem to active Pomelo Grant
     *
     * ### example
     *
     * ```json
     * {
     *     "template_id": 123,
     *     "quantity": {"contract": "eosio.token", "quantity": "1.0000 EOS"},
     *     "pomelo_grant": true
     * }
     * ```
     */
    struct [[eosio::table("redeems")]] redeems_row {
        uint32_t                template_id;
        extended_asset          quantity;
        bool                    pomelo_grant;

        uint64_t primary_key() const { return template_id; }
    };
    typedef eosio::multi_index< "redeems"_n, redeems_row> redeems_table;

    [[eosio::action]]
    void setredeem( const uint32_t template_id, const asset quantity, const bool pomelo_grant );

    /**
     * Notify contract when AtomicAssets NFT token transfer notifiers relay contract
     */
    [[eosio::on_notify("atomicassets::transfer")]]
    void on_nft_transfer( const name from, const name to, const vector<uint64_t> asset_ids, const std::string memo );

private:
    void transfer( const name from, const name to, const extended_asset value, const string memo );
};