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
import android.app.Activity;                                                                   // Activity
import android.widget.AdapterView;                                                             // AdapterView
import androidx.appcompat.app.AlertDialog;                                                     // AlertDialog
import android.widget.ArrayAdapter;                                                            // ArrayAdapter
import java.util.ArrayList;                                                                    // ArrayList
import android.util.AttributeSet;                                                              // AttributeSet
import android.graphics.BitmapFactory;                                                         // BitmapFactory
import android.graphics.Bitmap;                                                                // Bitmap
import us.gov.io.blob_reader_t;                                                                // blob_reader_t
import static us.gov.io.types.blob_t;                                                          // blob_t
import us.gov.io.blob_writer_t;                                                                // blob_writer_t
import us.wallet.trader.bookmarks_t;                                                           // bookmarks_t
import us.wallet.trader.bookmark_t;                                                            // bookmark_t
import android.os.Bundle;                                                                      // Bundle
import android.content.Context;                                                                // Context
import us.gov.socket.datagram;                                                                 // datagram
import us.wallet.trader.data_t;                                                                // data_t
import android.content.DialogInterface;                                                        // DialogInterface
import static us.gov.crypto.ripemd160.hash_t;                                                  // hash_t
import android.widget.ImageView;                                                               // ImageView
import static us.gov.cash.types.*;                                                             // *
import static us.gov.crypto.types.*;                                                           // *
import static us.gov.io.types.blob_t.*;                                                        // *
import static us.gov.socket.types.*;                                                           // *
import static us.ko.*;                                                                         // *
import static us.stdint.*;                                                                     // *
import static us.tuple.*;                                                                      // *
import us.gov.crypto.ripemd160.*;                                                              // *
import us.stdint.*;                                                                            // *
import us.tuple.*;                                                                             // *
import us.ko;                                                                                  // ko
import android.view.LayoutInflater;                                                            // LayoutInflater
import android.widget.LinearLayout;                                                            // LinearLayout
import java.util.ListIterator;                                                                 // ListIterator
import java.util.Map;                                                                          // Map
import com.google.android.material.button.MaterialButton;                                      // MaterialButton
import com.google.android.material.card.MaterialCardView;                                      // MaterialCardView
import us.pair;                                                                                // pair
import us.string;                                                                              // string
import android.widget.TextView;                                                                // TextView
import android.widget.Toast;                                                                   // Toast
import static us.gov.cash.types.track_t;                                                       // track_t
import android.view.ViewGroup;                                                                 // ViewGroup
import android.view.View;                                                                      // View

public class txlog_view extends LinearLayout {

    static void log(final String line) {            //--strip
        CFG.log_android("txlog_view: " + line);     //--strip
    }                                               //--strip

    public static class txlog_entry {

        public txlog_entry() {
            track = new track_t(0L);
            label = "";
            wallet_state = new uint8_t((short)0);
            gov_state = new uint8_t((short)0);
        }
        track_t track;
        String label;
        uint8_t wallet_state;
        uint8_t gov_state;
        String gov_state_info;
    }

    public static class adapter_t extends ArrayAdapter<txlog_entry> {
        private LayoutInflater inflater;

        public adapter_t(role_fragment rf_, Activity ac, ArrayList<txlog_entry> data) {
            super(ac, R.layout.txlog_entry, data);
            log("adapter constructor " + data.size()); //--strip
            rf = rf_;
            inflater = (LayoutInflater)ac.getSystemService(Context.LAYOUT_INFLATER_SERVICE);
        }

        private class view_holder {
            TextView txinfo;
            TextView wallet_track_status;
            TextView gov_track_status;
            MaterialButton cancel;
            MaterialButton confirm;
        }

        void on_cancel(track_t track) {
            log("on_cancel. track " + track.value); //--strip
            rf.a.hmi.command_trade(rf.tr.tid, "cancel " + track.value);
        }

        void on_confirm(track_t track) {
            log("on_confirm. track " + track.value); //--strip
            rf.a.hmi.command_trade(rf.tr.tid, "pay " + track.value);
        }

        String draw_gov_state(uint8_t x) {
            us.gov.engine.track_status_t.evt_status_t v = us.gov.engine.track_status_t.evt_status_t.from_short(x.value);
            if (v == us.gov.engine.track_status_t.evt_status_t.evt_wait_arrival) return "[====>.........................]";
            if (v == us.gov.engine.track_status_t.evt_status_t.evt_calendar) return     "[====>====>....................]";
            if (v == us.gov.engine.track_status_t.evt_status_t.evt_mempool) return      "[====>====>====>...............]";
            if (v == us.gov.engine.track_status_t.evt_status_t.evt_craftblock) return   "[====>====>====>====>..........]";
            if (v == us.gov.engine.track_status_t.evt_status_t.evt_consensus) return    "[====>====>====>====>====>.....]";
            if (v == us.gov.engine.track_status_t.evt_status_t.evt_settled) return      "[====>====>====>====>====>====>]";
            return "";
        }

        @Override public View getView(int position, View convert_view, ViewGroup parent) {
            view_holder holder = null;
            txlog_entry sbm = getItem(position);
            if (convert_view == null) {
                convert_view = inflater.inflate(R.layout.txlog_entry, null, true);
                holder = new view_holder();
                holder.txinfo = convert_view.findViewById(R.id.txinfo);
                holder.wallet_track_status = convert_view.findViewById(R.id.wallet_track_status);
                holder.gov_track_status = convert_view.findViewById(R.id.gov_track_status);
                holder.cancel = convert_view.findViewById(R.id.cancel);
                holder.confirm = convert_view.findViewById(R.id.confirm);
                convert_view.setTag(holder);
            }
            else {
                holder = (view_holder) convert_view.getTag();
            }
            log("" + sbm.wallet_state.value + " " + us.wallet.wallet.local_api.wallet_track_status_str.length); //--strip
            holder.txinfo.setText(sbm.label);
            holder.wallet_track_status.setText(us.wallet.wallet.local_api.wallet_track_status_str[sbm.wallet_state.value]);
            if (sbm.wallet_state.value == us.wallet.wallet.local_api.wallet_track_status_t.wts_delivered.as_short()) {
                holder.gov_track_status.setText(us.gov.engine.track_status_t.evt_status_str[sbm.gov_state.value] + " " + draw_gov_state(sbm.gov_state) + ' ' + sbm.gov_state_info);
                holder.gov_track_status.setVisibility(View.VISIBLE);
            }
            else {
                holder.gov_track_status.setVisibility(View.GONE);
            }
            if (sbm.wallet_state.value == us.wallet.wallet.local_api.wallet_track_status_t.wts_wait_signature.as_short()) {
                holder.cancel.setVisibility(View.VISIBLE);
                final adapter_t v = this;
                holder.cancel.setOnClickListener(new View.OnClickListener() {
                        @Override public void onClick(View view) {
                            v.on_cancel(sbm.track);
                        }
                    });
                holder.confirm.setOnClickListener(new View.OnClickListener() {
                        @Override public void onClick(View view) {
                            v.on_confirm(sbm.track);
                        }
                    });
                holder.confirm.setVisibility(View.VISIBLE);
            }
            else {
                holder.cancel.setVisibility(View.GONE);
                holder.confirm.setVisibility(View.GONE);
            }
            return convert_view;
        }

        role_fragment rf;
    }

    public txlog_view(Context context, AttributeSet attrs) {
        super(context, attrs);
    }

    public void init(role_fragment rf_, Activity ac) {
        rf = rf_;

        card = findViewById(R.id.card);
        lv = (no_scroll_list_view) findViewById(R.id.listviewX);
        assert lv != null;
        if (entries == null) entries = new ArrayList<txlog_entry>();
        adapter = new adapter_t(rf_, ac, entries);
        lv.setAdapter(adapter);
        lv.setOnItemClickListener(new AdapterView.OnItemClickListener() {
            @Override public void onItemClick(AdapterView parentView, View childView, int position, long id) {
            }
        });

        lv.setOnItemLongClickListener(new AdapterView.OnItemLongClickListener() {
            @Override public boolean onItemLongClick(AdapterView parentView, View childView, int position, long id) {
                return true;
            }
        });
    }

    public void add_entry(final txlog_entry entry) {
        adapter.add(entry);
    }

    public boolean refresh() {
        log("refresh"); //--strip
        return true;
    }

    public static class index_item_t implements us.gov.io.seriable {
        @Override public serid_t serial_id() { return serid_t.no_header; }

        @Override public int blob_size() {
            return 0; //TODO
        }

        @Override public void to_blob(blob_writer_t writer) {
            assert false;
            return; //TODO
        }

        @Override public ko from_blob(blob_reader_t reader) {
            string s = new string();
            {
                ko r = reader.read(s);
                if (ko.is_ko(r)) return r;
                label = s.value;
            }
            uint8_t wst = new uint8_t();
            {
                ko r = reader.read(wst);
                if (ko.is_ko(r)) return r;
                wallet_evt_status = wst;
            }
            uint8_t gst = new uint8_t();
            {
                ko r = reader.read(gst);
                if (ko.is_ko(r)) return r;
                gov_evt_status = gst;
            }
            string info = new string();
            {
                ko r = reader.read(info);
                if (ko.is_ko(r)) return r;
                gov_evt_status_info = info.value;
            }
            return ok;
        }

        String label;
        uint8_t wallet_evt_status;
        uint8_t gov_evt_status;
        String gov_evt_status_info;

    }

    @SuppressWarnings("serial")
    public static class index_t extends ArrayList<pair<track_t, index_item_t>> implements us.gov.io.seriable {
        @Override public serid_t serial_id() { return serid_t.no_header; }

        @Override public int blob_size() {
            return 0; //TODO
        }

        @Override public void to_blob(blob_writer_t writer) {
            assert false;
            return; //TODO
        }

        @Override public ko from_blob(blob_reader_t reader) {

            uint64_t sz = new uint64_t();
            {
                ko r = reader.read_sizet(sz);
                if (ko.is_ko(r)) return r;
            }
            log("from_blob " + sz.value); //--strip
            clear();
            ensureCapacity((int)sz.value);
            for (int i = 0; i < sz.value; ++i) {
                track_t x = new track_t();
                {
                    ko r = reader.read(x);
                    if (ko.is_ko(r)) return r;
                }
                index_item_t s = new index_item_t();
                {
                    ko r = reader.read(s);
                    if (ko.is_ko(r)) return r;
                }
                log("from_blob index_item: " + x.to_string() + " " + s.label + " " + s.wallet_evt_status.value + " " + s.gov_evt_status.value); //--strip
                add(new pair<track_t, index_item_t>(x, s));
            }
            return ok;
        }
    };

    public void update(byte[] payload) {
        blob_reader_t reader = new blob_reader_t(new blob_t(payload));
        index_t x = new index_t();
        ko r = x.from_blob(reader);
        if (is_ko(r)) {
            log("Invalid datagram. " + r.msg); //--strip
            return;
        }
        update(x);
    }

    public void update(index_t index) {
        log("update index sz " + index.size()); //--strip
        entries = convert(index);
        rf.tr.a.main.runOnUiThread(new Runnable() {
            public void run() {
                adapter.clear();
                adapter.addAll(entries);
            }
        });
    }

    ArrayList<txlog_entry> convert(index_t index) {
        log("convert"); //--strip
        ArrayList<txlog_entry> x = new ArrayList<txlog_entry>();
        ListIterator<pair<track_t, index_item_t>> i = index.listIterator(index.size());
        while(i.hasPrevious()) {
            txlog_entry e = new txlog_entry();
            pair<track_t, index_item_t> o = i.previous();
            e.track = o.first;
            e.label = o.second.label;
            e.wallet_state = o.second.wallet_evt_status;
            e.gov_state = o.second.gov_evt_status;
            e.gov_state_info = o.second.gov_evt_status_info;
            log("==> " + e.track.value + " " + e.label); //--strip
            x.add(e);
        }
        return x;
    }

    ArrayList<txlog_entry> entries = null;
    no_scroll_list_view lv;
    adapter_t adapter = null;
    MaterialCardView card;
    role_fragment rf;

}

