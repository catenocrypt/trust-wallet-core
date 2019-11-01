#include <TrustWalletCore/TWEthABI.h>
#include <TrustWalletCore/TWString.h>
#include <TrustWalletCore/TWData.h>

#include "Ethereum/ABI.h"  // TODO include this full ABI, once all sub-headers are inlined
//#include "Ethereum/ABI/Numbers.h"
//#include "Ethereum/ABI/Array.h"
#include "Data.h"
#include "HexCoding.h"
#include "../uint256.h"

#include <string>
#include <vector>

using namespace TW;
using namespace TW::Ethereum;
using namespace TW::Ethereum::ABI;


TWEthABIFunction TWEthABIFunctionCreate(const char *_Nonnull name)
{
    auto func = new Function(name);
    return static_cast<TWEthABIFunction>(func);
}

void TWEthABIFunctionDelete(TWEthABIFunction *_Nonnull function_inout)
{
    auto func = static_cast<const Function*>(*function_inout);
    delete func;
    *function_inout = nullptr;
}

int TWEthABIFuncAddInParamInt32(TWEthABIFunction func, uint8_t val)
{
    auto function = static_cast<Function*>(func);
    assert(function != nullptr);
    auto param = new ParamInt32(val);
    return function->addInParam(static_cast<ParamBase*>(param));
}

int TWEthABIFuncAddInParamUInt256(TWEthABIFunction func, TWData *__nonnull val)
{
    auto function = static_cast<Function*>(func);
    assert(function != nullptr);
    uint256_t val2 = load(*static_cast<const Data*>(val));
    auto param = new ParamUInt256(val2);
    return function->addInParam(static_cast<ParamBase*>(param));
}

int TWEthABIFuncAddOutParamInt32(TWEthABIFunction func)
{
    auto function = static_cast<Function*>(func);
    assert(function != nullptr);
    auto param = new ParamInt32();
    return function->addOutParam(static_cast<ParamBase*>(param));
}

int TWEthABIFuncAddOutParamUInt256(TWEthABIFunction func)
{
    auto function = static_cast<Function*>(func);
    assert(function != nullptr);
    auto param = new ParamUInt256();
    return function->addOutParam(static_cast<ParamBase*>(param));
}

uint8_t TWEthABIFuncGetOutParamInt32(TWEthABIFunction func, int paramIndex) {
    auto function = static_cast<Function*>(func);
    assert(function != nullptr);
    ParamBase* baseParam = nullptr;
    if (!function->getOutParam(paramIndex, &baseParam)) {
        return 0;
    }
    auto param2 = static_cast<ParamInt32*>(baseParam);
    assert(param2 != nullptr);
    return param2->getVal();
}

TWData *_Nonnull TWEthABIFuncGetOutParamUInt256(TWEthABIFunction func, int paramIndex) {
    auto function = static_cast<Function*>(func);
    assert(function != nullptr);
    ParamBase* baseParam = nullptr;
    if (!function->getOutParam(paramIndex, &baseParam)) {
        return new Data(0);
    }
    auto param2 = static_cast<ParamUInt256*>(baseParam);
    assert(param2 != nullptr);
    Data data1 = store(param2->getVal());
    Data* data2 = new Data(data1); // copy for allocation
    return static_cast<TWData*>(data2);
}

TWData *_Nonnull TWEthABIFuncEncode(TWEthABIFunction func) {
    auto function = static_cast<Function*>(func);
    assert(function != nullptr);
    Data* encoded = new Data(); // allocated on heap
    function->encode(*encoded);
    return static_cast<TWData*>(encoded);
}




TWString *_Nonnull TWEthABIEncodeUint32(uint32_t i) {
    Data encoded;
    encode(i, encoded);
    TWString* encHex = TWStringCreateWithUTF8Bytes(hex(encoded).data());
    return encHex;
}

TWString *_Nonnull TWEthABIEncodeString(TWString *_Nonnull string) {
    auto str2 = std::string(TWStringUTF8Bytes(string));
    Data encoded;
    ParamString::encodeString(str2, encoded);
    TWString* encHex = TWStringCreateWithUTF8Bytes(hex(encoded).data());
    return encHex;
}

bool TWEthABIDecodeString(TWString *_Nonnull encoded_in, TWString *_Nonnull *_Nonnull decoded_out)
{
    Data encoded = parse_hex(std::string(TWStringUTF8Bytes(encoded_in))); //data(std::string(TWStringUTF8Bytes(encoded_in)));
    std::string decoded;
    size_t offset = 0;
    bool res = ParamString::decodeString(encoded, decoded, offset);
    *decoded_out = TWStringCreateWithUTF8Bytes(decoded.data());
    return res;
}
