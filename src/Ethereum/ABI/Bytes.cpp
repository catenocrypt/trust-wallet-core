// Copyright © 2017-2019 Trust Wallet.
//
// This file is part of Trust. The full Trust copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.

#include "Bytes.h"
#include "ParamNumber.h"
#include "Util.h"

namespace TW::Ethereum::ABI {

void ParamBytes::encodeBytes(const Data& bytes, Data& data) {
    TW::Ethereum::ABI::encode(uint256_t(bytes.size()), data);

    const auto count = bytes.size();
    const auto padding = Util::padNeeded32(count);
    data.insert(data.end(), bytes.begin(), bytes.begin() + count);
    append(data, Data(padding));
}

bool ParamBytes::decodeBytes(const Data& encoded, Data& decoded, size_t& offset_inout)
{
    size_t origOffset = offset_inout;
    // read len
    uint256_t len256;
    if (!TW::Ethereum::ABI::decode(encoded, len256, offset_inout)) { return false; }
    // check if length is in the size_t range
    size_t len = static_cast<size_t>(len256);
    if (len256 != static_cast<uint256_t>(len)) { return false; }
    // check if there is enough data
    if (encoded.size() < offset_inout + len) { return false; }
    // read data
    decoded = Data(encoded.begin() + offset_inout, encoded.begin() + offset_inout + len);
    offset_inout += len;
    // padding
    offset_inout = origOffset + Util::paddedTo32(offset_inout - origOffset);
    return true;
}

void ParamBytesFix::encode(Data& data) const {
    const auto count = _bytes.size();
    const auto padding = Util::padNeeded32(count);
    data.insert(data.end(), _bytes.begin(), _bytes.begin() + count);
    append(data, Data(padding));
}

bool ParamBytesFix::decodeBytesFix(const Data& encoded, size_t n, Data& decoded, size_t& offset_inout) {
    size_t origOffset = offset_inout;
    if (encoded.size() < offset_inout + n) {
        // not enough data
        return false;
    }
    if (decoded.size() < n) append(decoded, Data(n - decoded.size()));
    std::copy(encoded.begin() + offset_inout, encoded.begin() + (offset_inout + n), decoded.begin());
    offset_inout += n;
    // padding
    offset_inout = origOffset + Util::paddedTo32(offset_inout - origOffset);
    return true;
}

void ParamString::encodeString(const std::string decoded, Data& data) {
    auto bytes = Data(decoded.begin(), decoded.end());
    ParamBytes::encodeBytes(bytes, data);
}

bool ParamString::decodeString(const Data& encoded, std::string& decoded, size_t& offset_inout) {
    Data decodedData;
    if (!ParamBytes::decodeBytes(encoded, decodedData, offset_inout)) { return false; }
    decoded = std::string(decodedData.begin(), decodedData.end());
    return true;
}





/*
void encode(const Data& bytes, Data& data) {
    encode(uint256_t(bytes.size()), data);

    const auto count = bytes.size();
    const auto padding = padNeeded32(count);
    data.insert(data.end(), bytes.begin(), bytes.begin() + count);
    append(data, Data(padding));
}

bool decode(const Data& encoded, Data& decoded, size_t& offset_inout) {
    size_t origOffset = offset_inout;
    // read len
    uint256_t len256;
    if (!decode(encoded, len256, offset_inout)) { return false; }
    // check if length is in the size_t range
    size_t len = static_cast<size_t>(len256);
    if (len256 != static_cast<uint256_t>(len)) { return false; }
    // check if there is enough data
    if (encoded.size() < offset_inout + len) { return false; }
    // read data
    decoded = Data(encoded.begin() + offset_inout, encoded.begin() + offset_inout + len);
    offset_inout += len;
    // padding
    offset_inout = origOffset + paddedTo32(offset_inout - origOffset);
    return true;
}

bool decode(const Data& encoded, std::string& decoded, size_t& offset_inout) {
    Data decodedData;
    if (!decode(encoded, decodedData, offset_inout)) { return false; }
    decoded = std::string(decodedData.begin(), decodedData.end());
    return true;
}
*/

} // namespace TW::Ethereum::ABI
