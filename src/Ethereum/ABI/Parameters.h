// Copyright © 2017-2019 Trust Wallet.
//
// This file is part of Trust. The full Trust copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.

#pragma once

#include "ParamBase.h"
#include "ParamNumber.h"

#include <vector>
#include <string>
#include <cassert>

namespace TW::Ethereum::ABI {

// TODO move some impl to CPP file

class ParamSet {
private:
    std::vector<ParamBase*> _params;

public:
    ParamSet() = default;
    ParamSet(ParamBase* param1) { addParam(param1); }
    ParamSet(std::vector<ParamBase*> params) { addParams(params); }
    ~ParamSet() {
        for (auto p: _params) {
            if (p != nullptr) {
                delete p;
            }
        }
        _params.clear();
    }

    /// Returns the index of the parameter
    int addParam(ParamBase* param)
    {
        if (param == nullptr) return -1;
        assert(param != nullptr);
        _params.push_back(param);
        return static_cast<int>(_params.size() - 1);
    }

    void addParams(std::vector<ParamBase*> params) {
        for (auto p: params) addParam(p);
    }

    bool getParam(int paramIndex, ParamBase** param_out) const {
        if (paramIndex >= _params.size() || paramIndex < 0) {
            return false;
        }
        *param_out = _params[paramIndex];
        return true;
    }

    ParamBase* getParamUnsafe(int paramIndex) const {
        if (paramIndex < 0) {
            return nullptr;
        }
        if (paramIndex >= _params.size()) {
            return _params[_params.size() - 1];
        }
        return _params[paramIndex];
    }

    size_t getCount() const { return _params.size(); }

    /// Return the function type signature, of the form "baz(int32,uint256)"
    std::string getType() const {
        std::string t = "(";
        int cnt = 0;
        for(auto p: _params) {
            if (cnt > 0) t += ",";
            t += p->getType();
            ++cnt;
        }
        t += ")";
        return t;
    }

    size_t getSize() const {
        size_t s = 0;
        for(auto p: _params) {
            s += p->getSize();
        }
        return 32 + Util::paddedTo32(s);
    }

    size_t getHeadSize() const {
        size_t s = 0;
        for(auto p: _params) {
            if (p->isDynamic()) {
                s += 32;
            } else {
                s += p->getSize();
            }
        }
        return s;
    }

    void encodeSmall(size_t headSize, size_t& offset, Data& data) const {
        for(auto p: _params) {
            if (p->isDynamic()) {
                // include only offset
                TW::Ethereum::ABI::encode(uint256_t(headSize + offset), data);
                offset += p->getSize();
            } else {
                p->encode(data);
            }
        }        
    }

    void encodeLarge(Data& data) const {
        for(auto p: _params) {
            if (p->isDynamic()) {
                p->encode(data);
            }
        }        
    }

    void encode(Data& data) const {
        // 2-pass encoding
        size_t headSize = getHeadSize();
        size_t dynamicOffset = 0;
        encodeSmall(headSize, dynamicOffset, data);
        encodeLarge(data);
    }

    bool decode(const Data& encoded, size_t& offset_inout) {
        // pass 1: small values
        for(auto p: _params) {
            if (p->isDynamic()) {
                uint256_t index;
                if (!TW::Ethereum::ABI::decode(encoded, index, offset_inout)) { return false; }
                // index is read but not used
            } else {
                if (!p->decode(encoded, offset_inout)) { return false; }
            }
        }
        // pass2: large values    
        for(auto p: _params) {
            if (p->isDynamic()) {
                if (!p->decode(encoded, offset_inout)) { return false; }
            }
        }
        return true;
    }
};

/// Collection of different parameters, dynamic length.
class Parameters: public ParamCollection
{
private:
    ParamSet _params;

public:
    Parameters() = default;
    Parameters(std::vector<ParamBase*> params) : ParamCollection(), _params(ParamSet(params)) {}
    void addParam(ParamBase* param) { _params.addParam(param); }
    void addParams(std::vector<ParamBase*> params) { _params.addParams(params); }
    ParamBase* getParam(int paramIndex) const { return _params.getParamUnsafe(paramIndex); }
    virtual std::string getType() const { return _params.getType(); }
    virtual size_t getSize() const { return _params.getSize(); }
    virtual bool isDynamic() const { return true; }
    virtual size_t getCount() const { return _params.getCount(); }
    virtual void encode(Data& data) const { _params.encode(data); }
    virtual bool decode(const Data& encoded, size_t& offset_inout) { return _params.decode(encoded, offset_inout); }
};

} // namespace TW::Ethereum::ABI
