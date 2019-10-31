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


TWEthABIFunction TWEthABIFunctionCreate(const char *_Nonnull name)
{
    auto func = new FunctionNG(name);
    return static_cast<TWEthABIFunction>(func);
}

void TWEthABIFunctionDelete(TWEthABIFunction *_Nonnull function_inout)
{
    auto func = static_cast<const FunctionNG*>(*function_inout);
    delete func;
    *function_inout = nullptr;
}

int TWEthABIFuncAddInParamByte(TWEthABIFunction func, uint8_t val)
{
    auto function = static_cast<FunctionNG*>(func);
    assert(function != nullptr);
    auto param = new FunctionNG::ParamByte(val);
    return function->AddInParam(static_cast<FunctionNG::ParamBase*>(param));
}

int TWEthABIFuncAddInParamUInt256(TWEthABIFunction func, TWData *__nonnull val)
{
    auto function = static_cast<FunctionNG*>(func);
    assert(function != nullptr);
    uint256_t val2 = load(*static_cast<const Data*>(val));
    auto param = new FunctionNG::ParamUInt256(val2);
    return function->AddInParam(static_cast<FunctionNG::ParamBase*>(param));
}

int TWEthABIFuncAddOutParamByte(TWEthABIFunction func)
{
    auto function = static_cast<FunctionNG*>(func);
    assert(function != nullptr);
    auto param = new FunctionNG::ParamByte();
    return function->AddOutParam(static_cast<FunctionNG::ParamBase*>(param));
}

int TWEthABIFuncAddOutParamUInt256(TWEthABIFunction func)
{
    auto function = static_cast<FunctionNG*>(func);
    assert(function != nullptr);
    auto param = new FunctionNG::ParamUInt256();
    return function->AddOutParam(static_cast<FunctionNG::ParamBase*>(param));
}

uint8_t TWEthABIFuncGetOutParamByte(TWEthABIFunction func, int paramIndex)
{
    auto function = static_cast<FunctionNG*>(func);
    assert(function != nullptr);
    FunctionNG::ParamBase* baseParam = nullptr;
    if (!function->GetOutParam(paramIndex, &baseParam)) {
        return 0;
    }
    auto param2 = static_cast<FunctionNG::ParamByte*>(baseParam);
    assert(param2 != nullptr);
    return param2->getVal();
}

TWData *_Nonnull TWEthABIFuncGetOutParamUInt256(TWEthABIFunction func, int paramIndex)
{
    auto function = static_cast<FunctionNG*>(func);
    assert(function != nullptr);
    FunctionNG::ParamBase* baseParam = nullptr;
    if (!function->GetOutParam(paramIndex, &baseParam)) {
        return new Data(0);
    }
    auto param2 = static_cast<FunctionNG::ParamUInt256*>(baseParam);
    assert(param2 != nullptr);
    Data data1 = store(param2->getVal());
    Data* data2 = new Data(data1); // copy for allocation
    return static_cast<TWData*>(data2);
}




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
