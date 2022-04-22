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

/*
 * Copyright 2009 ZXing authors
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

//package com.google.zxing.integration.android;

/**
 * <p>Encapsulates the result of a barcode scan invoked through {@link IntentIntegrator}.</p>
 *
 * @author Sean Owen
 */
public final class scan_intent_result {

    private final String contents;
    private final String format_name;
    private final byte[] raw_bytes;
    private final Integer orientation;
    private final String error_correction_level;

    scan_intent_result() {
        this(null, null, null, null, null);
    }

    scan_intent_result(String contents,
                 String format_name,
                 byte[] raw_bytes,
                 Integer orientation,
                 String error_correction_level) {
        this.contents = contents;
        this.format_name = format_name;
        this.raw_bytes = raw_bytes;
        this.orientation = orientation;
        this.error_correction_level = error_correction_level;
    }

    /**
     * @return raw content of barcode
     */
    public String get_contents() {
        return contents;
    }

    /**
     * @return name of format, like "QR_CODE", "UPC_A". See {@code BarcodeFormat} for more format names.
     */
    public String get_format_name() {
        return format_name;
    }

    /**
     * @return raw bytes of the barcode content, if applicable, or null otherwise
     */
    public byte[] get_raw_bytes() {
        return raw_bytes;
    }

    /**
     * @return rotation of the image, in degrees, which resulted in a successful scan. May be null.
     */
    public Integer get_orientation() {
        return orientation;
    }

    /**
     * @return name of the error correction level used in the barcode, if applicable
     */
    public String get_error_correction_level() {
        return error_correction_level;
    }

    @Override
    public String toString() {
        int raw_bytes_length = raw_bytes == null ? 0 : raw_bytes.length;
        return "Format: " + format_name + '\n' +
                "Contents: " + contents + '\n' +
                "Raw bytes: (" + raw_bytes_length + " bytes)\n" +
                "Orientation: " + orientation + '\n' +
                "EC level: " + error_correction_level + '\n';
    }

}
