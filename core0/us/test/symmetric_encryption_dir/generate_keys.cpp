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
#include "us/gov/crypto/symmetric_encryption.h"

#include <iostream>
#include <string>
#include "us/gov/crypto/ec.h"

using namespace std;
using namespace us::gov::crypto;

int main ()
{
    ec::keys k = ec::keys::generate();
    std::cout << k.priv << " " << k.pub << endl;
    //std::cout << "4ACKBcXXhtGb3NtZzSfwgSs3GqCgCBY65juF2UVSJQR2 dPtUg631Hh7tL8t3wK6KHDwFfQzLmcHtkEAH5mSsANX3" << endl;
    //std::cout << "7RZ9FmWLCdfGZCzThU7DZKf2q8hyxtAzCXfjXEGVU8E1 R9s12KNiCKJG1ax5hYGoSHgLSMX3trHY26jmwbhUQFRQCgwR2gRtrAnJTR8NwmFkD4YMtHtXhe57KAd6nSjrYtMV" << endl;
    //std::cout << "9pKQDhcZsi9V1qVhaDDnqV7HyiatxUEwTiLjqtqD7ZR6 R9s12KNiCKJG1ax5hYGoSHgLSMX3trHY26jmwbhUQFRQCgwR2gRtrAnJTR8NwmFkD4YMtHtXhe57KAd6nSjrYtMV" << endl;
    //std::cout << "9pKQDhcZsi9V1qVhaDDnqV7HyiatxUEwTiLjqtqD7ZR6 or69BumA7ZALzHNKjuxDLtHithXo3BfzJ2VYg73uNizk" <<endl;
}