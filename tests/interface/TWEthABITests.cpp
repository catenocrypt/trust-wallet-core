// Copyright © 2017-2019 Trust Wallet.
//
// This file is part of Trust. The full Trust copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.

#include <TrustWalletCore/TWEthABI.h>
#include <TrustWalletCore/TWString.h>

#include <gtest/gtest.h>
#include <string>

namespace TW::Ethereum {

TEST(TWABI, EncodeUint32) {
    uint32_t in = 69u;
    TWString* encoded = TWEthABIEncodeUint32(in);
    EXPECT_EQ(std::string("0000000000000000000000000000000000000000000000000000000000000045"), std::string(TWStringUTF8Bytes(encoded)));
}

TEST(TWABI, EncodeString) {
    TWString* string = TWStringCreateWithUTF8Bytes("Hello");
    TWString* encoded = TWEthABIEncodeString(string);
    EXPECT_EQ(std::string(
        "0000000000000000000000000000000000000000000000000000000000000005"\
        "48656c6c6f000000000000000000000000000000000000000000000000000000"), 
        std::string(TWStringUTF8Bytes(encoded)));
}

TEST(TWABI, DecodeString) {
    TWString* encoded = TWStringCreateWithUTF8Bytes(
        "0000000000000000000000000000000000000000000000000000000000000005"\
        "48656c6c6f000000000000000000000000000000000000000000000000000000"
    );
    TWString* decoded;
    bool res = TWEthABIDecodeString(encoded, &decoded);
    EXPECT_TRUE(res);
    EXPECT_EQ(std::string("Hello"), std::string(TWStringUTF8Bytes(decoded)));
}
} // namespace TW::Ethereum
