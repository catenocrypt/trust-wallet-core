// Copyright © 2017-2019 Trust Wallet.
//
// This file is part of Trust. The full Trust copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.

#include "Mikron/Address.h"
#include "HexCoding.h"

#include <gtest/gtest.h>

using namespace std;
using namespace TW;
using namespace TW::Mikron;

TEST(MikAddress, FromPublicKey) {
    {
        const auto publicKey = PublicKey(parse_hex("a073380e2ea7f47f4b495540be6e6858055ab7ef429fcf66e93365a8f3eba7ef"), TWPublicKeyTypeED25519Blake2b);
        const auto address = Address(publicKey);
        ASSERT_EQ(string("mik_3a5m9194xbznhx7nkoc1qsq8ip17dcuyyinzsxmgkeu7o5syqbzhcb7uur1h"), address.string());
    }

    {
        const auto publicKey = PublicKey(parse_hex("03e20ec6b4a39a629815ae02c0a1393b9225e3b890cae45b59f42fa29be9668d"), TWPublicKeyTypeED25519);
        ASSERT_THROW(Address address(publicKey), std::invalid_argument);
    }
}

TEST(MikAddress, FromString) {
    {
        string mikAddress = "mik_3a5m9194xbznhx7nkoc1qsq8ip17dcuyyinzsxmgkeu7o5syqbzhcb7uur1h";
        const auto address = Address(mikAddress);
        ASSERT_EQ(address.string(), mikAddress);
        ASSERT_EQ(hex(address.data()), "a073380e2ea7f47f4b495540be6e6858055ab7ef429fcf66e93365a8f3eba7ef");
    }

    {
        string addressStr = "mik_1111111111111111111111111111111111111111111111111111hifc8npp";
        const auto address = Address(addressStr);
        ASSERT_EQ(address.string(), addressStr);
        ASSERT_EQ(hex(address.data()), "0000000000000000000000000000000000000000000000000000000000000000");
    }
}

TEST(MikAddress, isValid) {
    string mikAddress = "mik_3a5m9194xbznhx7nkoc1qsq8ip17dcuyyinzsxmgkeu7o5syqbzhcb7uur1h";
    string faultyChecksumAddress = "mik_1111111111111111111111111111111111111111111111111111hi111111";
    string bitcoinAddress = "1Ma2DrB78K7jmAwaomqZNRMCvgQrNjE2QC";

    ASSERT_TRUE(Address::isValid(mikAddress));
    ASSERT_FALSE(Address::isValid(faultyChecksumAddress));
    ASSERT_FALSE(Address::isValid(bitcoinAddress));
}
