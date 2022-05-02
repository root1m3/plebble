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
#pragma once
#include <us/gov/config.h>
#if CFG_FCGI == 1

#include <iostream>
#include <thread>
#include <fstream>
#include <fastcgi++/request.hpp>
#include <fastcgi++/manager.hpp>

#include <us/wallet/engine/daemon_t.h>

namespace us { namespace wallet { namespace cli {

using namespace std;
using namespace Fastcgipp;
using namespace us::wallet;

//typedef wchar_t char_type;
//typedef wchar_t char_type;
typedef char char_type;
typedef std::basic_string<char_type> string;
typedef std::basic_ostream<char_type> ostream;
typedef std::basic_istream<char_type> istream;
typedef std::basic_ostringstream<char_type> ostringstream;
typedef std::basic_istringstream<char_type> istringstream;
typedef std::basic_ofstream<char_type> ofstream;
typedef std::basic_ifstream<char_type> ifstream;

struct fcgi_t: Fastcgipp::Request<char_type> {
//    typedef basic_string<chartype> string;
//    typedef basic_istringstream<chartype> istringstream;

    fcgi_t() {
    }

    ~fcgi_t() {
    }

    static int count_reqs;
    static us::wallet::engine::daemon_t* api;

    void help(ostream& os) const;
//    Json::Value to_json(const string& s) const;

    bool response();

    void draw_home();

/*
    bool dumpall() {
      wchar_t cookieString[] = { '<', '"', 0x0440, 0x0443, 0x0441, 0x0441, 0x043a, 0x0438, 0x0439, '"', '>', ';', 0x0000 };
//      char cookieString[] = { '<', '"', 0x40, 0x43, 0x41, 0x41, 0x3a, 0x38, 0x39, '"', '>', ';', 0x00 };
//      char_t cookieString[] = { '<', '"', 0x0440, 0x0443, 0x0441, 0x0441, 0x043a, 0x0438, 0x0439, '"', '>', ';', 0x0000 };
      out << "Set-Cookie: echoCookie=" << encoding(URL) << cookieString << encoding(NONE) << "; path=/\n";
      out << "Content-Type: text/html; charset=utf-8\r\n\r\n";
      out << "<html><head><meta http-equiv='Content-Type' content='text/html; charset=utf-8' />";
      out << "<title>fastcgi++: Echo in UTF-8</title></head><body>";
      out << "<h1>Environment Parameters</h1>";
      out << "<p><b>FastCGI Version:</b> " << Protocol::version << "<br />";
      out << "<b>fastcgi++ Version:</b> " << version << "<br />";
      out << "<b>Hostname:</b> " << encoding(HTML) << environment().host << encoding(NONE) << "<br />";
      out << "<b>User Agent:</b> " << encoding(HTML) << environment().userAgent << encoding(NONE) << "<br />";
      out << "<b>Accepted Content Types:</b> " << encoding(HTML) << environment().acceptContentTypes << encoding(NONE) << "<br />";
      out << "<b>Accepted Languages:</b> " << encoding(HTML) << environment().acceptLanguages << encoding(NONE) << "<br />";
      out << "<b>Accepted Characters Sets:</b> " << encoding(HTML) << environment().acceptCharsets << encoding(NONE) << "<br />";
      out << "<b>Referer:</b> " << encoding(HTML) << environment().referer << encoding(NONE) << "<br />";
      out << "<b>Content Type:</b> " << encoding(HTML) << environment().contentType << encoding(NONE) << "<br />";
      out << "<b>Root:</b> " << encoding(HTML) << environment().root << encoding(NONE) << "<br />";
      out << "<b>Script Name:</b> " << encoding(HTML) << environment().scriptName << encoding(NONE) << "<br />";
      out << "<b>Request URI:</b> " << encoding(HTML) << environment().requestUri << encoding(NONE) << "<br />";
      out << "<b>Request Method:</b> " << encoding(HTML) << environment().requestMethod << encoding(NONE) << "<br />";
      out << "<b>Content Length:</b> " << encoding(HTML) << environment().contentLength << encoding(NONE) << "<br />";
      out << "<b>Keep Alive Time:</b> " << encoding(HTML) << environment().keepAlive << encoding(NONE) << "<br />";
      out << "<b>Server Address:</b> " << encoding(HTML) << environment().serverAddress << encoding(NONE) << "<br />";
      out << "<b>Server Port:</b> " << encoding(HTML) << environment().serverPort << encoding(NONE) << "<br />";
      out << "<b>Client Address:</b> " << encoding(HTML) << environment().remoteAddress << encoding(NONE) << "<br />";
      out << "<b>Client Port:</b> " << encoding(HTML) << environment().remotePort << encoding(NONE) << "<br />";
      out << "<b>If Modified Since:</b> " << encoding(HTML) << environment().ifModifiedSince << encoding(NONE) << "</p>";
      out << "<h1>Path Data</h1>";
      if(environment().pathInfo.size())
      {
         std::wstring preTab;
         for(Http::Environment<char_type>::PathInfo::const_iterator it=environment().pathInfo.begin(); it!=environment().pathInfo.end(); ++it)
         {
            out << preTab << encoding(HTML) << *it << encoding(NONE) << "<br />";
            preTab += L"&nbsp;&nbsp;&nbsp;";
         }
      }
      else
         out << "<p>No Path Info</p>";
      out << "<h1>GET Data</h1>";
      if(environment().gets.size())
         for(Http::Environment<char_type>::Gets::const_iterator it=environment().gets.begin(); it!=environment().gets.end(); ++it)
            out << "<b>" << encoding(HTML) << it->first << encoding(NONE) << ":</b> " << encoding(HTML) << it->second << encoding(NONE) << "<br />";
      else
         out << "<p>No GET data</p>";
      
      out << "<h1>Cookie Data</h1>";
      if(environment().cookies.size())
         for(Http::Environment<char_type>::Cookies::const_iterator it=environment().cookies.begin(); it!=environment().cookies.end(); ++it)
            out << "<b>" << encoding(HTML) << it->first << encoding(NONE) << ":</b> " << encoding(HTML) << it->second << encoding(NONE) << "<br />";
      else
         out << "<p>No Cookie data</p>";
      out << "<h1>POST Data</h1>";
      if(environment().posts.size())
      {
         for(Http::Environment<char_type>::Posts::const_iterator it=environment().posts.begin(); it!=environment().posts.end(); ++it)
         {
            out << "<h2>" << encoding(HTML) << it->first << encoding(NONE) << "</h2>";
            if(it->second.type==Http::Post<char_type>::form)
            {
               out << "<p><b>Type:</b> form data<br />";
               out << "<b>Value:</b> " << encoding(HTML) << it->second.value << encoding(NONE) << "</p>";
            }
            
            else
            {
               out << "<p><b>Type:</b> file<br />";
               out << "<b>Filename:</b> " << encoding(HTML) << it->second.filename << encoding(NONE) << "<br />";
               out << "<b>Content Type:</b> " << encoding(HTML) << it->second.contentType << encoding(NONE) << "<br />";
/ *
               out << "<b>Size:</b> " << it->second.size << "<br />";
               out << "<b>Data:</b></p><pre>";
               out.dump(it->second.data.get(), it->second.size);

               out << "</pre>";
* /
            }
         }
      }
      else
         out << "<p>No POST data</p>";
      out << "</body></html>";
      return true;
  }
*/
};


}}}

#endif

