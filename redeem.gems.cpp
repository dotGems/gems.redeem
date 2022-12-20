#include "./redeem.gems.hpp"

[[eosio::action]]
void redeem::delredeem( const uint32_t template_id )
{
    require_auth( get_self() );

    redeem::redeems_table _redeems( get_self(), get_self().value );
    auto & itr = _redeems.get( template_id, "template_id does not exists" );
    _redeems.erase( itr );
}

[[eosio::action]]
void redeem::setredeem( const uint32_t template_id, const asset quantity, const bool redirect_to_pomelo_grant )
{
    require_auth( get_self() );

    check( quantity.is_valid(), "invalid quantity" );
    check( quantity.amount > 0, "must redeem positive quantity" );
    check( quantity.symbol == symbol("EOS", 4), "invalid quantity symbol" );

    redeem::redeems_table _redeems( get_self(), get_self().value );
    auto itr = _redeems.find( template_id );

    // recipe content
    auto insert = [&]( auto & row ) {
        row.template_id = template_id;
        row.quantity = extended_asset{quantity, "eosio.token"_n};
        row.redirect_to_pomelo_grant = redirect_to_pomelo_grant;
    };

    if ( itr == _redeems.end() ) _redeems.emplace( get_self(), insert );
    else  _redeems.modify( itr, get_self(), insert );
}

[[eosio::on_notify("atomicassets::transfer")]]
void redeem::on_nft_transfer( const name from, const name to, const vector<uint64_t> asset_ids, const std::string memo )
{
    redeem::redeems_table _redeems( get_self(), get_self().value );

    for ( const uint64_t asset_id : asset_ids ) {
        auto asset = atomic::get_asset( get_self(), asset_id );
        auto redeem = _redeems.get( asset.template_id, "cannot redeem template_id" );

        // burn and transfer
        atomic::burnasset( get_self(), asset_id );
        transfer( get_self(), from, redeem.quantity, "redeemed from NFT");
    }
}

void redeem::transfer( const name from, const name to, const extended_asset value, const string memo )
{
    eosio::token::transfer_action transfer( value.contract, { from, "active"_n });
    transfer.send( from, to, value.quantity, memo );
}

