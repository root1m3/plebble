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
package us.gov.io;
import us.CFG;                                                                                 // CFG
import java.util.concurrent.locks.Condition;                                                   // Condition
import java.io.OutputStream;                                                                   // OutputStream
import java.io.PrintStream;                                                                    // PrintStream
import java.util.concurrent.locks.ReentrantLock;                                               // ReentrantLock

public class screen {

    static void log(final String line) {                //--strip
        CFG.log_gov_io("screen " + line);               //--strip
    }                                                   //--strip

    public static interface supervisor {
        boolean is_active();
    };

    public screen(PrintStream os_) {
        os = os_;
    }

    public int getch() {
        char ch;
        try {
            ch = (char) System.in.read();
            log("System.in.read is " + (int)ch); //--strip
        }
        catch(Exception e) {
            log("KO 59405 Exception " + e.getMessage()); //--strip
            return 0;
        }
        return (int)ch;
/*
        struct termios oldattr, newattr;
        int ch;
        tcgetattr( STDIN_FILENO, &oldattr );
        newattr = oldattr;
        newattr.c_lflag &= ~( ICANON | ECHO );
        tcsetattr( STDIN_FILENO, TCSANOW, &newattr );
        ch = getchar();
        tcsetattr( STDIN_FILENO, TCSANOW, &oldattr );
        return ch;
*/
        //return 0;
    }

    public int getche() {
/*
        struct termios oldattr, newattr;
        int ch;
        tcgetattr( STDIN_FILENO, &oldattr );
        newattr = oldattr;
        newattr.c_lflag &= ~( ICANON );
        tcsetattr( STDIN_FILENO, TCSANOW, &newattr );
        ch = getchar();
        tcsetattr( STDIN_FILENO, TCSANOW, &oldattr );
        return ch;
*/
        return 0;
    }

    boolean new_paragraph = false;

    public void scrlock(boolean newp) {
        lock.lock();
        new_paragraph = newp;
        if (new_paragraph) os.println();
    }

    public void scrunlock() {
        if (new_paragraph) os.println();
        lock.unlock();
    }

    public void print_prompt() {
        lock.lock();
        try {
            print_prompt_();
        }
        finally {
            lock.unlock();
        }
    }

    public void print_prompt_() {
        if (prompt.isEmpty()) {
            os.print("> ");
        }
        else {
            os.print(prompt + "> ");
        }
        os.print(line);
        os.flush();
        log("toy language doesn't flush. Added annoying line feed."); //--strip
        os.println(""); //here
    }

    public void print(String s) {
        lock.lock();
        try {
            if (mute) return;
            os.print(s);
        }
        finally {
            lock.unlock();
        }
    }

    public void print(short s) {
        lock.lock();
        try {
            if (mute) return;
            os.print(s);
        }
        finally {
            lock.unlock();
        }
    }

    public void print(int s) {
        lock.lock();
        try {
            if (mute) return;
            os.print(s);
        }
        finally {
            lock.unlock();
        }
    }

    public void print(long s) {
        lock.lock();
        try {
            if (mute) return;
            os.print(s);
        }
        finally {
            lock.unlock();
        }
    }

    public void println() {
        lock.lock();
        try {
            if (mute) return;
            os.println();
        }
        finally {
            lock.unlock();
        }
    }

    public void println(String s) {
        lock.lock();
        try {
            if (mute) return;
            os.println(s);
        }
        finally {
            lock.unlock();
        }
    }

    public void println(short s) {
        lock.lock();
        try {
            if (mute) return;
            os.println(s);
        }
        finally {
            lock.unlock();
        }
    }

    public void println(int s) {
        lock.lock();
        try {
            if (mute) return;
            os.println(s);
        }
        finally {
            lock.unlock();
        }
    }

    public void println(long s) {
        lock.lock();
        try {
            if (mute) return;
            os.println(s);
        }
        finally {
            lock.unlock();
        }
    }

    public String capture_input(supervisor sup) {
        capturing = true;

/*
        lock.lock();
        try {
            capturing=true;
            long nanos=1000000000;
            log("scr unlock - await"); //--strip
            cv_print_prompt.awaitNanos(nanos); //1sec
            log("scr lock - return from await"); //--strip
        }
        catch(Exception e) {
            log ("KO 4004 - "); //--strip
        }
        finally {
            lock.unlock();
        }
*/
        print_prompt();
        while(sup.is_active()) {
            int x = getch();
            log("Keyboard input "+x); //--strip
            if (!sup.is_active()) return "";
            if (x == 10) {
                scrlock(false);
                os.println();
                scrunlock();
                break;
            }
            if (x == 127) { //backspace
                try {
                    if (!line.isEmpty()) {
                        line = line.substring(0, line.length() - 1);
                        scrlock(false);
                        os.println();
                        scrunlock();
                        print_prompt();
                    }
                }
                finally {
                }
                continue;
            }
            char c = (char)(x&0xFF);
            if (c >= 32 && c < 127) {
                try {
                    line = line + c;
                    //os.print(c);
                    //os.flush();
                }
                finally {
                }
                continue;
            }
        }
        String lin;
        capturing = false;
        lin = line;
        line = "";

        return lin;
    }

    public void mute() {
        lock.lock();
        try {
            bos = os;
            os = new PrintStream(new OutputStream() {
                    public void write(int b) {
                    }
                });
        }
        finally {
            lock.unlock();
        }
    }

    public void unmute() {
        lock.lock();
        try {
            os=bos;
            bos=null;
        }
        finally {
            lock.unlock();
        }
    }

    public boolean muted() {
        lock.lock();
        try {
            return bos!=null;
        }
        finally {
            lock.unlock();
        }
    }

    String line = "";
    public boolean capturing = false;
    public String prompt = "";
    public PrintStream os;
    public PrintStream bos = null;
    public boolean mute = false;

    public final ReentrantLock lock = new ReentrantLock();
    //final Condition cv_print_prompt=lock.newCondition();
    //boolean prompted=false;

}


