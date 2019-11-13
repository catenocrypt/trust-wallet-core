// Copyright © 2017-2019 Trust Wallet.
//
// This file is part of Trust. The full Trust copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.

#include "WalletConsole.h"

#include <iostream>

using namespace std;

int main() {
    cout << "Wallet-core Console" << endl;

    TW::WalletConsole::WalletConsole console;
    console.loop();
}
