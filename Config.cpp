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
    unordered_map<string, string>::iterator iterator;
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

    iterator = configs.begin();
    int cpu_num;
    while (iterator != configs.end()) {
        if ((*iterator).first == "port") {
            try {
                port = stoi((*iterator).second);
            } catch (std::invalid_argument) {
                throw "Check Port Firstly";
            } catch (std::out_of_range) {
                throw "Port Too Large";
            }
        } else if ((*iterator).first == "process_num") {
            try {
                process_num = stoi((*iterator).second);
            } catch (std::invalid_argument) {
                throw "Invalid Process Num";
            } catch (std::out_of_range) {
                throw "Process Num Too Large";
            }
            //refer to https://stackoverflow.com/questions/150355/programmatically-find-the-number-of-cores-on-a-machine
#ifdef  linux
            cpu_num = sysconf(_SC_NPROCESSORS_ONLN);
            if (process_num > cpu_num) {
               process_num = cpu_num;
            }
#endif
#ifdef _WIN32
            SYSTEM_INFO sysinfo;
            GetSystemInfo(&sysinfo);
            cpu_num = sysinfo.dwNumberOfProcessors;
#endif
        } else if ((*iterator).first == "run_as_daemon") {
#ifdef linux
            if ((*iterator).second != "true" && (*iterator).second != "false") {
                throw "Config run_as_daemon Only Support true or false";
            }

            run_as_daemon = (*iterator).second == "true";
#else
            run_as_daemon = false;
#endif

        } else if ((*iterator).first == "debug_open") {
            if ((*iterator).second != "true" && (*iterator).second != "false") {
                throw "Config debug_open Only Support true or false";
            }
            debug_open = (*iterator).second == "true";
        }
        ++iterator;
    }
    if (debug_open) {
        cout << "---------------app config as follows-------------" << endl;
        cout << "WebSocket Server Port:" << port << endl;
        cout << "Worker Process Num:" << process_num << endl;
        cout << "Debug Mode:" << (debug_open ? "open" : "close") << endl;
        cout << "Run As Daemon:" << (run_as_daemon ? "true" : "false") << endl;
        cout << "-------------------------------------------------" << endl;
    }
}


void Config::Print() {
    auto ib = configs.begin();
    auto ie = configs.end();
    for (; ib != ie; ++ib) {
        cout << (*ib).first << ":" << (*ib).second << endl;
    }
}