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
            state = new uint16_t(0);
        }

        track_t track;
        String label;
        uint16_t state;
    }

    public static class adapter_t extends ArrayAdapter<txlog_entry> {
        private LayoutInflater inflater;

        public adapter_t(role_fragment rf_, Activity ac, ArrayList<txlog_entry> data) {
            super(ac, R.layout.txlog_entry, data);
            log("adapter constructor"); //--strip
            rf = rf_;
            inflater = (LayoutInflater)ac.getSystemService(Context.LAYOUT_INFLATER_SERVICE);
        }

        private class view_holder {
            TextView txinfo;
            MaterialButton cancel;
            MaterialButton confirm;
        }

        void on_cancel(track_t track) {
            log("on_cancel. track " + track.value); //--strip
            rf.a.hmi.command_trade(rf.tr.tid, "cancel " + track.value);
        }

        @Override
        public View getView(int position, View convert_view, ViewGroup parent) {
            view_holder holder = null;
            //View vi = convert_view;
            txlog_entry sbm = getItem(position);
            if (convert_view == null) {
                convert_view = inflater.inflate(R.layout.txlog_entry, null, true);
                holder = new view_holder();
                holder.txinfo = convert_view.findViewById(R.id.txinfo);
                holder.cancel = convert_view.findViewById(R.id.cancel);
                holder.confirm = convert_view.findViewById(R.id.confirm);
                convert_view.setTag(holder);
            }
            else {
                holder = (view_holder) convert_view.getTag();
            }
            //log("bm.qr.to_string() " + bm.qr.to_string()); //--strip
            holder.txinfo.setText("" + sbm.track.value + " " + sbm.label);
            if (sbm.state.value == 2) {
                holder.cancel.setVisibility(View.VISIBLE);
                final adapter_t v = this;
                holder.cancel.setOnClickListener(new View.OnClickListener() {
                        @Override
                        public void onClick(View view) {
                            v.on_cancel(sbm.track);
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
            @Override
            public void onItemClick(AdapterView parentView, View childView, int position, long id) {
//                progressbarcontainer.setVisibility(View.VISIBLE);

               // item_click2(redirects.get(position), position);
            }
        });

        lv.setOnItemLongClickListener(new AdapterView.OnItemLongClickListener() {
            @Override
            public boolean onItemLongClick(AdapterView parentView, View childView, int position, long id) {
                return true;
            }
        });
/*
                if (main._nodes_mode_custom != null) {
                    return true;
                }
                if (!main._nodes_mode_all) return true;
                final EditText input = new EditText(parentView.getContext());
                input.setOnFocusChangeListener(new OnFocusChangeListener() {
                        @Override
                        public void onFocusChange(View v, boolean hasFocus) {
                            input.post(new Runnable() {
                                @Override
                                public void run() {
                                    InputMethodManager inputMethodManager= (InputMethodManager) nodes.this.getSystemService(Context.INPUT_METHOD_SERVICE);
                                    inputMethodManager.showSoftInput(input, InputMethodManager.SHOW_IMPLICIT);
                                }
                            });
                        }
                    });
                input.requestFocus();
                input.setSingleLine();
                AlertDialog dialog = new AlertDialog.Builder(new ContextThemeWrapper(nodes.this, R.style.myDialog))
                    .setTitle("Bookmark")
                    .setMessage("Enter label for qr " + world.get(position).second.qr.to_string())
                    .setView(input)
                    .setPositiveButton("Ok", new DialogInterface.OnClickListener() {
                            @Override
                            public void onClick(DialogInterface dialog, int which) {
                                pair<String, bookmark_t> bm = world.get(position);
                                //byte[] ico = new byte[0];
                                bm.second.label = String.valueOf(input.getText()).trim();
                                us.wallet.engine.rpc_peer_t.bookmark_add_in_t o = new us.wallet.engine.rpc_peer_t.bookmark_add_in_t(new string(bm.first), bm.second);
                                string s = new string();
                                ko r = a.hmi.rpc_peer.call_bookmark_add(o, s);
                                if (is_ko(r)) { //--strip
                                    log(r.msg);  //--strip
                                }//--strip
                                else {//--strip
                                    log("added bookmark"); //--strip
                                }//--strip
                             }
                        })
                    .setNegativeButton("Cancel", new DialogInterface.OnClickListener() {
                            @Override
                            public void onClick(DialogInterface dialog, int which) {
                            }
                        })
                    .create();
                dialog.show();
                return true;
            }
        }
        );
*/

        //set_redirects(new bookmarks_t());
    }

    public void add_entry(final txlog_entry entry) {
        adapter.add(entry);
//        activity.main.runOnUiThread(new Runnable() {
//            public void run() {
//                refresh();
//            }
//        });
//    }
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
            track_t tr = new track_t();
            {
                ko r = reader.read(tr);
                if (ko.is_ko(r)) return r;
                track = tr;
            }
            string s = new string();
            {
                ko r = reader.read(s);
                if (ko.is_ko(r)) return r;
                label = s.value;
            }
            uint16_t st = new uint16_t();
            {
                ko r = reader.read(st);
                if (ko.is_ko(r)) return r;
                state = st;
            }
            return ok;
        }

        track_t track;
        String label;
        uint16_t state;
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
            clear();
            ensureCapacity((int)sz.value);
            for (int i =0; i< sz.value; ++i) {
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
            log("wrong datagram." + r.msg); //--strip
            return;
        }
        update(x);
    }

    public void update(index_t index) {
        log("update index sz " + index.size()); //--strip
        entries = convert(index);
        activity.main.runOnUiThread(new Runnable() {
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
            e.state = o.second.state;
            log("==> " + e.track.value + " " + e.label); //--strip
            x.add(e);
        }
        return x;
    }

/*
    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle saved_instance) {
        view parent = super.onCreateView(inflater, container, saved_instance);
        v = inflater.inflate(R.layout.tip, container, false);
        if (parent != null) {
            parent.addView(v);
            return parent;
        }
        return v;
    }
*/
    ArrayList<txlog_entry> entries = null;
    no_scroll_list_view lv;
    adapter_t adapter = null;
    MaterialCardView card;
    role_fragment rf;
//    public uint32_t id = new uint32_t(0);

}

