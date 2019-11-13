// Copyright © 2017-2019 Trust Wallet.
//
// This file is part of Trust. The full Trust copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.

#include "WalletConsole.h"
#include "Keys.h"

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

void CommandExecutor::help() {
    cout << "Commands:" << endl;
    cout << "  exit                Exit" << endl;
    cout << "  quit                Exit" << endl;
    cout << "  help                This help" << endl;
    cout << "Inputs, buffer:" << endl;
    cout << "  #<n>                Print nth previous result" << endl;
    cout << "  #                   Print last result" << endl;
    cout << "Keys:" << endl;
    cout << "  newkey              Create new pseudo-random 32-byte key" << endl;
    cout << "Transformations:" << endl;
    cout << "  hex <inp>           Encode given string to hex" << endl;
    cout << "  base64enc <inp>     Encode given hex data to Base64" << endl;
    cout << "  base64dec <inp>     Encode given Base64 string to hex data" << endl;
}

bool CommandExecutor::executeOne(const string& cmd, const vector<string>& params_in, string& res) {
    if (cmd == "exit" || cmd == "quit") {
        // handled in loop
        cout << "Bye!" << endl;
        return false;
    }

    // prepare inputs
    vector<string> params;
    if (!prepareInputs(params_in, params)) {
        cout << "Error processing input(s)" << endl;
        return false;
    }
    
    if (cmd == "help") { help(); return false; }
    if (cmd[0] == '#') {
        // echo input, with substitution
        cout << "Last result is:  " << params[0] << endl;
        return false;
    }
    if (cmd == "hex") { if (checkMinParams(params, 1)) { return Transformer::hex(params[1], res); } }
    if (cmd == "base64enc") { if (checkMinParams(params, 1)) { return Transformer::base64enc(params[1], res); } }
    if (cmd == "base64dec") { if (checkMinParams(params, 1)) { return Transformer::base64dec(params[1], res); } }
    if (cmd == "newkey") { return _keys.newkey(res); }
    // fallback
    help();
    return false;
}

void CommandExecutor::execute(const string& cmd, const vector<string>& params) {
    try {
        string resultStr;
        bool res = executeOne(cmd, params, resultStr);
        if (res && resultStr.length() > 0) {
            // there is a new result
            cout << "Result:  " << resultStr << endl;
            _buffer.addResult(resultStr);
        }
    } catch (exception& ex) {
        cout << "Error while executing command" << endl;
    }
}

bool CommandExecutor::prepareInputs(const vector<string>& p_in, vector<string>& p_out) {
    p_out = vector<string>{};
    for (auto p: p_in) {
        string p2;
        if (!_buffer.prepareInput(p, p2)) {
            return false;
        }
        p_out.push_back(p2);
    }
    assert(p_out.size() == p_in.size());
    return true;
}

bool CommandExecutor::checkMinParams(const vector<string>& params, int n) {
    if (params.size() >= n) {
        return true;
    }
    cout << "At least " << n << " parameters are needed!" << endl;
    help();
    return false;
}

void WalletConsole::loop() {
    while (true) {
        cout << "> ";
        string line;
        getline(cin, line);
        //cout << "Read line: '" << line << "'" << endl;
        trimLeft(line);
        if (line.length() == 0) { continue; }
        vector<string> params;
        auto cmd = parseLine(line, params);
        //cout << "Read cmd: '" << cmd << "'" << endl;
        if (isExit(cmd)) {
            break;
        }

        _executor.execute(cmd, params);
    }
    cout << "Bye!" << endl << endl;
}

bool WalletConsole::isExit(const string& cmd) {
    if (cmd == "quit" || cmd == "exit") { return true; }
    return false;
}

vector<string> WalletConsole::tokenize(const string& line) {
    vector<string> tok;
    size_t n = line.length();
    size_t idx = 0;
    while (true) {
        auto spaceIdx = line.find(' ', idx);
        if (spaceIdx == string::npos || spaceIdx >= n) {
            // no more space
            if (idx < n) {
                tok.push_back(line.substr(idx, n - idx));
            }
            return tok;
        }
        // there is a space, take current word up to it
        if (idx < spaceIdx) {
            tok.push_back(line.substr(idx, spaceIdx - idx));
        }
        idx = spaceIdx + 1;
    }
}

string WalletConsole::parseLine(const string& line, vector<string>& params) {
    auto tokens = tokenize(line);
    assert(tokens.size() > 0);
    auto cmd = tokens[0];
    params = tokens;
    return cmd;
}

// trim from start (in place)
void WalletConsole::trimLeft(std::string &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int ch) {
        return !std::isspace(ch);
    }));
}

} // namespace TW::WalletConsole
