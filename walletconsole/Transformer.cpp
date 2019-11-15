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
#include <fstream>

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

bool Transformer::filew(const string& filename, const string& data, string& res) {
    if (filesystem::exists(filename)) {
        cout << "Warning: File '" << filename << "' already exists, not overwriting to be safe." << endl;
        return false;
    }
    try {
        ofstream outfile(filename,  std::ios::out | std::ios::binary);
        Data bindata = parse_hex(data);
        outfile.write((const char*)bindata.data(), bindata.size());
        outfile.close();
        cout << "Written to file '" << filename << "', " << bindata.size() << " bytes." << endl;
    } catch (exception& ex) {
        cout << "Error writing to file '" << filename << "': " << ex.what() << endl;
    }
    return false;
}

} // namespace TW::WalletConsole
