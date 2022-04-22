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
#include "screen.h"
#include <termios.h>
#include <unistd.h>
#include <string>

#define loglevel "io"
#define logclass "screen"
#include "logs.inc"

using namespace std;
using namespace us::gov;
using c = us::gov::io::screen;

c::screen(ostream& os): os(os) {
}

int c::getch() {
    struct termios oldattr, newattr;
    int ch;
    tcgetattr(STDIN_FILENO, &oldattr);
    newattr = oldattr;
    newattr.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newattr);
    ch = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &oldattr);
    return ch;
}

int c::getche() {
    struct termios oldattr, newattr;
    int ch;
    tcgetattr(STDIN_FILENO, &oldattr);
    newattr = oldattr;
    newattr.c_lflag &= ~(ICANON);
    tcsetattr(STDIN_FILENO, TCSANOW, &newattr);
    ch = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &oldattr);
    return ch;
}

bool c::toggle_mute() {
    mute = !mute;
    return mute;
}

void c::print_prompt() {
    lock_guard<mutex> lock(mx);
    print_prompt_();
}

void c::print_prompt_() {
    if (mute) return;
    if (prompt.empty()) {
        os << "$> ";
    }
    else {
        os << prompt << "$> ";
    }
    os << line;
    os.flush();
}

void c::set_mute(bool b) {
    lock_guard<mutex> lock(mx);
    mute = b;
}

ostream c::null_os{0};

string c::capture_input(const supervisor& sup) {
    capturing = true;
    print_prompt();
    while (sup.is_active()) {
        int x = getch();
        if (!sup.is_active()) return "";
        log("Keyboard input", x);
        if (x == 10) {
            *this << '\n';
            break;
        }
        if (x == 127) { //backspace
            if (!line.empty()) {
                line = line.substr(0, line.size() - 1);
                *this << '\n';
                print_prompt();
            }
            continue;
        }
        char c = (char)(x & 0xFF);
        if (c >= 32 && c < 127) {
            line = line + c;
            *this << c;
            flush();
        }
    }
    string lin;
    capturing = false;
    lin = line;
    line = "";
    log("input", lin);
    return lin;
}

