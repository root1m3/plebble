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
package us.cash;
import us.CFG;                                                                                 // CFG

public class cryptocurrency {
    String mnemonic;
    String token;
    double amount;
    int decimals;
    String logo = "default_coin";

    public cryptocurrency() {
    }

    public cryptocurrency(String mnemonic, String token, double amount, int decimals, String logo) {
        this.mnemonic = mnemonic;
        this.token = token;
        this.amount = amount;
        this.decimals = decimals;
        this.logo = logo;
        if (mnemonic.equals(us.CFG.UGAS) || mnemonic.equals(us.CFG.LGAS)){
            this.logo="gas";
        }
    }

    public void set_mnemonic(String mnemonic) {
        this.mnemonic = mnemonic;
    }

    public void set_token(String token) {
        this.token = token;
    }

    public void set_amount(double amount) {
        this.amount = amount;
    }

    public void set_decimals(int decimals) {
        this.decimals = decimals;
    }

    public void set_logo(String logo) {
        this.logo = logo;
    }

    public String get_mnemonic() {
        return mnemonic;
    }

    public String get_token() {
        return token;
    }

    public double get_amount() {
        return amount;
    }

    public int get_decimals() {
        return decimals;
    }

    public String get_logo() {
        return logo;
    }
}

