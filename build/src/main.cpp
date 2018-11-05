/******************************************************************************
 *    Copyright (c) Open Connectivity Foundation (OCF), AllJoyn Open Source
 *    Project (AJOSP) Contributors and others.
 *
 *    SPDX-License-Identifier: Apache-2.0
 *
 *    All rights reserved. This program and the accompanying materials are
 *    made available under the terms of the Apache License, Version 2.0
 *    which accompanies this distribution, and is available at
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 *    Copyright (c) Open Connectivity Foundation and Contributors to AllSeen
 *    Alliance. All rights reserved.
 *
 *    Copyright (c) V2 Systems, LLC.  All rights reserved.
 *
 *    Permission to use, copy, modify, and/or distribute this software for
 *    any purpose with or without fee is hereby granted, provided that the
 *    above copyright notice and this permission notice appear in all
 *    copies.
 *
 *    THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL
 *    WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED
 *    WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE
 *    AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL
 *    DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
 *    PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 *    TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 *    PERFORMANCE OF THIS SOFTWARE.
******************************************************************************/

// INCLUDE
#include <iostream>     // cout, cin
#include <thread>       // thread, join
#include <chrono>       // now, duration
#include <map>
#include <string>
#include <vector>

#include "include/tsu.h"
#include "include/DistributedEnergyResource.hpp"
#include "include/Modbus.h"

using namespace std;

bool done = false;  //signals program to stop

// Help
// - CLI interface description
static void Help () {
    printf ("\n\t[Help Menu]\n\n");
    printf ("> q            quit\n");
    printf ("> h            display help menu\n");
    printf ("> i <watts>    import power\n");
    printf ("> e <watts>    export power\n");
    printf ("> p            print properties\n");
} // end Help


// Command Line Interface
// - method to allow user controls during program run-time
static bool CommandLineInterface (const string& input, 
                                  DistributedEnergyResource *DER) {
    // check for program argument
    if (input == "") {
        return false;
    }
    char cmd = input[0];

    // deliminate input string to argument parameters
    vector <string> tokens;
    stringstream ss(input);
    string token;
    while (ss >> token) {
        tokens.push_back(token);
    }

    switch (cmd) {
        case 'q':
           return true;

        case 'i': {
            try {
                DER->SetImportWatts(stoul(tokens[1]));
            } catch(...) {
                cout << "[ERROR]: Invalid Argument." << endl;
            }
            break;
        }

        case 'e': {
            try {
                DER->SetExportWatts(stoul(tokens[1]));
            } catch(...) {
                cout << "[ERROR]: Invalid Argument." << endl;
            }
            break;
        }

        case 'p': {
            cout << "\n\t[Properties]\n"
                << "\nExport Energy:\t" << DER->GetExportEnergy ()
                << "\nExport Power:\t" << DER->GetExportPower ()
                << "\nImport Energy:\t" << DER->GetImportEnergy ()
                << "\nImport Power:\t" << DER->GetImportPower () << endl;
            break;
        }

        default: {
            Help();
            break;
        }
    }

    return false;
}  // end Command Line Interface

void ResourceLoop (DistributedEnergyResource *DER, unsigned int time_wait) {
    unsigned int time_remaining, time_past;
    auto time_start = chrono::high_resolution_clock::now();
    auto time_end = chrono::high_resolution_clock::now();
    chrono::duration<double, milli> time_elapsed;

    while (!done) {
        time_start = chrono::high_resolution_clock::now();
            // time since last control call;
            time_elapsed = time_start - time_end;
            time_past = time_elapsed.count();
            DER->Loop(time_past);
        time_end = chrono::high_resolution_clock::now();
        time_elapsed = time_end - time_start;

        // determine sleep duration after deducting process time
        time_remaining = (time_wait - time_elapsed.count());
        time_remaining = (time_remaining > 0) ? time_remaining : 0;
        this_thread::sleep_for (chrono::milliseconds (time_remaining));
    }
}  // end Resource Loop

// Main
// ----
int main (int argc, char** argv) {
    cout << "\nStarting Program...\n";
    cout << "\n\tLoading parameters...\n";

    if (argc == 1) {
        // this means no arguments were given
        string name = argv[0];
        tsu::CommandLineHelp(name);
        return EXIT_FAILURE;
    }
    tsu::string_map parameters = tsu::CommandLineParse(argc, argv);

    cout << "\n\tMapping configuration file...\n";
    string config_file = parameters.at("config");
    tsu::config_map ini_map = tsu::MapConfigFile(config_file);

    cout << "\n\t\tLooking for resource...\n";

    DistributedEnergyResource *der_ptr = 
        new DistributedEnergyResource (ini_map["DER"]);

    Modbus bms(ini_map["BMS"]);
    Modbus inverter(ini_map["Inverter"]);

    cout << "\n\t\tReadModel Test: \n";
    tsu::string_map points = bms.ReadModel (99001);
    for (auto& point : points) {
        cout << point.first << "\t" << point.second << "\n";
    }
    
    cout << "\n\t\tWriteModel Test: \n";
    bms.WriteModel (99001, points);

    cout << "\nProgram initialization complete...\n";
    thread DER(ResourceLoop, der_ptr, stoul(ini_map["DER"]["ThreadPeriod"]));

    // CLI Loop
    Help ();
    string input;
    while (!done) {
        getline(cin, input);
        done = CommandLineInterface(input, der_ptr);
    }

    cout << "\nProgram shutting down...\n";
    cout << "\n\t Joining threads...\n";
    DER.join();

    cout << "\n\t deleting pointers...\n";
    delete der_ptr;

    return EXIT_SUCCESS;
} // end main
