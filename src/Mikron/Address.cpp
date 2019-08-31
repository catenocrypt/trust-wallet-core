// Copyright © 2017-2019 Trust Wallet.
//
// This file is part of Trust. The full Trust copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.

#include "Address.h"
#include <TrezorCrypto/nano.h>

using namespace TW::Mikron;

const std::string kPrefixMik{"mik_"};

bool Address::isValid(const std::string& address) {
    bool valid = nano_validate_address(
        kPrefixMik.c_str(), kPrefixMik.length(),
        address.c_str(), address.length(),
        NULL);
    return valid;
}


Address::Address(const std::string& address) {
    bool valid = nano_validate_address(
        kPrefixMik.c_str(), kPrefixMik.length(),
        address.c_str(), address.length(),
        bytes.data());

    // Ensure address is valid
    if (!valid) {
        throw std::invalid_argument("Invalid address data");
    }
}

Address::Address(const PublicKey& publicKey) {
    if (publicKey.type != TWPublicKeyTypeED25519Blake2b) {
        throw std::invalid_argument("Invalid public key type");
    }

    auto keyBytes = publicKey.bytes;
    std::copy(keyBytes.begin(), keyBytes.begin() + 32, bytes.data());
}

std::string Address::string() const {
    std::array<char, 5 + 60 + 1> out = {0};

    size_t count = nano_get_address(
            bytes.data(),
            kPrefixMik.c_str(), kPrefixMik.length(),
            out.data(), out.size());
    out[count] = 0;
    return std::string(out.data());
}
