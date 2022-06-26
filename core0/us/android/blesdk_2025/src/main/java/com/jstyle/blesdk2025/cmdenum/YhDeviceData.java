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
package com.jstyle.blesdk2025.cmdenum;

import java.io.Serializable;

public class YhDeviceData implements Serializable {
    int dataType;// is index
    String data="";//data object
    boolean dataEnd=false; //Is it over?

    public boolean isDataEnd() {
        return dataEnd;
    }

    public void setDataEnd(boolean dataEnd) {
        this.dataEnd = dataEnd;
    }

    public int getDataType() {
        return dataType;
    }

    public void setDataType(int dataType) {
        this.dataType = dataType;
    }

    public String getData() {
        return data==null?"":data;
    }

    public void setData(String data) {
        this.data = data;
    }

    @Override
    public String toString() {
        return "YhDeviceData{" +
                "dataType=" + dataType +
                ", data=" + data  +
                ", dataEnd=" + dataEnd +
                '}';
    }
}
