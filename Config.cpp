//
// Created by Administrator on 2019/10/8 0008.
//

#include "Config.h"
#include <fstream>

using namespace std;

void Config::Parse(string file) {
    if (file.length() <= 0 && config_filename.length() <= 0) {
        throw "Please Provide Invalid Config Filename";
    }
    if (file.length() > 0) {
        config_filename = file;
    }

    ifstream stream;
    stream.open(config_filename, ios::in);
    if (!stream) {
        throw "Open  Config failed";
    } else {
        string s;
        int pos;
        int line = 0;

        while (true) {
            ++line;
            s.clear();
            getline(stream, s);
            if (s.length() > 0) {
                if (s[0] == '#') {
                    continue;
                } else {
                    pos = s.find_first_of('=', 0);
                    if (pos == string::npos) {
                        break;
                    } else {
                        configs[s.substr(0, pos)] = s.substr(pos + 1);//skip char '='
                    }
                }
            } else {
                break;
            }
        }
    }
}


void Config::Print() {
    auto ib = configs.begin();
    auto ie = configs.end();
    for (; ib != ie; ++ib) {
        cout << (*ib).first << ":" << (*ib).second << endl;
    }
}