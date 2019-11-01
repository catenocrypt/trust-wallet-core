// Copyright © 2017-2019 Trust Wallet.
//
// This file is part of Trust. The full Trust copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.

#pragma once

#include "ParamBase.h"
#include "ParamNumber.h"
#include "Parameters.h"

#include <vector>
#include <cassert>


// TODO remove
#include "../../Data.h"
#include "../../uint256.h"
#include "Numbers.h"
#include "Bytes.h"

#include <numeric>

namespace TW::Ethereum::ABI {

/// Dynamic length array of the same types
class ParamArray: public ParamCollection
{
private:
    ParamSet _params;

public:
    ParamArray() = default;
    ParamArray(ParamBase* param1) : ParamCollection() { addParam(param1); }
    ParamArray(std::vector<ParamBase*> params) : ParamCollection() { addParams(params); }
    void addParam(ParamBase* param) {
        if (param == nullptr) return;
        assert(param != nullptr);
        if (_params.getCount() >= 1 && param->getType() != getFirstType()) return; // do not add different types
        _params.addParam(param);
    }
    void addParams(std::vector<ParamBase*> params) {
        for (auto p: params) addParam(p);
    }
    std::string getFirstType() const {
        if (_params.getCount() == 0) return "null";
        return _params.getParamUnsafe(0)->getType();
    }
    ParamBase* getParam(int paramIndex) { return _params.getParamUnsafe(paramIndex); }
    virtual std::string getType() const { return getFirstType() + "[]"; }
    virtual size_t getSize() const { return _params.getSize(); }
    virtual bool isDynamic() const { return true; }
    virtual size_t getCount() const { return _params.getCount(); }

    virtual void encode(Data& data) const {
        size_t n = _params.getCount();
        TW::Ethereum::ABI::encode(uint256_t(n), data);

        size_t headSize = 0;
        for (auto i = 0; i < n; ++i) {
            auto p = _params.getParamUnsafe(i);
            if (p->isDynamic()) {
                headSize += 32;
            } else {
                headSize += p->getSize();
            }
        }

        size_t dynamicOffset = 0;
        for (auto i = 0; i < n; ++i) {
            auto p = _params.getParamUnsafe(i);
            if (p->isDynamic()) {
                TW::Ethereum::ABI::encode(uint256_t(headSize + dynamicOffset), data);
                dynamicOffset += p->getSize();
            } else {
                p->encode(data);
            }
        }

        for (auto i = 0; i < n; ++i) {
            auto p = _params.getParamUnsafe(i);
            if (p->isDynamic()) {
                p->encode(data);
            }
        }
    }

    bool decode(const Data& encoded, size_t& offset_inout) {
        size_t origOffset = offset_inout;
        // read length
        uint256_t len256;
        if (!TW::Ethereum::ABI::decode(encoded, len256, offset_inout)) { return false; }
        // check if length is in the size_t range
        size_t len = static_cast<size_t>(len256);
        if (len256 != static_cast<uint256_t>(len)) { return false; }
        // read values
        // TODO how to create values on the file to have the correct number?
        auto n = _params.getCount();
        for (auto i = 0; i < n; ++i) {
            if (!_params.getParamUnsafe(i)->decode(encoded, offset_inout)) { return false; }
        }
        // padding
        offset_inout = origOffset + Util::paddedTo32(offset_inout - origOffset);
        // TODO handle dynamic types
        return true;
    }
};


/*
template <typename T>
inline bool is_dynamic(std::vector<T>) {
    return true;
}

template <typename T>
inline std::size_t size(const std::vector<T>& array) {
    size_t rest = std::accumulate(array.begin(), array.end(), 0u,
                                [](size_t sum, auto x) { return sum + size(x); });
    return 32 + paddedTo32(rest);
}

template <typename T>
inline void encode(const std::vector<T>& array, Data& data) {
    encode(uint256_t(array.size()), data);

    std::size_t headSize = 0;
    for (auto& x : array) {
        if (is_dynamic(x)) {
            headSize += 32;
        } else {
            headSize += size(x);
        }
    }

    std::size_t dynamicOffset = 0;
    for (auto& x : array) {
        if (is_dynamic(x)) {
            encode(uint256_t(headSize + dynamicOffset), data);
            dynamicOffset += size(x);
        } else {
            encode(x, data);
        }
    }

    for (auto& x : array) {
        if (is_dynamic(x))
            encode(x, data);
    }
}

template <typename T>
inline std::string type_string(const std::vector<T>& array) {
    return type_string(array[0]) + "[]";
}

template <typename T>
inline bool decode(const Data& encoded, std::vector<T>& decoded, size_t& offset_inout) {
    size_t origOffset = offset_inout;
    // read length
    uint256_t len256;
    if (!decode(encoded, len256, offset_inout)) { return false; }
    // check if length is in the size_t range
    size_t len = static_cast<size_t>(len256);
    if (len256 != static_cast<uint256_t>(len)) { return false; }
    // read values
    decoded = std::vector<T>(len);
    for (int i = 0; i < len; ++i) {
        if (!decode(encoded, decoded[i], offset_inout)) { return false; }
    }
    // padding
    offset_inout = origOffset + paddedTo32(offset_inout - origOffset);
    // TODO handle dynamic types
    return true;
}
*/

} // namespace TW::Ethereum::ABI
