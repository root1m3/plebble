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
#include "evidence.h"
#include "app.h"
#include "calendar.h"

#include "dbsync.h"

#define loglevel "engine"
#define logclass "calendar"
#include "logs.inc"

using namespace std;
using namespace us::gov::engine;
using c = us::gov::engine::dbsync;

void c::seeds_t::add_sample(seeds_t&& sample) {
}

c::seeds_t c::seeds_t::get_sample() const {
    seeds_t seeds;
    return seeds;
}
        
void c::process(peer_t* peer) {
    {
        seeds_t peer_sample;
        seeds_t my_sample = seeds.get_sample();
        //peer->call_seeds_sample(my_sample, peer_sample);
        seeds.add_sample(move(peer_sample));
    }
    
}

