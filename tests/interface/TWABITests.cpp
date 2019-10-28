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

TEST(TWABI, Hello){
    TWString* in1 = TWStringCreateWithUTF8Bytes("One");
    TWString* str2 = TWEthABI_AppendHello(in1);
    std::string str3 = TWStringUTF8Bytes(str2);
    EXPECT_EQ(std::string("One Hello"), str3);
}

TEST(TWABI, EncodeUint32) {
    uint32_t in = 69u;
    TWString* encoded = TWEthABIEncodeUint32(in);
    EXPECT_EQ(std::string("0000000000000000000000000000000000000000000000000000000000000045"), std::string(TWStringUTF8Bytes(encoded)));
}

} // namespace TW::Ethereum
