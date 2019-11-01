// Copyright © 2017-2019 Trust Wallet.
//
// This file is part of Trust. The full Trust copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.

#pragma once

#include "ParamBase.h"
#include "Parameters.h"

#include "../../uint256.h"
#include "../../Hash.h"

#include <string>
#include <cassert>





// TODO remove
#include "Tuple.h"
#include "Array.h"
#include "Numbers.h"



namespace TW::Ethereum::ABI {

// TODO move some impl to CPP file

/// Non-generic version of Function, templated version is impossible to pass around to and back over C interface
/// (void* looses the temaplate parameters).
class Function {
public:
    std::string name;
    ParamSet _inParams;
    ParamSet _outParams;

    Function(std::string name) : name(std::move(name)) {}
    Function(std::string name, std::vector<ParamBase*> inParams)
        : name(std::move(name)), _inParams(ParamSet(inParams)) {}

    /// Returns the index of the parameter
    int addInParam(ParamBase* param)
    {
        return _inParams.addParam(param);
    }

    /// Returns the index of the parameter
    int addOutParam(ParamBase* param)
    {
        return _outParams.addParam(param);
    }

    bool getInParam(int paramIndex, ParamBase** param_out)
    {
        return _inParams.getParam(paramIndex, param_out);
    }

    bool getOutParam(int paramIndex, ParamBase** param_out)
    {
        return _outParams.getParam(paramIndex, param_out);
    }

    /// Return the function type signature, of the form "baz(int32,uint256)"
    std::string getType() const {
        return name + _inParams.getType();
    }

    /// Return the 4-byte function signature
    Data getSignature() const {
        auto typ = getType();
        auto hash = Hash::keccak256(Data(typ.begin(), typ.end()));
        auto signature = Data(hash.begin(), hash.begin() + 4);
        return signature;
    }

    void encode(Data& data) const {
        Data signature = getSignature();
        append(data, signature);
        _inParams.encode(data);
    }

    inline bool decode(const Data& encoded, size_t& offset_inout) {
        // read 4-byte hash
        auto p = ParamBytesFix(4);
        if (!p.decode(encoded, offset_inout)) { return false; }
        std::vector<byte> hash = p.getVal();
        // adjust offset; hash is NOT padded to 32 bytes
        offset_inout = offset_inout - 32 + 4;
        // verify hash
        auto string = getType();
        Data hashExpect = getSignature();
        if (hash != hashExpect) {
            // invalid hash
            return false;
        }
        // read parameters
        if (!_inParams.decode(encoded, offset_inout)) { return false; }
        return true;
    }
};

inline void encode(const Function& func, Data& data) {
    func.encode(data);
}



/*
template <typename... Params>
class Function {
  public:
    std::string name;
    std::tuple<Params...> parameters;

    Function() = default;
    Function(std::string name, std::tuple<Params...> parameters)
        : name(std::move(name)), parameters(std::move(parameters)) {}
};

template <typename... Params>
inline bool is_dynamic(const Function<Params...>& f) {
    return is_dynaic(f.parameters);
}

template <typename... Params>
inline bool size(const Function<Params...>& f) {
    return 4 + size(f.parameters);
}

template <typename... Params>
inline void encode(const Function<Params...>& f, Data& data) {
    auto string = type_string(f);
    auto hash = Hash::keccak256(Data(string.begin(), string.end()));
    auto signature = Data(hash.begin(), hash.begin() + 4);
    append(data, signature);
    encode(f.parameters, data);
}

template <typename... Params>
inline bool decode(const Data& encoded, Function<Params...>& function, size_t& offset_inout) {
    // read 4-byte hash
    std::array<byte, 4> hash;
    if (!decode(encoded, hash, offset_inout)) { return false; }
    // adjust offset; hash is NOT padded to 32 bytes
    offset_inout = offset_inout - 32 + 4;
    // verify hash
    auto string = type_string(function);
    Data hashExpect = Hash::keccak256(Data(string.begin(), string.end()));
    hashExpect = Data(hashExpect.begin(), hashExpect.begin() + 4);
    if (Data(hash.begin(), hash.end()) != hashExpect) {
        // invalid hash
        return false;
    }
    // read parameters
    if (!decode(encoded, function.parameters, offset_inout)) { return false; }
    return true;
}

template <typename... Params>
inline std::string type_string(const Function<Params...>& f) {
    return f.name + "(" + type_string(f.parameters) + ")";
}
*/

} // namespace TW::Ethereum::ABI
