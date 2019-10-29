#include <TrustWalletCore/TWEthABI.h>
//#include "Ethereum/ABI.h"  // TODO include this full ABI, once all sub-headers are inlined
#include "Ethereum/ABI/Numbers.h"
#include "Ethereum/ABI/Array.h"
#include "Data.h"
#include "HexCoding.h"
#include <TrustWalletCore/TWString.h>

#include <string>

using namespace TW;
using namespace TW::Ethereum;

TWString *_Nonnull TWEthABIEncodeUint32(uint32_t i) {
    Data encoded;
    encode(i, encoded);
    TWString* encHex = TWStringCreateWithUTF8Bytes(hex(encoded).data());
    return encHex;
}

TWString *_Nonnull TWEthABIEncodeString(TWString *_Nonnull string) {
    Data encoded;
    encode(std::string(TWStringUTF8Bytes(string)), encoded);
    TWString* encHex = TWStringCreateWithUTF8Bytes(hex(encoded).data());
    return encHex;
}

bool TWEthABIDecodeString(TWString *_Nonnull encoded_in, TWString *_Nonnull *_Nonnull decoded_out)
{
    Data encoded = parse_hex(std::string(TWStringUTF8Bytes(encoded_in))); //data(std::string(TWStringUTF8Bytes(encoded_in)));
    std::string decoded;
    size_t offset = 0;
    bool res = decode(encoded, decoded, offset);
    *decoded_out = TWStringCreateWithUTF8Bytes(decoded.data());
    return res;
}
