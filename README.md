# .gems `Redeem` smart contract

[![GitHub license](https://img.shields.io/badge/license-MIT-blue.svg)](https://github.com/dotGems/gems.redeem/blob/main/LICENSE)
[![Antelope CDT](https://github.com/dotGems/gems.redeem/actions/workflows/release.yml/badge.svg)](https://github.com/dotGems/gems.redeem/actions/workflows/release.yml)
[![Blanc++ Vert](https://github.com/dotGems/gems.redeem/actions/workflows/tests.yml/badge.svg)](https://github.com/dotGems/gems.redeem/actions/workflows/tests.yml)

## Quickstart

```bash
# Set redeem
$ cleos push action redeem.gems setredeem '[123, "1.0000 EOS", false]' -p redeem.gems
```

## Build

```bash
# using Antelope CDT
$ cdt-cpp eosio.time.cpp
# using Blanc++
$ blanc++ eosio.time.cpp
```

## TABLE `redeems`

### params

- `{uint32_t} template_id` - (primary key) AtomicAsset NFT template ID
- `{extended_asset} quantity` - redeemable quantity
- `{bool} pomelo_grant` - send redeem to active Pomelo Grant

### example

```json
{
    "template_id": 123,
    "quantity": {"contract": "eosio.token", "quantity": "1.0000 EOS"},
    "pomelo_grant": true
}
```