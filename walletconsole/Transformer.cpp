// Copyright © 2017-2019 Trust Wallet.
//
// This file is part of Trust. The full Trust copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.

#include "Transformer.h"

#include "Base64.h"
#include "HexCoding.h"
#include "Data.h"

#include <iostream>
#include <vector>
#include <cassert>

namespace TW::WalletConsole {

using namespace std;
using namespace TW;

bool Transformer::hex(const string& p, string& res) {
    res = TW::hex(data(p));
    //cout << "Hex result:  '" << res << "'" << endl;
    return true;
}

bool Transformer::base64enc(const string& p, string& res) {
    try {
        Data data = parse_hex(p);
        try {
            res = Base64::encode(data);
            //cout << "Base64 encode result:  '" << res << "'" << endl;
            return true;
        } catch (exception& ex) {
            cout << "Error while Base64 encode" << endl;
            return false;
        }
    } catch (exception& ex) {
        cout << "Error decoding input as hex" << endl;
        return false;
    }
}

bool Transformer::base64dec(const string& p, string& res) {
    try {
        auto dec = Base64::decode(p);
        res = TW::hex(dec);
        //cout << "Base64 decode result:  '" << res << "'" << endl;
        return true;
    } catch (exception& ex) {
        cout << "Error while Base64 decode" << endl;
        return false;
    }
}

} // namespace TW::WalletConsole
