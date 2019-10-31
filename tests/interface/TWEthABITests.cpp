// Copyright © 2017-2019 Trust Wallet.
//
// This file is part of Trust. The full Trust copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.

#include <TrustWalletCore/TWEthABI.h>
#include <TrustWalletCore/TWString.h>

#include "Data.h"
#include "HexCoding.h"

#include <gtest/gtest.h>
#include <string>

namespace TW::Ethereum {

TEST(TWABI, Func1) {
    TWEthABIFunction func = TWEthABIFunctionCreate("baz");
    EXPECT_TRUE(func != nullptr);
    
    {
        int p1index = TWEthABIFuncAddInParamByte(func, 69);
        EXPECT_EQ(0, p1index);
        int p2index = TWEthABIFuncAddOutParamByte(func);
        EXPECT_EQ(0, p2index);
        // check back get value
        int p2val2 = TWEthABIFuncGetOutParamByte(func, p2index);
        EXPECT_EQ(0, p2val2);
    }
    {
        TWString* p1valStr = TWStringCreateWithUTF8Bytes("0045");
        TWData* p1val = TWDataCreateWithHexString(p1valStr);
        int p1index = TWEthABIFuncAddInParamUInt256(func, p1val);
        EXPECT_EQ(1, p1index);
        TWDataDelete(p1val);
        TWStringDelete(p1valStr);

        int p2index = TWEthABIFuncAddOutParamUInt256(func);
        EXPECT_EQ(1, p2index);

        // check back get value
        TWData* p2val2 = TWEthABIFuncGetOutParamUInt256(func, p2index);
        Data p2val3 = data(TWDataBytes(p2val2), TWDataSize(p2val2));
        EXPECT_EQ("00", hex(p2val3));
    }

    // delete
    TWEthABIFunctionDelete(&func);
    EXPECT_EQ(nullptr, func);
}

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
