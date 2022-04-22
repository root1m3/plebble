//===-                           P L E B B L E
//===-                         https://plebble.us
//===-
//===-              Copyright (C) 2017-2022 root1m3@plebble.us
//===-
//===-                      GNU GENERAL PUBLIC LICENSE
//===-                       Version 3, 29 June 2007
//===-
//===-    This program is free software: you can redistribute it and/or modify
//===-    it under the terms of the AGPLv3 License as published by the Free
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

#include <string>
#include <iostream>
#include <libxml++/libxml++.h>
#include <vector>
#include <fstream>
#include <sstream>
#include <cassert>

using namespace std;
using namespace xmlpp;

typedef xmlpp::Element dom_element;

struct item {
    string header;
    string body;
};

    struct items: vector<item*> {
        ~items() {
            for (auto&i:*this) delete i;
        }
    };

dom_element* root=0;


void print_fullpath(const dom_element& e) {
        vector<string> v;
        const dom_element* p=&e;
        while(true) {
            if (p==0) {
                break;
            }
            v.push_back(p->get_name());
            p=p->get_parent();
        }
        for (auto i=v.rbegin(); i!=v.rend(); ++i) {
            cout << *i << "/";
        }
        cout << endl;


}


dom_element* find_elem(dom_element* e, const item& itm) {
    cout << itm.header << endl;
    NodeSet v=e->find(".//compounddef");
    cout << v.size() << endl;
    for (auto &i:v) {
        dom_element* e=dynamic_cast<dom_element*>(i);
        if (e!=0) {
            string attr=e->get_attribute("kind")->get_value();
            cout << attr << " ";
            auto nl=e->get_children("compoundname");
            assert(nl.size()==1);
            dom_element* e2=dynamic_cast<dom_element*>(*nl.begin());
            cout << e2->get_child_text()->get_content() << endl;


            //print_fullpath(*e);
        }
    }
exit(0);
    //NodeList ch=e->get_children();
//    for (auto&i:ch) {

  //  }

    return 0;
}

int main(int argc, char **argv) {

    string file=argv[1];

    xmlpp::DomParser parser;
    parser.set_substitute_entities(); //We just want the text to be resolved/unescaped automatically
    parser.parse_file(file);
    if (parser) {

        root=dynamic_cast<dom_element*>(parser.get_document()->get_root_node());
//        parse(root);
    }
    else {
        cerr << "Unable to parse "  << endl;
        exit(1);
    }

    string docfile=argv[2];

    ifstream is(docfile);

    items v;
    bool in=false;
    item* itm=0;
    ostringstream *os=0;
    while(is.good()) {
        string line;
        getline(is,line);
        if (!in) {
            if (line=="->--------------------------------------------------------------") {
                in=true;
                assert(itm==0);
                itm=new item();
                os=new ostringstream();
                getline(is,line);
                itm->header=line;
                continue;
            }
        }
        if (in) {
            if (line=="-<--------------------------------------------------------------") {
                in=false;
                itm->body=os->str();
                v.push_back(itm);
                itm=0;
                delete os;
                continue;
            }
            *os << line << endl;
        }
    }

    cout << v.size() << endl;

    for (auto& i:v) {
        dom_element* e=find_elem(root,*i);
        if (e==0) {
            cerr << "NOT FOUND in XML: " << i->header << endl;
        }
    }


    (*parser.get_document()).write_to_file("output.xml");

}
