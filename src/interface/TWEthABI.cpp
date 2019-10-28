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

TWString *_Nonnull TWEthABI_AppendHello(TWString *_Nonnull str) {
    std::string str2 = TWStringUTF8Bytes(str);
    std::string str3 = str2 + " Hello";
    TWString* str4 = TWStringCreateWithUTF8Bytes(str3.data());
    return str4;
}
