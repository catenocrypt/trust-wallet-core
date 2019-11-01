// Copyright © 2017-2019 Trust Wallet.
//
// This file is part of Trust. The full Trust copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.

#include "Ethereum/ABI.h"
#include "HexCoding.h"

#include <gtest/gtest.h>

using namespace TW;
using namespace TW::Ethereum::ABI;

TEST(ABI, pad32) {
    EXPECT_EQ(64, Util::paddedTo32(40));
    EXPECT_EQ(32, Util::paddedTo32(32));
    EXPECT_EQ(64, Util::paddedTo32(33));
    EXPECT_EQ(64, Util::paddedTo32(63));
    EXPECT_EQ(64, Util::paddedTo32(64));
    EXPECT_EQ(96, Util::paddedTo32(65));
    EXPECT_EQ(24, Util::padNeeded32(40));
    EXPECT_EQ(0, Util::padNeeded32(32));
    EXPECT_EQ(31, Util::padNeeded32(33));
    EXPECT_EQ(1, Util::padNeeded32(63));
    EXPECT_EQ(0, Util::padNeeded32(64));
    EXPECT_EQ(31, Util::padNeeded32(65));
}

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
    encode(uint256_t(int256_t("-1")), encoded);
    EXPECT_EQ(hex(encoded), "ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff");
}

TEST(ABI, EncodeBigIntOverflow) {
    Data encoded;
    try {
        encode(uint256_t(int256_t("F123456789ABCDEF0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF0")), encoded);
    } catch (std::exception ex) {
        // expected exception
        return;
    }
    FAIL() << "Should fail due to overflow";
}

TEST(ABI, EncodeString) {
    auto p = ParamString("Hello World!    Hello World!    Hello World!");
    EXPECT_EQ("string", p.getType());
    Data encoded;
    p.encode(encoded);
    EXPECT_EQ(hex(encoded), 
        "000000000000000000000000000000000000000000000000000000000000002c"
        "48656c6c6f20576f726c64212020202048656c6c6f20576f726c642120202020"
        "48656c6c6f20576f726c64210000000000000000000000000000000000000000");
}

TEST(ABI, EncodeParamsSimple) {
    auto p = Parameters(std::vector<ParamBase*>{ new ParamUInt256(16u), new ParamUInt256(17u), new ParamBool(true) });
    EXPECT_EQ("(uint256,uint256,bool)", p.getType());
    Data encoded;
    p.encode(encoded);

    EXPECT_EQ(3 * 32, encoded.size());
    EXPECT_EQ(
        "0000000000000000000000000000000000000000000000000000000000000010"
        "0000000000000000000000000000000000000000000000000000000000000011"
        "0000000000000000000000000000000000000000000000000000000000000001",
        hex(encoded));
}

TEST(ABI, EncodeParamsMixed) {
    auto p = Parameters(std::vector<ParamBase*>{
        new ParamUInt256(69u), 
        new ParamArray(std::vector<ParamBase*>{
            new ParamUInt256(1),
            new ParamUInt256(2),
            new ParamUInt256(3)
        }),
        new ParamBool(true),
        new ParamString("Hello"),
        new ParamByteArray(Data{0x64, 0x61, 0x76, 0x65})
    });
    EXPECT_EQ("(uint256,uint256[],bool,string,bytes)", p.getType());
    Data encoded;
    p.encode(encoded);

    EXPECT_EQ(13 * 32, encoded.size());
    EXPECT_EQ(
        "0000000000000000000000000000000000000000000000000000000000000045"
        "00000000000000000000000000000000000000000000000000000000000000a0"
        "0000000000000000000000000000000000000000000000000000000000000001"
        "0000000000000000000000000000000000000000000000000000000000000120"
        "0000000000000000000000000000000000000000000000000000000000000160"
        "0000000000000000000000000000000000000000000000000000000000000003"
        "0000000000000000000000000000000000000000000000000000000000000001"
        "0000000000000000000000000000000000000000000000000000000000000002"
        "0000000000000000000000000000000000000000000000000000000000000003"
        "0000000000000000000000000000000000000000000000000000000000000005"
        "48656c6c6f000000000000000000000000000000000000000000000000000000"
        "0000000000000000000000000000000000000000000000000000000000000004"
        "6461766500000000000000000000000000000000000000000000000000000000",
        hex(encoded));
    /*
     * explained:
     * uint256 69u
     * idx of dynamic vector, 5*32
     * true
     * index of dynamic string, 9*32
     * index of dynamic data, 11*32
     * vector size 3
     * vector val1
     * vector val2
     * vector val3
     * string size 5
     * string
     * data size 4
     * data
     */
}

TEST(ABI, DecodeParamsSimple) {
    Data encoded;
    append(encoded, parse_hex("0000000000000000000000000000000000000000000000000000000000000010"));
    append(encoded, parse_hex("0000000000000000000000000000000000000000000000000000000000000011"));
    append(encoded, parse_hex("0000000000000000000000000000000000000000000000000000000000000001"));
    auto p = Parameters(std::vector<ParamBase*>{ new ParamUInt256(0), new ParamUInt256(0), new ParamBool(false) });
    EXPECT_EQ("(uint256,uint256,bool)", p.getType());
    size_t offset = 0;
    bool res = p.decode(encoded, offset);
    EXPECT_TRUE(res);
    EXPECT_EQ(uint256_t(16u), ((ParamUInt256*)p.getParam(0))->getVal());
    EXPECT_EQ(uint256_t(17u), ((ParamUInt256*)p.getParam(1))->getVal());
    EXPECT_EQ(true, ((ParamBool*)p.getParam(2))->getVal());
    EXPECT_EQ(3 * 32, offset);
}

TEST(ABI, DecodeParamsMixed) {
    Data encoded;
    append(encoded, parse_hex("0000000000000000000000000000000000000000000000000000000000000045"));
    append(encoded, parse_hex("00000000000000000000000000000000000000000000000000000000000000a0"));
    append(encoded, parse_hex("0000000000000000000000000000000000000000000000000000000000000001"));
    append(encoded, parse_hex("0000000000000000000000000000000000000000000000000000000000000120"));
    append(encoded, parse_hex("0000000000000000000000000000000000000000000000000000000000000160"));
    append(encoded, parse_hex("0000000000000000000000000000000000000000000000000000000000000003"));
    append(encoded, parse_hex("0000000000000000000000000000000000000000000000000000000000000001"));
    append(encoded, parse_hex("0000000000000000000000000000000000000000000000000000000000000002"));
    append(encoded, parse_hex("0000000000000000000000000000000000000000000000000000000000000003"));
    append(encoded, parse_hex("0000000000000000000000000000000000000000000000000000000000000005"));
    append(encoded, parse_hex("48656c6c6f000000000000000000000000000000000000000000000000000000"));
    append(encoded, parse_hex("0000000000000000000000000000000000000000000000000000000000000004"));
    append(encoded, parse_hex("6461766500000000000000000000000000000000000000000000000000000000"));
    auto p = Parameters(std::vector<ParamBase*>{
        new ParamUInt256(), 
        new ParamArray(std::vector<ParamBase*>{new ParamUInt256(), new ParamUInt256(), new ParamUInt256()}),
        new ParamBool(),
        new ParamString(),
        new ParamByteArray()
    });
    EXPECT_EQ("(uint256,uint256[],bool,string,bytes)", p.getType());
    size_t offset = 0;
    bool res = p.decode(encoded, offset);
    EXPECT_TRUE(res);
    EXPECT_EQ(uint256_t(69u), ((ParamUInt256*)p.getParam(0))->getVal());
    EXPECT_EQ(3, ((ParamArray*)p.getParam(1))->getCount());
    EXPECT_EQ(1, ((ParamUInt256*)((ParamArray*)p.getParam(1))->getParam(0))->getVal());
    EXPECT_EQ(3, ((ParamUInt256*)((ParamArray*)p.getParam(1))->getParam(2))->getVal());
    EXPECT_EQ(true, ((ParamBool*)p.getParam(2))->getVal());
    EXPECT_EQ("Hello", ((ParamString*)p.getParam(3))->getVal());
    EXPECT_EQ(13 * 32, offset);
}

TEST(ABI, EncodeSignature) {
    auto func = Function("baz", std::vector<ParamBase*>{
        new ParamUInt256(69u), new ParamBool(true)
    });
    EXPECT_EQ("baz(uint256,bool)", func.getType());
    Data encoded;
    func.encode(encoded);

    EXPECT_EQ(encoded.size(), 32 * 2 + 4);
    EXPECT_EQ(hex(encoded.begin(), encoded.begin() + 4), "72ed38b6");
    EXPECT_EQ(hex(encoded.begin() +   4, encoded.begin() + 36 ), "0000000000000000000000000000000000000000000000000000000000000045");
    EXPECT_EQ(hex(encoded.begin() +  36, encoded.begin() + 68 ), "0000000000000000000000000000000000000000000000000000000000000001");
}

TEST(ABI, EncodeFunctionWithDynamicArgumentsCase1) {
    auto func = Function("sam", std::vector<ParamBase*>{
        new ParamByteArray(Data{0x64, 0x61, 0x76, 0x65}),
        new ParamBool(true),
        new ParamArray(std::vector<ParamBase*>{
            new ParamUInt256(1),
            new ParamUInt256(2),
            new ParamUInt256(3)
        })
    });
    EXPECT_EQ("sam(bytes,bool,uint256[])", func.getType());
    Data encoded;
    func.encode(encoded);

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
    auto func = Function("f", std::vector<ParamBase*>{
        new ParamUInt256(0x123),
        new ParamArray(std::vector<ParamBase*>{
            new ParamUInt32(0x456),
            new ParamUInt32(0x789)
        }),
        new ParamByteArrayFix(10, std::vector<byte>{0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x30}),
        new ParamString("Hello, world!")
    });
    EXPECT_EQ("f(uint256,uint32[],bytes10,string)", func.getType());
    Data encoded;
    func.encode(encoded);

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

TEST(ABI, DecodeSignature) {
    Data encoded;
    append(encoded, parse_hex("72ed38b6"));
    append(encoded, parse_hex("0000000000000000000000000000000000000000000000000000000000000045"));
    append(encoded, parse_hex("0000000000000000000000000000000000000000000000000000000000000001"));
    auto func = Function("baz", std::vector<ParamBase*>{
        new ParamUInt256(), new ParamBool()
    });
    EXPECT_EQ("baz(uint256,bool)", func.getType());
    size_t offset = 0;
    bool res = func.decode(encoded, offset);
    EXPECT_TRUE(res);
    ParamBase* param;
    EXPECT_TRUE(func.getInParam(0, &param));
    EXPECT_EQ(69u, ((ParamUInt256*)param)->getVal());
    EXPECT_TRUE(func.getInParam(1, &param));
    EXPECT_EQ(true, ((ParamBool*)param)->getVal());
    EXPECT_EQ(4 + 2 * 32, offset);  
}

TEST(ABI, DecodeFunctionWithDynamicArgumentsCase1) {
    Data encoded;
    append(encoded, parse_hex("a5643bf2"));
    append(encoded, parse_hex("0000000000000000000000000000000000000000000000000000000000000060"));
    append(encoded, parse_hex("0000000000000000000000000000000000000000000000000000000000000001"));
    append(encoded, parse_hex("00000000000000000000000000000000000000000000000000000000000000a0"));
    append(encoded, parse_hex("0000000000000000000000000000000000000000000000000000000000000004"));
    append(encoded, parse_hex("6461766500000000000000000000000000000000000000000000000000000000"));
    append(encoded, parse_hex("0000000000000000000000000000000000000000000000000000000000000003"));
    append(encoded, parse_hex("0000000000000000000000000000000000000000000000000000000000000001"));
    append(encoded, parse_hex("0000000000000000000000000000000000000000000000000000000000000002"));
    append(encoded, parse_hex("0000000000000000000000000000000000000000000000000000000000000003"));

    auto func = Function("sam", std::vector<ParamBase*>{
        new ParamByteArray(Data{0x64, 0x61, 0x76, 0x65}),
        new ParamBool(true),
        new ParamArray(std::vector<ParamBase*>{
            new ParamUInt256(1),
            new ParamUInt256(2),
            new ParamUInt256(3)
        })
    });
    EXPECT_EQ("sam(bytes,bool,uint256[])", func.getType());

    size_t offset = 0;
    bool res = func.decode(encoded, offset);
    EXPECT_TRUE(res);
    ParamBase* param;
    EXPECT_TRUE(func.getInParam(0, &param));
    EXPECT_EQ(4, ((ParamByteArray*)param)->getCount());
    EXPECT_EQ(0x64, ((ParamByteArray*)param)->getVal()[0]);
    EXPECT_EQ(0x65, ((ParamByteArray*)param)->getVal()[3]);
    EXPECT_TRUE(func.getInParam(1, &param));
    EXPECT_EQ(true, ((ParamBool*)param)->getVal());
    EXPECT_TRUE(func.getInParam(2, &param));
    EXPECT_EQ(3, ((ParamArray*)param)->getCount());
    EXPECT_EQ(uint256_t(1), ((ParamUInt256*)((ParamArray*)param)->getVal()[0])->getVal());
    EXPECT_EQ(uint256_t(3), ((ParamUInt256*)((ParamArray*)param)->getVal()[2])->getVal());
    EXPECT_EQ(4 + 9 * 32, offset);
}

TEST(ABI, DecodeFunctionWithDynamicArgumentsCase2) {
    Data encoded;
    append(encoded, parse_hex("47b941bf"));
    append(encoded, parse_hex("0000000000000000000000000000000000000000000000000000000000000123"));
    append(encoded, parse_hex("0000000000000000000000000000000000000000000000000000000000000080"));
    append(encoded, parse_hex("3132333435363738393000000000000000000000000000000000000000000000"));
    append(encoded, parse_hex("00000000000000000000000000000000000000000000000000000000000000e0"));
    append(encoded, parse_hex("0000000000000000000000000000000000000000000000000000000000000002"));
    append(encoded, parse_hex("0000000000000000000000000000000000000000000000000000000000000456"));
    append(encoded, parse_hex("0000000000000000000000000000000000000000000000000000000000000789"));
    append(encoded, parse_hex("000000000000000000000000000000000000000000000000000000000000000d"));
    append(encoded, parse_hex("48656c6c6f2c20776f726c642100000000000000000000000000000000000000"));

    auto func = Function("f", std::vector<ParamBase*>{
        new ParamUInt256(0x123),
        new ParamArray(std::vector<ParamBase*>{
            new ParamUInt32(0x456),
            new ParamUInt32(0x789)
        }),
        new ParamByteArrayFix(10, std::vector<byte>{0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x30}),
        new ParamString("Hello, world!")
    });
    EXPECT_EQ("f(uint256,uint32[],bytes10,string)", func.getType());

    size_t offset = 0;
    bool res = func.decode(encoded, offset);
    EXPECT_TRUE(res);
    ParamBase* param;
    EXPECT_TRUE(func.getInParam(0, &param));
    EXPECT_EQ(uint256_t(0x123), ((ParamUInt256*)param)->getVal());
    EXPECT_TRUE(func.getInParam(1, &param));
    EXPECT_EQ(2, ((ParamArray*)param)->getCount());
    EXPECT_EQ(0x456, ((ParamUInt32*)((ParamArray*)param)->getVal()[0])->getVal());
    EXPECT_EQ(0x789, ((ParamUInt32*)((ParamArray*)param)->getVal()[1])->getVal());
    EXPECT_TRUE(func.getInParam(2, &param));
    EXPECT_EQ(10, ((ParamByteArrayFix*)param)->getCount());
    EXPECT_EQ("31323334353637383930", hex(((ParamByteArrayFix*)param)->getVal()));
    EXPECT_TRUE(func.getInParam(3, &param));
    EXPECT_EQ(std::string("Hello, world!"), ((ParamString*)param)->getVal());
    EXPECT_EQ(4 + 9 * 32, offset);
}

TEST(ABI, EncodeVectorByte10) {
    auto p = ParamByteArrayFix(10, std::vector<byte>{0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x30});
    EXPECT_EQ("bytes10", p.getType());
    Data encoded;
    p.encode(encoded);
    EXPECT_EQ("3132333435363738393000000000000000000000000000000000000000000000", hex(encoded));
}

TEST(ABI, EncodeVectorByte) {
    auto p = ParamByteArray(std::vector<byte>{0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x30});
    EXPECT_EQ("bytes", p.getType());
    Data encoded;
    p.encode(encoded);
    EXPECT_EQ(
        "000000000000000000000000000000000000000000000000000000000000000a"
        "3132333435363738393000000000000000000000000000000000000000000000", hex(encoded));
}

TEST(ABI, DecodeUInt) {
    Data encoded = parse_hex("000000000000000000000000000000000000000000000000000000000000002a");
    size_t offset = 0;
    uint256_t decoded;
    bool res = ParamNumber<uint256_t>::decodeNumber(encoded, decoded, offset);
    EXPECT_TRUE(res);
    EXPECT_EQ(uint256_t(42), decoded);
    EXPECT_EQ(32, offset);
}

TEST(ABI, DecodeUInt8) {
    Data encoded = parse_hex("0000000000000000000000000000000000000000000000000000000000000018");
    size_t offset = 0;
    uint8_t decoded;
    bool res = ParamNumber<uint8_t>::decodeNumber(encoded, decoded, offset);
    EXPECT_TRUE(res);
    EXPECT_EQ(24, decoded);
    EXPECT_EQ(32, offset);
}

TEST(ABI, DecodeUInt8WithOffset) {
    Data encoded = parse_hex("abcdef0000000000000000000000000000000000000000000000000000000000000018");
    size_t offset = 3;
    uint8_t decoded;
    bool res = ParamNumber<uint8_t>::decodeNumber(encoded, decoded, offset);
    EXPECT_TRUE(res);
    EXPECT_EQ(24, decoded);
    EXPECT_EQ(3 + 32, offset);
}

TEST(ABI, DecodeUIntWithOffset) {
    Data encoded = parse_hex("abcdef000000000000000000000000000000000000000000000000000000000000002a");
    size_t offset = 3;
    uint256_t decoded;
    bool res = decode(encoded, decoded, offset);
    EXPECT_TRUE(res);
    EXPECT_EQ(uint256_t(42), decoded);
    EXPECT_EQ(3 + 32, offset);
}

TEST(ABI, DecodeUIntErrorTooShort) {
    Data encoded = parse_hex("000000000000000000000000000000000000000000000000002a");
    size_t offset = 0;
    uint256_t decoded;
    bool res = decode(encoded, decoded, offset);
    EXPECT_FALSE(res);
    EXPECT_EQ(uint256_t(0), decoded);
    EXPECT_EQ(0, offset);
}

TEST(ABI, DecodeArrayUint) {
    Data encoded;
    append(encoded, parse_hex("000000000000000000000000000000000000000000000000000000000000000a"));
    append(encoded, parse_hex("3132333435363738393000000000000000000000000000000000000000000000"));
    size_t offset = 0;
    std::vector<byte> decoded;
    bool res = ParamByteArray::decodeBytes(encoded, decoded, offset);
    EXPECT_TRUE(res);
    EXPECT_EQ(10, decoded.size());
    if (decoded.size() >= 2) {
        EXPECT_EQ(49u, decoded[0]);
        EXPECT_EQ(50u, decoded[1]);
    }
    EXPECT_EQ(32 + 32, offset);
}

TEST(ABI, DecodeArrayTooShort) {
    Data encoded;
    append(encoded, parse_hex("000000000000000000000000000000000000000000000000000000000000000a"));
    append(encoded, parse_hex("313233343536373839"));
    size_t offset = 0;
    std::vector<byte> decoded;
    bool res = ParamByteArray::decodeBytes(encoded, decoded, offset);
    EXPECT_FALSE(res);
}

TEST(ABI, DecodeArrayInvalidLen) {
    Data encoded = parse_hex("0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef");
    size_t offset = 0;
    std::vector<byte> decoded;
    bool res = ParamByteArray::decodeBytes(encoded, decoded, offset);
    EXPECT_FALSE(res);
}

TEST(ABI, DecodeByteArray) {
    Data encoded;
    append(encoded, parse_hex("000000000000000000000000000000000000000000000000000000000000000a"));
    append(encoded, parse_hex("3132333435363738393000000000000000000000000000000000000000000000"));
    size_t offset = 0;
    Data decoded;
    bool res = ParamByteArray::decodeBytes(encoded, decoded, offset);
    EXPECT_TRUE(res);
    EXPECT_EQ("31323334353637383930", hex(decoded));
    EXPECT_EQ(32 + 32, offset);
}

TEST(ABI, DecodeByteArray10) {
    Data encoded = parse_hex("3132333435363738393000000000000000000000000000000000000000000000");
    size_t offset = 0;
    Data decoded;
    bool res = ParamByteArrayFix::decodeBytesFix(encoded, 10, decoded, offset);
    EXPECT_TRUE(res);
    EXPECT_EQ(10, decoded.size());
    EXPECT_EQ(49u, decoded[0]);
    EXPECT_EQ(50u, decoded[1]);
    EXPECT_EQ(32, offset);
}

TEST(ABI, DecodeString) {
    Data encoded;
    append(encoded, parse_hex("000000000000000000000000000000000000000000000000000000000000002c"));
    append(encoded, parse_hex("48656c6c6f20576f726c64212020202048656c6c6f20576f726c642120202020"));
    append(encoded, parse_hex("48656c6c6f20576f726c64210000000000000000000000000000000000000000"));
    size_t offset = 0;
    std::string decoded;
    bool res = ParamString::decodeString(encoded, decoded, offset);
    EXPECT_TRUE(res);
    EXPECT_EQ("Hello World!    Hello World!    Hello World!", decoded);
    EXPECT_EQ(32 + 2 * 32, offset);
}
