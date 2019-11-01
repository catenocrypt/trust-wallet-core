// Copyright © 2017-2019 Trust Wallet.
//
// This file is part of Trust. The full Trust copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.

#pragma once

#include "ParamBase.h"
#include "Util.h"
#include "../../Data.h"

#include <array>



// TODO remove
#include "../../uint256.h"
#include "Numbers.h"

namespace TW::Ethereum::ABI {

// TODO move some impl to CPP
class ParamBytes: public ParamCollection
{
private:
    Data _bytes;
public:
    ParamBytes() = default;
    ParamBytes(Data val) : ParamCollection() { setVal(val); }
    void setVal(Data val) { _bytes = val; }
    const Data& getVal() const { return _bytes; }
    virtual std::string getType() const { return "bytes"; };
    virtual size_t getSize() const { return 32 + Util::paddedTo32(_bytes.size()); }
    virtual bool isDynamic() const { return true; }
    virtual size_t getCount() const { return _bytes.size(); }
    static void encodeBytes(const Data& bytes, Data& data);
    virtual void encode(Data& data) const { encodeBytes(_bytes, data); }
    static bool decodeBytes(const Data& encoded, Data& decoded, size_t& offset_inout);
    virtual bool decode(const Data& encoded, size_t& offset_inout) {
        return decodeBytes(encoded, _bytes, offset_inout);
    }
};

class ParamBytesFix: public ParamCollection
{
private:
    size_t _n;
    Data _bytes;
public:
    ParamBytesFix(size_t n): ParamCollection(), _n(n), _bytes(Data(_n)) {}
    ParamBytesFix(size_t n, Data val): ParamCollection(), _n(n), _bytes(Data(_n)) { setVal(val); }
    void setVal(Data val) { _bytes = val; }
    const std::vector<uint8_t>& getVal() const { return _bytes; }
    virtual std::string getType() const { return "bytes" + std::to_string(_n); };
    virtual size_t getSize() const { return Util::paddedTo32(_bytes.size()); }
    virtual bool isDynamic() const { return false; }
    virtual size_t getCount() const { return _bytes.size(); }
    virtual void encode(Data& data) const;
    static bool decodeBytesFix(const Data& encoded, size_t n, Data& decoded, size_t& offset_inout);
    virtual bool decode(const Data& encoded, size_t& offset_inout) {
        return decodeBytesFix(encoded, _n, _bytes, offset_inout);
    }
};

class ParamString: public ParamCollection
{
private:
    std::string _str;
public:
    ParamString() = default;
    ParamString(std::string val): ParamCollection() { setVal(val); }
    void setVal(std::string& val) { _str = val; }
    const std::string& getVal() const { return _str; }
    virtual std::string getType() const { return "string"; };
    virtual size_t getSize() const { return 32 + Util::paddedTo32(_str.size()); }
    virtual bool isDynamic() const { return true; }
    virtual size_t getCount() const { return _str.size(); }
    static void encodeString(const std::string decoded, Data& data);
    virtual void encode(Data& data) const { encodeString(_str, data); }
    static bool decodeString(const Data& encoded, std::string& decoded, size_t& offset_inout);
    virtual bool decode(const Data& encoded, size_t& offset_inout) {
        return decodeString(encoded, _str, offset_inout);
    }
};







/*
/// Compute size when padded to 32-byte boundary; round up to multiple of 32.
inline size_t paddedTo32(size_t len) { return ((len + 31) / 32) * 32; }

/// Compute padding needed to next 32-byte boundary.
inline size_t padNeeded32(size_t len) { return ((len + 31) / 32) * 32 - len; }

// dynamic bytes (Data)

inline bool is_dynamic(Data) {
    return true;
}

inline std::size_t size(const Data& bytes) {
    return 32 + paddedTo32(bytes.size());
}

void encode(const Data& bytes, Data& data);

bool decode(const Data& encoded, Data& decoded, size_t& offset_inout);

inline std::string type_string(const Data& data) {
    return "bytes";
}

// static bytes

template <std::size_t N>
inline bool is_dynamic(std::array<byte, N>) {
    return false;
}

template <std::size_t N>
inline std::size_t size(const std::array<byte, N>& bytes) {
    return paddedTo32(bytes.size());
}

template <std::size_t N>
inline void encode(const std::array<byte, N>& bytes, Data& data) {
    const auto count = std::min(std::size_t(32), bytes.size());
    const auto padding = padNeeded32(count);
    data.insert(data.end(), bytes.begin(), bytes.begin() + count);
    append(data, Data(padding));
}

template <std::size_t N>
inline bool decode(const Data& encoded, std::array<byte, N>& decoded, size_t& offset_inout) {
    size_t origOffset = offset_inout;
    if (encoded.size() < offset_inout + N) {
        // not enough data
        return false;
    }
    std::copy(encoded.begin() + offset_inout, encoded.begin() + (offset_inout + N), decoded.begin());
    offset_inout += N;
    // padding
    offset_inout = origOffset + paddedTo32(offset_inout - origOffset);
    return true;
}

template <std::size_t N>
inline std::string type_string(const std::array<byte, N>& bytes) {
    return "bytes" + std::to_string(N);
}

// string

inline bool is_dynamic(std::string) {
    return true;
}

inline std::size_t size(const std::string& string) {
    return 32 + paddedTo32(string.size());
}

inline void encode(const std::string& string, Data& data) {
    auto bytes = Data(string.begin(), string.end());
    encode(bytes, data);
}

bool decode(const Data& encoded, std::string& decoded, size_t& offset_inout);

inline std::string type_string(const std::string& data) {
    return "string";
}
*/

} // namespace TW::Ethereum::ABI
