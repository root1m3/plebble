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
package us.gov.socket;
import us.gov.io.blob_reader_t;                                                                // blob_reader_t
import us.gov.io.blob_writer_t;                                                                // blob_writer_t
import us.CFG;                                                                                 // CFG
import static us.gov.socket.types.*;                                                           // *
import us.stdint.*;                                                                            // *
import us.ko;                                                                                  // ko
import static us.ko.ok;                                                                        // ok
import us.pair;                                                                                // pair
import java.io.PrintStream;                                                                    // PrintStream
import us.gov.protocol;                                                                        // protocol
import java.net.Socket;                                                                        // Socket
import us.string;                                                                              // string

public abstract class peer_t extends client implements api {

    public static final ko KO_50143 = new ko("KO 50143 Unexpected service number.");
    public static final ko KO_20179 = new ko("KO 20179 Ignored.");
    public static final ko KO_10210 = new ko("KO 10210 Not implemented.");

    static void log(final String line) {                                          //--strip
        CFG.log_gov_socket("peer_t: " + line);                                    //--strip
    }                                                                             //--strip

    public peer_t(rpc_daemon_t rpc_daemon) {
        super(rpc_daemon);
    }

    public peer_t(rpc_daemon_t rpc_daemon, Socket sock) {
        super(rpc_daemon, sock);
    }

    public void ping() {
        string pong = new string();
        ko r = call_ping(new string("ping"), pong);
    }

    public void on_peer_disconnected(reason_t reason) {
        log("on_peer_disconnected " + reason.value); //--strip
    }

    @Override public void dump(final String prefix, PrintStream os) {
        os.println(prefix + "socket::peer_t: idle(recv) " + age(activity_recv));;
        super.dump(prefix, os);
    }

    public void prepare_worker_recv() {
        daemon.recv_led().set();
        //++busyrecv;
        log("prepare_worker recv"); //--strip
    }

    public void worker_ended_recv() {
        //--busyrecv;
        daemon.recv_led().reset();
        log("worker ended recv"); //--strip
    }

    public ko send1(datagram d) {
        if (!daemon.is_active()) {
            ko r = new ko("KO 73101 daemon is inactive.");
            log(r.msg); //--strip
            return r;
        }
        return daemon.send1(this, d);
    }

    public pair<ko, datagram> sendrecv(datagram d, string remote_error) {
        log("sendrecv"); //--strip
        return daemon.sendrecv(this, d, remote_error);
    }

    public void process_ok_work(datagram d) {
        log("process_ok_work " + d.service.value); //--strip
        daemon.process_ok_work(this, d);
    }

    public void process_ko_work(final channel_t channel, final seq_t seq, ko r) {
        log("process_ko_work. seq " + seq.value + " " + r.msg); //--strip
        daemon.process_ko_work(this, channel, seq, r);
    }

    @Override public void set_finished() {
        log("set finished"); //--strip
        super.set_finished();
    }

    public boolean process_work(datagram d) {
        assert d != null;
        assert d.service.value < protocol.socket_end;
        switch(d.service.value) {
            //#include <us/api/generated/gov/java/socket/hdlr_svc-router>
            //------------------------------------------------------------__begin__------generated by configure, do not edit.
            //content of file: <us/api/generated/gov/java/socket/hdlr_svc-router>
            // socket - master file: us/api/data/gov/socket

            case protocol.socket_error: { return process_async_api__socket_error(d); }
            case protocol.socket_ping: { return process_sync_api__socket_ping(d); }
            case protocol.socket_finished: { return process_async_api__socket_finished(d); }
            //-/----------------------------------------------------------___end___------generated by configure, do not edit.
        }
        return false;
    }

    //#include <us/api/generated/gov/java/socket/hdlr_svc_handler-impl>
    //------------------------------------------------------------__begin__------generated by configure, do not edit.
    //content of file: <us/api/generated/gov/java/socket/hdlr_svc_handler-impl>
    // socket - master file: us/api/data/gov/socket

    boolean process_async_api__socket_error(datagram d) {
        log("protocol.socket_error"); //--strip
        string msg = new string();
        {
            ko r = blob_reader_t.readD(d, msg);
            if (ko.is_ko(r)) {
                log(r.msg); //--strip
                return true; //processed
            }
        }
        {
            ko r = handle_error(d, msg);
            if (ko.is_ko(r)) {
                process_ko_work(d.decode_channel(), d.decode_sequence(), r);
                return true; //processed
            }
        }
        return true; //processed
    }

    boolean process_sync_api__socket_ping(datagram d) {
        log("protocol.socket_ping"); //--strip
        string msg = new string();
        {
            ko r = blob_reader_t.readD(d, msg);
            if (ko.is_ko(r)) {
                log(r.msg); //--strip
                return true; //processed
            }
        }
        string ans = new string();
        {
            ko r = handle_ping(msg, ans);
            if (ko.is_ko(r)) {
                process_ko_work(d.decode_channel(), d.decode_sequence(), r);
                return true; //processed
            }
        }
        datagram dout = blob_writer_t.get_datagram(d.decode_channel(), new svc_t(protocol.socket_ping_response), d.decode_sequence(), ans);
        log("processing response svc = " + dout.service.value); //--strip
        process_ok_work(dout);
        return true; //processed
    }

    boolean process_async_api__socket_finished(datagram d) {
        log("protocol.socket_finished"); //--strip
        string reason = new string();
        {
            ko r = blob_reader_t.readD(d, reason);
            if (ko.is_ko(r)) {
                log(r.msg); //--strip
                return true; //processed
            }
        }
        {
            ko r = handle_finished(reason);
            if (ko.is_ko(r)) {
                process_ko_work(d.decode_channel(), d.decode_sequence(), r);
                return true; //processed
            }
        }
        return true; //processed
    }
    //-/----------------------------------------------------------___end___------generated by configure, do not edit.

//------------------apitool - API Spec defined @ us/api/generated/gov/java/socket/hdlr_local-impl

@Override public ko handle_error(final datagram dgram, final string msg) {
    log("error svc " + dgram.service.value + " sz " + dgram.size()); //--strip
    daemon.rendezvous.arrived(dgram);
    return ok;
}

@Override public ko handle_ping(final string msg, string ans) {
    log("ping "); //--strip
    ans.value = "pong";
    return ok;
}

@Override public ko handle_finished(final string reason) {
    log("finished " + reason.value); //--strip
    on_peer_disconnected(new reason_t(reason.value));
    return ok;
}

//-/----------------apitool - End of API implementation.



    //#include <us/api/generated/gov/java/socket/cllr_rpc-impl>
    //------------------------------------------------------------__begin__------generated by configure, do not edit.
    //content of file: <us/api/generated/gov/java/socket/cllr_rpc-impl>
    @Override public ko call_error(final string msg) {
        log("error "); //--strip
        return send1(blob_writer_t.get_datagram(daemon.channel, new svc_t(protocol.socket_error), new seq_t(0), msg));
    }

    @Override public ko call_ping(final string msg, string ans) {
        log("ping "); //--strip
        datagram d_in;
        {
            string lasterr = new string();
            pair<ko, datagram> r = sendrecv(blob_writer_t.get_datagram(daemon.channel, new svc_t(protocol.socket_ping), new seq_t(0), msg), lasterr);
            if (ko.is_ko(r.first)) {
                assert r.second == null;
                if (lasterr.value != null) {
                    lasterror.set(lasterr.value);
                }
                else {
                    lasterror.set("");
                }
                return r.first;
            }
            assert r.second != null;
            d_in = r.second;
        }
        if (d_in.service.value != protocol.socket_ping_response) {
            ko r = KO_50143;
            log(r.msg + "expected " + protocol.socket_ping_response + " got " + d_in.service.value); //--strip
            return r;
        }
        ko r = blob_reader_t.readD(d_in, ans);
        return r;
    }

    @Override public ko call_finished(final string reason) {
        log("finished "); //--strip
        return send1(blob_writer_t.get_datagram(daemon.channel, new svc_t(protocol.socket_finished), new seq_t(0), reason));
    }
    //-/----------------------------------------------------------___end___------generated by configure, do not edit.

    public static ThreadLocal<String> lasterror = new ThreadLocal<String>();

}

