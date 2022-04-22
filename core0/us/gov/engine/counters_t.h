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
#pragma once
#include <us/gov/config.h>

#if CFG_COUNTERS == 1
#include <iostream>

namespace us { namespace gov { namespace engine {

   struct counters_t final {
        uint32_t cycles{0};
        uint32_t computed_right_delta{0};
        uint32_t diff_applied{0};
        uint32_t genesis_diff_applied{0};
        uint32_t diff_discarded_not_in_sequence{0};
        uint32_t clear_db{0};
        uint32_t patch_db{0};
        uint32_t problem_opening_block_file{0};
        uint32_t problem_reading_block_file{0};
        uint32_t problem_importing_diff_file{0};
        uint32_t problem_loading_base_file{0};
        uint32_t patch_db_ok{0};
        uint32_t load_db{0};
        uint32_t problem_opening_db{0};
        uint32_t problem_reading_db{0};
        uint32_t snapshot_create{0};
        uint32_t error_renaming_db_file{0};
        uint32_t relay_local_deltas{0};
        uint32_t created_empty_local_deltas{0};
        uint32_t created_nonempty_local_deltas{0};
        uint32_t sent_vote{0};
        uint32_t recv_vote{0};
        uint32_t recv_vote_wrong_signature{0};
        uint32_t relay_vote_success{0};
        uint32_t relay_vote_fail{0};
        uint32_t recv_dup_vote{0};
        uint32_t recv_local_deltas_accepted{0};
        uint32_t recv_local_deltas_unparseable{0};
        uint32_t recv_local_deltas_wrong_signature{0};
        uint32_t recv_local_deltas_duplicate{0};
        uint32_t lost_evidences{0};
        uint32_t processed_evidences{0};
        uint32_t processed_evidences_last_block{0};
        uint32_t processed_evidences_cur_block{0};
        void dump(std::ostream&) const;
    };

}}}

#endif

