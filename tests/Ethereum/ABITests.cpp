// Copyright © 2017-2019 Trust Wallet.
//
// This file is part of Trust. The full Trust copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.

#include "Ethereum/ABI.h"
#include "HexCoding.h"

//#include <TrustWalletCore/TWEthABI.h>

#include <gtest/gtest.h>

namespace TW::Ethereum {

TEST(ABI, EncodeBool) {
    {   // True
        Data encoded;
        encode(true, encoded);

        EXPECT_EQ(hex(encoded), "0000000000000000000000000000000000000000000000000000000000000001");
    }
    {   // False
        Data encoded;
        encode(false, encoded);

        EXPECT_EQ(hex(encoded), "0000000000000000000000000000000000000000000000000000000000000000");
    }
}

TEST(ABI, EncodeUInt) {
    Data encoded;
    encode(69u, encoded);
    
    EXPECT_EQ(hex(encoded), "0000000000000000000000000000000000000000000000000000000000000045");
}

TEST(ABI, EncodeNegativeInt) {
    Data encoded;
    encode(-1, encoded);
    
    EXPECT_EQ(hex(encoded), "ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff");
}

TEST(ABI, EncodeBigUInt) {
    Data encoded;
    encode(uint256_t("0x1234567890123456789012345678901234567890"), encoded);
    
    EXPECT_EQ(hex(encoded), "0000000000000000000000001234567890123456789012345678901234567890");
}

TEST(ABI, EncodeNegativeBigInt) {
    Data encoded;
    encode(int256_t("-1"), encoded);
    
    EXPECT_EQ(hex(encoded), "ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff");
}

TEST(ABI, EncodeBigIntOverflow) {
    Data encoded;
    try {
        encode(int256_t("F123456789ABCDEF0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF0"), encoded);
    } catch (std::exception ex) {
        // expected exception
        return;
    }
    FAIL() << "publicKey not found in message.accountKeys";
}

TEST(ABI, EncodeSignature) {
    Data encoded;
    auto function = Function("baz", std::make_tuple(uint256_t(69u), true));
    encode(function, encoded);

    EXPECT_EQ(encoded.size(), 32 * 2 + 4);
    EXPECT_EQ(hex(encoded.begin(), encoded.begin() + 4), "72ed38b6");
    EXPECT_EQ(hex(encoded.begin() +   4, encoded.begin() + 36 ), "0000000000000000000000000000000000000000000000000000000000000045");
    EXPECT_EQ(hex(encoded.begin() +  36, encoded.begin() + 68 ), "0000000000000000000000000000000000000000000000000000000000000001");
}

TEST(ABI, EncodeFunctionWithDynamicArgumentsCase1) {
    Data encoded;
    auto function = Function("sam", std::make_tuple(Data{0x64, 0x61, 0x76, 0x65}, true, std::vector<uint256_t>{1, 2, 3}));
    encode(function, encoded);

    EXPECT_EQ(encoded.size(), 32 * 9 + 4);
    EXPECT_EQ(hex(encoded.begin() +   0, encoded.begin() + 4  ), "a5643bf2");
    EXPECT_EQ(hex(encoded.begin() +   4, encoded.begin() + 36 ), "0000000000000000000000000000000000000000000000000000000000000060");
    EXPECT_EQ(hex(encoded.begin() +  36, encoded.begin() + 68 ), "0000000000000000000000000000000000000000000000000000000000000001");
    EXPECT_EQ(hex(encoded.begin() +  68, encoded.begin() + 100), "00000000000000000000000000000000000000000000000000000000000000a0");
    EXPECT_EQ(hex(encoded.begin() + 100, encoded.begin() + 132), "0000000000000000000000000000000000000000000000000000000000000004");
    EXPECT_EQ(hex(encoded.begin() + 132, encoded.begin() + 164), "6461766500000000000000000000000000000000000000000000000000000000");
    EXPECT_EQ(hex(encoded.begin() + 164, encoded.begin() + 196), "0000000000000000000000000000000000000000000000000000000000000003");
    EXPECT_EQ(hex(encoded.begin() + 196, encoded.begin() + 228), "0000000000000000000000000000000000000000000000000000000000000001");
    EXPECT_EQ(hex(encoded.begin() + 228, encoded.begin() + 260), "0000000000000000000000000000000000000000000000000000000000000002");
    EXPECT_EQ(hex(encoded.begin() + 260, encoded.begin() + 292), "0000000000000000000000000000000000000000000000000000000000000003");
}

TEST(ABI, EncodeFunctionWithDynamicArgumentsCase2) {
    Data encoded;
    auto function = Function("f", std::make_tuple(
        uint256_t(0x123),
        std::vector<uint32_t>{0x456, 0x789},
        std::array<byte, 10>{0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x30},
        std::string("Hello, world!")
    ));
    encode(function, encoded);

    EXPECT_EQ(encoded.size(), 32 * 9 + 4);
    EXPECT_EQ(hex(encoded.begin() +   0, encoded.begin() + 4  ), "47b941bf");
    EXPECT_EQ(hex(encoded.begin() +   4, encoded.begin() + 36 ), "0000000000000000000000000000000000000000000000000000000000000123");
    EXPECT_EQ(hex(encoded.begin() +  36, encoded.begin() + 68 ), "0000000000000000000000000000000000000000000000000000000000000080");
    EXPECT_EQ(hex(encoded.begin() +  68, encoded.begin() + 100), "3132333435363738393000000000000000000000000000000000000000000000");
    EXPECT_EQ(hex(encoded.begin() + 100, encoded.begin() + 132), "00000000000000000000000000000000000000000000000000000000000000e0");
    EXPECT_EQ(hex(encoded.begin() + 132, encoded.begin() + 164), "0000000000000000000000000000000000000000000000000000000000000002");
    EXPECT_EQ(hex(encoded.begin() + 164, encoded.begin() + 196), "0000000000000000000000000000000000000000000000000000000000000456");
    EXPECT_EQ(hex(encoded.begin() + 196, encoded.begin() + 228), "0000000000000000000000000000000000000000000000000000000000000789");
    EXPECT_EQ(hex(encoded.begin() + 228, encoded.begin() + 260), "000000000000000000000000000000000000000000000000000000000000000d");
    EXPECT_EQ(hex(encoded.begin() + 260, encoded.begin() + 292), "48656c6c6f2c20776f726c642100000000000000000000000000000000000000");
}

TEST(ABI, EncodeStaticBytes) {
    Data encoded;
    encode(std::array<uint8_t, 6>{0, 1, 2, 3, 4, 5}, encoded);
    EXPECT_EQ(hex(encoded), "0001020304050000000000000000000000000000000000000000000000000000");
}

TEST(ABI, EncodeDynamicBytes) {
    Data encoded;
    encode(std::vector<uint8_t>{0, 1, 2, 3, 4, 5}, encoded);
    EXPECT_EQ(hex(encoded), "0000000000000000000000000000000000000000000000000000000000000006"\
        "0001020304050000000000000000000000000000000000000000000000000000");
}

TEST(ABI, Decode) {
    Data encoded = parse_hex("000000000000000000000000000000000000000000000000000000000000002a");
    uint256_t decoded = decodeUInt256(encoded);
    EXPECT_EQ(uint256_t(42), decoded);
}

} // namespace TW::Ethereum
