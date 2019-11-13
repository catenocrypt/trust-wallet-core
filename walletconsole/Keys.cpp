// Copyright © 2017-2019 Trust Wallet.
//
// This file is part of Trust. The full Trust copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.

#include "Keys.h"

#include "WalletConsole.h"
#include "Data.h"

#include <iostream>
#include <vector>
#include <cassert>
#include <stdlib.h>
#include <time.h>

namespace TW::WalletConsole {

using namespace std;

Keys::Keys() {
    // init pseudo-random
    ::srand(::time(NULL));
}

bool Keys::newkey(string& res) {
    int n = 32;
    Data k = Data(n);
    for (int i = 0; i < n; ++i) {
        k[i] = ::rand() % 256;
    }
    res = hex(k);
    return true;
}

} // namespace TW::WalletConsole
