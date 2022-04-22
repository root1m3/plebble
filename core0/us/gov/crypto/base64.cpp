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
/*
base64.cc - c++ source to a base64 reference encoder and decoder

This is part of the libb64 project, and has been placed in the public domain.
For details, see http://sourceforge.net/projects/libb64

cencoder.c - c source to a base64 encoding algorithm implementation

This is part of the libb64 project, and has been placed in the public domain.
For details, see http://sourceforge.net/projects/libb64
*/
#include "base64.h"
#include <stdlib.h>
#include <sstream>

using namespace std;

namespace {

    typedef enum {
        step_A, step_B, step_C
    } base64_encodestep;

    typedef struct {
        base64_encodestep step;
        char result;
    //    int stepcount;
    } base64_encodestate;

    void base64_init_encodestate(base64_encodestate* state_in);
    char base64_encode_value(char value_in);
    int base64_encode_block(const char* plaintext_in, int length_in, char* code_out, base64_encodestate* state_in);
    int base64_encode_blockend(char* code_out, base64_encodestate* state_in);

    void base64_init_encodestate(base64_encodestate* state_in) {
        state_in->step = step_A;
        state_in->result = 0;
    }

    char base64_encode_value(char value_in) {
        static const char* encoding = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
        if (value_in > 63) return '=';
        return encoding[(int)value_in];
    }

    int base64_encode_block(const char* plaintext_in, int length_in, char* code_out, base64_encodestate* state_in) {
        const char* plainchar = plaintext_in;
        const char* const plaintextend = plaintext_in + length_in;
        char* codechar = code_out;
        char result;
        char fragment;

        result = state_in->result;

        switch (state_in->step)
        {
            while (1)
            {
        case step_A:
                if (plainchar == plaintextend)
                {
                    state_in->result = result;
                    state_in->step = step_A;
                    return codechar - code_out;
                }
                fragment = *plainchar++;
                result = (fragment & 0x0fc) >> 2;
                *codechar++ = base64_encode_value(result);
                result = (fragment & 0x003) << 4;
        case step_B:
                if (plainchar == plaintextend)
                {
                    state_in->result = result;
                    state_in->step = step_B;
                    return codechar - code_out;
                }
                fragment = *plainchar++;
                result |= (fragment & 0x0f0) >> 4;
                *codechar++ = base64_encode_value(result);
                result = (fragment & 0x00f) << 2;
        case step_C:
                if (plainchar == plaintextend)
                {
                    state_in->result = result;
                    state_in->step = step_C;
                    return codechar - code_out;
                }
                fragment = *plainchar++;
                result |= (fragment & 0x0c0) >> 6;
                *codechar++ = base64_encode_value(result);
                result  = (fragment & 0x03f) >> 0;
                *codechar++ = base64_encode_value(result);
            }
        }
        // control should not reach here
        return codechar - code_out;
    }

    int base64_encode_blockend(char* code_out, base64_encodestate* state_in) {
        char* codechar = code_out;

        switch (state_in->step)
        {
        case step_B:
            *codechar++ = base64_encode_value(state_in->result);
            *codechar++ = '=';
            *codechar++ = '=';
            break;
        case step_C:
            *codechar++ = base64_encode_value(state_in->result);
            *codechar++ = '=';
            break;
        case step_A:
            break;
        }
        return codechar - code_out;
    }

    typedef enum {
        step_a, step_b, step_c, step_d
    } base64_decodestep;

    typedef struct {
        base64_decodestep step;
        char plainchar;
    } base64_decodestate;

    void base64_init_decodestate(base64_decodestate* state_in);
    int base64_decode_value(char value_in);
    int base64_decode_block(const char* code_in, const int length_in, char* plaintext_out, base64_decodestate* state_in);

    int base64_decode_value(char value_in) {
        static const signed char decoding[] = {62,-1,-1,-1,63,52,53,54,55,56,57,58,59,60,61,-1,-1,-1,-2,-1,-1,-1,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,-1,-1,-1,-1,-1,-1,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51};
        static const char decoding_size = sizeof(decoding);
        if (value_in < 43) return -1;
        value_in -= 43;
        if (value_in >= decoding_size) return -1;
        return decoding[(int)value_in];
    }

    void base64_init_decodestate(base64_decodestate* state_in) {
        state_in->step = step_a;
        state_in->plainchar = 0;
    }

    int base64_decode_block(const char* code_in, const int length_in, char* plaintext_out, base64_decodestate* state_in) {
        const char* codechar = code_in;
        char* plainchar = plaintext_out;
        int fragment;

        *plainchar = state_in->plainchar;

        switch (state_in->step)
        {
            while (1)
            {
        case step_a:
                do {
                    if (codechar == code_in+length_in)
                    {
                        state_in->step = step_a;
                        state_in->plainchar = *plainchar;
                        return plainchar - plaintext_out;
                    }
                    fragment = base64_decode_value(*codechar++);
                } while (fragment < 0);
                *plainchar    = (fragment & 0x03f) << 2;
        case step_b:
                do {
                    if (codechar == code_in+length_in)
                    {
                        state_in->step = step_b;
                        state_in->plainchar = *plainchar;
                        return plainchar - plaintext_out;
                    }
                    fragment = base64_decode_value(*codechar++);
                } while (fragment < 0);
                *plainchar++ |= (fragment & 0x030) >> 4;
                *plainchar    = (fragment & 0x00f) << 4;
        case step_c:
                do {
                    if (codechar == code_in+length_in)
                    {
                        state_in->step = step_c;
                        state_in->plainchar = *plainchar;
                        return plainchar - plaintext_out;
                    }
                    fragment = base64_decode_value(*codechar++);
                } while (fragment < 0);
                *plainchar++ |= (fragment & 0x03c) >> 2;
                *plainchar    = (fragment & 0x003) << 6;
        case step_d:
                do {
                    if (codechar == code_in+length_in)
                    {
                        state_in->step = step_d;
                        state_in->plainchar = *plainchar;
                        return plainchar - plaintext_out;
                    }
                    fragment = base64_decode_value(*codechar++);
                } while (fragment < 0);
                *plainchar++   |= (fragment & 0x03f);
            }
        }
        // control should not reach here
        return plainchar - plaintext_out;
    }

}

string us::gov::crypto::b64::encode_string(const string& s) {
    base64_encodestate _state;
    base64_init_encodestate(&_state);
    char* code = new char[s.size()*2];
    int codelength;
    ostringstream os;
    codelength = base64_encode_block(s.data(), s.size(), code, &_state);
    os.write(code, codelength);
    codelength = base64_encode_blockend(code, &_state);
    os.write(code, codelength);
    delete [] code;
    return os.str();
}

string us::gov::crypto::b64::decode_string(const string& s) {
    base64_decodestate _state;
    base64_init_decodestate(&_state);
    char* code = new char[s.size()];
    int codelength;
    ostringstream os;
    codelength = base64_decode_block(s.data(), s.size(), code, &_state);
    os.write(code, codelength);
    delete [] code;
    return os.str();
}

