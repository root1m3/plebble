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
package us.cash;
import us.gov.io.blob_reader_t;                                                                // blob_reader_t
import us.wallet.trader.bookmarks_t;                                                           // bookmarks_t
import android.os.Bundle;                                                                      // Bundle
import us.gov.socket.datagram;                                                                 // datagram
import us.wallet.trader.data_t;                                                                // data_t
import androidx.fragment.app.Fragment;                                                         // Fragment
import static us.gov.crypto.ripemd160.hash_t;                                                  // hash_t
import static us.gov.io.types.*;                                                               // *
import static us.stdint.*;                                                                     // *
import static us.tuple.*;                                                                      // *
import android.content.Intent;                                                                 // Intent
import static us.ko.is_ko;                                                                     // is_ko
import us.ko;                                                                                  // ko
import android.view.LayoutInflater;                                                            // LayoutInflater
import android.widget.LinearLayout;                                                            // LinearLayout
import com.google.android.material.button.MaterialButton;                                      // MaterialButton
import androidx.core.widget.NestedScrollView;                                                  // NestedScrollView
import us.pair;                                                                                // pair
import us.string;                                                                              // string
import android.widget.Toast;                                                                   // Toast
import android.view.ViewGroup;                                                                 // ViewGroup
import android.view.View;                                                                      // View

public abstract class role_fragment extends Fragment implements datagram_dispatcher_t.handler_t {

    private static void log(final String line) {             //--strip
        CFG.log_android("role_fragment: " + line);           //--strip
    }                                                        //--strip

    public role_fragment() {
        logo = true;
    }

    public role_fragment(boolean logo_) {
        logo = logo_;
    }

    public String init_cards() {
        return "";
    }

    @Override public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle saved_state) {
        log("onCreateView"); //--strip
        View v = inflater.inflate(R.layout.role_fragment, container, false);
        content = v.findViewById(R.id.role_content);
        scroll =  v.findViewById(R.id.scroll);
        tr = (trader)getActivity();
        assert tr != null;
        a = (app) tr.getApplication();
        assert a != null;
        if (tr.getIntent().hasExtra("tid")) {
            Bundle bundle = tr.getIntent().getExtras();
            tid = new hash_t(bundle.getByteArray("tid"));
        }
        else {
            log("KO 6059 - missing tid"); //--strip
            tr.finish();
            return null;
        }
        if (logo) {
            _logo_view = (logo_view) inflater.inflate(R.layout.logo, null);
            content.addView(_logo_view);
            _logo_view.init(this);
        }

        _tip_view = (tip_view) inflater.inflate(R.layout.tip, null);
        content.addView(_tip_view);
        _tip_view.init(this);

        _workflow_view = (workflow_view) inflater.inflate(R.layout.workflow, null);
        content.addView(_workflow_view);

        _redirects_view = (redirects_view) inflater.inflate(R.layout.redirects, null);
        content.addView(_redirects_view);
        _redirects_view.init(this, getActivity());

        if (under_construction) {
            under_construction_view _under_construction_view = (under_construction_view) inflater.inflate(R.layout.under_construction, null);
            content.addView(_under_construction_view);
        }


        String cards = init_cards();
        _workflow_view.init(this, cards, new workflow_item_view.on_click_listener() {
                @Override public void on_send(workflow_item_view v) {
                    String docname = v.local.name;
                    log("on_send " + docname); //--strip
                    a.hmi().command_trade(tr.tid, "send " + docname);
                }

                @Override public void on_show(workflow_item_view v) {
                    String docname = v.local.name;
                    log("on_show " + docname); //--strip
                    String sendcmd = "";
                    if (v.local.mode == workflow_item_t.mode_t.mode_send) {
                        sendcmd = "send " + docname;
                    }
                    launch_doc_viewer(a.i18n.resolve(activity.a.i18n_sid(docname)), docname, "show " + docname + " -p", sendcmd);
                }
            });

        log("connect network-datagram hose");//--strip
        dispatchid = a.hmi().dispatcher.connect_sink(this);
        return v;
    }

    @Override public void onDestroyView() {
        super.onDestroyView();
        log("onDestroyView"); //--strip
        assert a != null;
        assert a.hmi().dispatcher != null;
        a.hmi().dispatcher.disconnect_sink(dispatchid);
    }

    @Override public void on_push(final hash_t target_tid, final uint16_t code, final byte[] payload) {
        log("on_push " + tid.encode() + " " + target_tid.encode() + " code " + code.value + " payload BIN sz: " + payload.length + " bytes"); //--strip
        if (!target_tid.equals(tid)) {
            log("not for me"); //--strip
            return;
        }
        switch(code.value) {
            case us.wallet.trader.trader_protocol.push_logo: {
                log("a logo for me"); //--strip
                if (logo) {
                    _logo_view.setlogo(payload);
                }
                return;
            }
            case us.wallet.trader.trader_protocol.push_tip: {
                log("a tip for me"); //--strip
                string s = new string();
                ko r = blob_reader_t.parse(new blob_t(payload), s);
                if (is_ko(r)) {
                    return;
                }
                _tip_view.set_tip_(s.value);
                return;
            }
            case us.wallet.trader.workflow.trader_protocol.push_redirects: {
                log("redirects for me"); //--strip
                bookmarks_t bm = new bookmarks_t();
                ko r = blob_reader_t.readB(new blob_t(payload), bm);
                if (is_ko(r)) {
                    return;
                }
                _redirects_view.set_redirects(bm);
                return;
            }
        }
    }

    static final int DOC_VIEWER_RESULT = 49832;

    public void launch_doc_viewer(final String title, final String fname, final String fetch_content_cmd, final String action_cmd) {
        Intent intent = new Intent(getContext(), doc_viewer.class);
        intent.putExtra("tid", tr.tid);
        intent.putExtra("title", title);
        intent.putExtra("fname", "tid_" + tr.tid.encode() + "_" + fname);
        intent.putExtra("fetch_content_cmd", fetch_content_cmd);
        intent.putExtra("action_cmd", action_cmd);
        startActivityForResult(intent, DOC_VIEWER_RESULT);
    }

    @Override public void onActivityResult(int request_code, int result_code, Intent data) {
        super.onActivityResult(request_code, result_code, data);
        log("onActivityResult " + request_code + " " + result_code); //--strip
        if (request_code != DOC_VIEWER_RESULT) {
            log("not from doc_viewer"); //--strip
            return;
        }
        if (result_code != doc_viewer.RESULT_OK) {
            log("doc_viewer was cancelled"); //--strip
            return;
        }
        String cmd = data.getStringExtra("actioncommand");
        log("Xecuting action: " + cmd); //--strip
        a.hmi().command_trade(tid, cmd);
    }

    void scroll_down() {
        scroll.postDelayed(new Runnable() {
                @Override
                public void run() {
                    scroll.fullScroll(scroll.FOCUS_DOWN);
                }
            }, 1000);
    }

    boolean archive_button_added = false;
    public void add_archive_button() {
        if (archive_button_added) return;

        LayoutInflater inflater = tr.getLayoutInflater();
        LinearLayout archive_btn = (LinearLayout) inflater.inflate(R.layout.archive_trade, null);
        MaterialButton btn = archive_btn.findViewById(R.id.closetrade_btn);
        btn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                tr.close_trade();
            }
        });
        content.addView(archive_btn);
        archive_button_added = true;
    }

    public boolean refresh() {
        assert tr != null;
        app.assert_ui_thread();  //--strip
        data_t data = tr.get_data();
        if (data == null) {
            tr.setmode_wrong("KO 4021");
            return false;
        }
        tr.setmode_ready();

        if (logo) {
            _logo_view.refresh();
        }
        _tip_view.refresh();
        _workflow_view.refresh();
        _redirects_view.refresh();

        {
            String st = data.find("trade_state");
            if (st != null) {
                String[] x = st.split(" ", 2);
                trade_state = Integer.parseInt(x[0]);
                log("trade_state = " + trade_state + " " + x[1]); //--strip
            }
        }

        return true;
    }

    public hash_t tid = hash_t.zero_;
    int dispatchid;
    public app a;
    public trader tr;
    public ViewGroup content = null;

    logo_view _logo_view;
    tip_view _tip_view;
    workflow_view _workflow_view;
    redirects_view _redirects_view;

    public int trade_state = 0;
    public NestedScrollView scroll;

    boolean logo = true;
    public boolean under_construction = false;
}

