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
#include <random>
#include <iostream>
#include <array>
using namespace std;

#define output_size 20

struct x: array<uint8_t, output_size> {
void fill_random(mt19937& rng) {
    size_t p = 0;
    while (true) {
cout << "write @ " << p << endl;
        *reinterpret_cast<mt19937::result_type*>(&(*this)[p]) = rng();
        p += sizeof(mt19937::result_type);
        if (p == output_size) break;
        if (p > output_size) {
            p = output_size - sizeof(mt19937::result_type);
cout << "write @ " << p << endl;
            *reinterpret_cast<mt19937::result_type*>(&(*this)[p]) = rng();
            break;
        }
    }
}
};

void main_rng() {
cout << sizeof(mt19937::result_type) << endl;
x a;
mt19937 rng;
a.fill_random(rng);
}

//hash_t (aka ripemd160::value_type)::fill_random(rng)

//int main() {
//    main_rng();
//}
