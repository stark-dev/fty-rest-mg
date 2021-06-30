/*
 *
 * Copyright (C) 2015 - 2020 Eaton
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 */

/*!
 * \file augtool.cc
 * \author Michal Hrusecky <MichalHrusecky@Eaton.com>
 * \brief Not yet documented file
 */
#include <vector>
#include <string>
#include <functional>
#include <fty/string-utils.h>
#include <mutex>
#include <thread>
#include <chrono>
#include <fty_log.h>

#include "shared/augtool.h"

//using namespace shared;

std::string augtool::get_cmd_out(std::string cmd, bool key_value,
                                 std::string sep,
                                 std::function<bool(std::string)> filter) {
    std::string in = get_cmd_out_raw(cmd);
    std::vector<std::string> spl = fty::split(in, "\n");
    bool not_first = false;
    std::string out;
    if(spl.size() >= 3) {
        spl.erase(spl.begin());
        spl.pop_back();
    } else {
        return out;
    }
    for(auto i : spl) {
        auto pos = i.find_first_of("=");
        if(pos == std::string::npos) {
            if(key_value)
                continue;
            if(not_first)
                out += sep;
            if(filter(i))
                continue;
            out += i;
        } else {
            if(not_first)
                out += sep;
            if(filter(i.substr(pos+2)))
                continue;
            out += i.substr(pos+2);
        }
        not_first = true;
    }
    return out;
}


std::string augtool::get_cmd_out_raw(std::string command) {
    std::string ret;
    bool err = false;

    std::lock_guard<std::mutex> lock(mux);

    if(command.empty() || command.back() != '\n')
        command += "\n";
    if(!prc->write(command))
        err = true;
    //std::this_thread::sleep_for(std::chrono::microseconds(1000));
    ret = prc->readAllStandardOutput();
    return err ? "" : ret;
}

void augtool::run_cmd(std::string cmd) {
    get_cmd_out_raw(cmd);
}

void augtool::clear() {
    static std::mutex clear_mux;
    std::lock_guard<std::mutex> lock(clear_mux);
    run_cmd("");
    run_cmd("load");
}

static std::mutex in_mux;

augtool* augtool::get_instance() {
    static augtool inst;
    std::string nil;

    // Initialization of augtool subprocess if needed
    std::lock_guard<std::mutex> lock(in_mux);

    if(inst.prc == NULL) {
        inst.prc = new fty::Process("sudo", {"augtool", "-S", "-I/usr/share/fty/lenses", "-e"});

        //sleep to ensure augeas is launched
        //std::this_thread::sleep_for(std::chrono::seconds(2));
    }
    if(!inst.prc->exists()) {
        if (inst.prc->run()) {
            //std::this_thread::sleep_for(std::chrono::seconds(1));
            nil = inst.get_cmd_out_raw("help");
            //log_debug("augtool: %s", nil.c_str());
            if(nil.find("match") == nil.npos) {
                //log_error("Cleanup wrong augtool");
                delete inst.prc;
                inst.prc = NULL;
                return NULL;
            }
        }
    }
    inst.clear();
    return &inst;
}
