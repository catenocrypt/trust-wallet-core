// Copyright © 2017-2019 Trust Wallet.
//
// This file is part of Trust. The full Trust copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.

#pragma once

#include "FunctionNG.h"
#include "Tuple.h"
#include "Numbers.h"

#include "../../Hash.h"

#include <string>
#include <tuple>

namespace TW::Ethereum {

// TODO move some impl to CPP file

/// Non-generic version of Function, templated version is impossible to pass around to and back over C interface
/// (void* looses the temaplate parameters).
class FunctionNG {
public:
    class ParamBase
    {
    public:
        virtual ~ParamBase() = default;
        virtual std::string getType() = 0;
    };

    template<typename T> 
    class ParamGen : public ParamBase
    {
    public:
        ParamGen() = default;
        ParamGen(T val) { _val = val; }
        void setVal(T val) { _val = val; }
        T getVal() const { return _val; }
        virtual std::string getType() { return type_string(_val); }
    private:
        T _val;
    };

    class ParamByte : public ParamGen<uint8_t>
    {
    public:
        ParamByte() : ParamGen<uint8_t>(0) {}
        ParamByte(uint8_t val) : ParamGen<uint8_t>(val) {}
    };

    class ParamUInt256 : public ParamGen<uint256_t>
    {
    public:
        ParamUInt256() : ParamGen<uint256_t>(uint256_t(0)) {}
        ParamUInt256(uint256_t val) : ParamGen<uint256_t>(val) {}
    };

    std::string name;
    std::vector<ParamBase*> _inParams;
    std::vector<ParamBase*> _outParams;

    FunctionNG(std::string name) : name(std::move(name)) {}
    ~FunctionNG() {
        for (auto p: _inParams) {
            if (p != nullptr) {
                delete p;
            }
        }
        _inParams.clear();
        for (auto p: _outParams) {
            if (p != nullptr) {
                delete p;
            }
        }
        _outParams.clear();
    }

    /// Returns the index of the parameter
    int AddInParam(ParamBase* param)
    {
        _inParams.push_back(param);
        return static_cast<int>(_inParams.size() - 1);
    }

    /// Returns the index of the parameter
    int AddOutParam(ParamBase* param)
    {
        _outParams.push_back(param);
        return static_cast<int>(_outParams.size() - 1);
    }

    bool GetInParam(int paramIndex, ParamBase** param_out)
    {
        if (paramIndex >= _inParams.size() || paramIndex < 0) {
            return false;
        }
        *param_out = _inParams[paramIndex];
        return true;
    }

    bool GetOutParam(int paramIndex, ParamBase** param_out)
    {
        if (paramIndex >= _outParams.size() || paramIndex < 0) {
            return false;
        }
        *param_out = _outParams[paramIndex];
        return true;
    }
};

} // namespace TW::Ethereum
