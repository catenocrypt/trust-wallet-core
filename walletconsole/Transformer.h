// Copyright © 2017-2019 Trust Wallet.
//
// This file is part of Trust. The full Trust copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.

#pragma once

#include <string>

namespace TW::WalletConsole {

using namespace std;

class Transformer {
public:
    static bool hex(const string& p, string& res);
    static bool base64enc(const string& p, string& res);
    static bool base64dec(const string& p, string& res);
    static bool filew(const string& filename, const string& data, string& res);
};

} // namespace TW::WalletConsole
