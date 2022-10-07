//===-                           P L E B B L E
//===-                         https://plebble.us
//===-
//===-              Copyright (C) 2017-2022 root1m3@plebble.us
//===-
//===-                      GNU GENERAL PUBLIC LICENSE
//===-                       Version 3, 29 June 2007
//===-
//===-    This program is free software: you can redistribute it and/or modify
//===-    it under the terms of the GPLv3 License as published by the Free
//===-    Software Foundation.
//===-
//===-    This program is distributed in the hope that it will be useful,
//===-    but WITHOUT ANY WARRANTY; without even the implied warranty of
//===-    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//===-
//===-    You should have received a copy of the General Public License
//===-    along with this program, see LICENCE file.
//===-    see https://www.gnu.org/licenses
//===-
//===----------------------------------------------------------------------------
//===-
#include <iostream>
#include <fstream>

#include <us/gov/config.h>
#include <us/gov/engine/db_analyst.h>

//dot -Tpng error_expected.dot  > error_expected.png

#define loglevel "test"
#define logclass "db_analyst"
#include "logs.inc"

#include "assert.inc"

namespace us { namespace test {

using namespace us::gov::engine;
using namespace std;

void test_db_analyst_set(const string& set) {
//cout << "------------------ set " << set << endl;
    db_analyst::chains_t a;
    vector<string> f = a.files(set);
//    random_shuffle(f.begin(),f.end());
    {
        std::random_device rd;
        std::mt19937 g(rd());
        shuffle(f.begin(), f.end(), g);
    }
//for (auto i:f) cout << i << endl;
    a.load(set, f);
    ostringstream os;
    a.dot(os);
//cout << os.str() << endl;
    ifstream is(set+"_expected");
    ostringstream ose;
    while(is.good()) {
        string line;
        getline(is,line);
        if (line.empty()) continue;
        ose << line << endl;
    }

    if (os.str()!=ose.str()) {
        cerr << os.str() << endl;
        cerr << endl << "differs from:" << endl;
        cerr << ose.str() << endl;
        {
        ofstream e("error_got.dot");
        e << os.str() << endl;
        }
        {
        ofstream e("error_expected.dot");
        e << ose.str() << endl;
        }
        cerr << "set: " << set << endl;
        assert(false);
    }
}


int test_db_analyst(string sets) {
    srand(time(0));

    for (int i=0; i<10; ++i) {
        test_db_analyst_set(sets+"/set1");
        test_db_analyst_set(sets+"/set2");
        test_db_analyst_set(sets+"/set3");
        test_db_analyst_set(sets+"/set4");
        test_db_analyst_set(sets+"/set5");
        test_db_analyst_set(sets+"/set6");
        test_db_analyst_set(sets+"/set7");
        test_db_analyst_set(sets+"/set8");
        test_db_analyst_set(sets+"/set9");
        test_db_analyst_set(sets+"/set10");
    }


    return 0;
}

}}


