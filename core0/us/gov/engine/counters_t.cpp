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
#include "counters_t.h"
#if CFG_COUNTERS == 1

#define loglevel "gov/engine"
#define logclass "counters_t"
#include "logs.inc"

using c = us::gov::engine::counters_t;

void c::dump(std::ostream& os) const {
    os << "cycle\n";
    os << "  cycles " << cycles << '\n';
    os << "  computed_right_delta " << computed_right_delta << '\n';
    os << "  diff_applied " << diff_applied << '\n';
    os << "  genesis_diff_applied " << genesis_diff_applied << '\n';
    os << "  diff_discarded_not_in_sequence " << diff_discarded_not_in_sequence << '\n';
    os << "  clear_db " << clear_db << '\n';
    os << "Syncd\n";
    os << "  patch_db " << patch_db << '\n';
    os << "  problem_opening_block_file " << problem_opening_block_file << '\n';
    os << "  problem_reading_block_file " << problem_reading_block_file << '\n';
    os << "  problem_importing_diff_file " << problem_importing_diff_file << '\n';
    os << "  problem_loading_base_file " << problem_loading_base_file << '\n';
    os << "  patch_db_ok " << patch_db_ok << '\n';
    os << "  load_db " << load_db << '\n';
    os << "Protocol processor\n";
    os << "  problem_opening_db " << problem_opening_db << '\n';
    os << "  problem_reading_db " << problem_reading_db << '\n';
    os << "  snapshot_create " << snapshot_create << '\n';
    os << "  error_renaming_db_file " << error_renaming_db_file << '\n';
    os << "  relay_local_deltas " << relay_local_deltas << '\n';
    os << "  created_empty_local_deltas " << created_empty_local_deltas << '\n';
    os << "  created_nonempty_local_deltas " << created_nonempty_local_deltas << '\n';
    os << "  sent_vote " << sent_vote << '\n';
    os << "  recv_vote " << recv_vote << '\n';
    os << "  recv_vote_wrong_signature " << recv_vote_wrong_signature << '\n';
    os << "  relay_vote_success " << relay_vote_success << '\n';
    os << "  relay_vote_fail " << relay_vote_fail << '\n';
    os << "  recv_dup_vote " << recv_dup_vote << '\n';
    os << "  recv_local_deltas_accepted " << recv_local_deltas_accepted << '\n';
    os << "  recv_local_deltas_unparseable " << recv_local_deltas_unparseable << '\n';
    os << "  recv_local_deltas_wrong_signature " << recv_local_deltas_wrong_signature << '\n';
    os << "  recv_local_deltas_duplicate " << recv_local_deltas_duplicate << '\n';
    os << "Evidence processor\n";
    os << "  lost_evidences " << lost_evidences << '\n';
    os << "  processed_evidences " << processed_evidences << '\n';
    os << "  processed_evidences_last_block " << processed_evidences_last_block << '\n';
    os << "  processed_evidences_cur_block " << processed_evidences_cur_block << '\n';
}

#endif

