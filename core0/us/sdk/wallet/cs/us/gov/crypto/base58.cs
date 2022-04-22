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
using System;

namespace us.gov.crypto
{
    public class AddressFormatException : ArgumentException
    {
        public AddressFormatException(String message)
        {
            throw new ArgumentException(message);
        }
    }

    /**
     * This exception is thrown by {@link Base58}, {@link Bech32} and the {@link PrefixedChecksummedBytes} hierarchy of
     * classes when you try to decode data and a character isn't valid. You shouldn't allow the user to proceed in this
     * case.
     */
    public class InvalidCharacter : Exception
    {
        public InvalidCharacter(char character, int position)
        {
            throw new AddressFormatException("Invalid character '" + character + "' at position " + position.ToString());
        }
    }

    public class base58
    {

        public static readonly char[] ALPHABET = "123456789ABCDEFGHJKLMNPQRSTUVWXYZabcdefghijkmnopqrstuvwxyz".ToCharArray();
        private static readonly char ENCODED_ZERO = ALPHABET[0];
        private static readonly int[] INDEXES = new int[128];

        static base58()
        {
            INDEXES[0] = -1;
            for (int i = 0; i < ALPHABET.Length; i++)
            {
                INDEXES[ALPHABET[i]] = i;
            }
        }

        /**
         * Divides a number, represented as an array of bytes each containing a single digit
         * in the specified base, by the given divisor. The given number is modified in-place
         * to contain the quotient, and the return value is the remainder.
         *
         * @param number the number to divide
         * @param firstDigit the index within the array of the first non-zero digit
         *        (this is used for optimization by skipping the leading zeros)
         * @param base the base in which the number's digits are represented (up to 256)
         * @param divisor the number to divide by (up to 256)
         * @return the remainder of the division operation
         */
        private static byte divmod(byte[] number, int firstDigit, int based, int divisor)
        {
            // this is just long division which accounts for the base of the input digits
            int remainder = 0;
            for (int i = firstDigit; i < number.Length; i++)
            {
                int digit = (int)number[i] & 0xFF;
                int temp = remainder * based + digit;
                number[i] = (byte)(temp / divisor);
                remainder = temp % divisor;
            }
            return (byte)remainder;
        }

        /**
         * Encodes the given bytes as a base58 string (no checksum is appended).
         *
         * @param input the bytes to encode
         * @return the base58-encoded string
         */
        public static String encode(byte[] input_)
        {
            //System.out.println("US_CASH "+" ENCODE 1");
            if (input_.Length == 0)
            {
                return null;
            }
            // Count leading zeros.
            int zeros = 0;
            while (zeros < input_.Length && input_[zeros] == 0)
            {
                ++zeros;
            }
            // Convert base-256 digits to base-58 digits (plus conversion to ASCII characters)
            byte[] input = new byte[input_.Length];
            Buffer.BlockCopy(input_, 0, input, 0, input_.Length); // since we modify it in-place
            char[] encoded = new char[input.Length * 2]; // upper bound
            int outputStart = encoded.Length;
            for (int inputStart = zeros; inputStart < input.Length;)
            {
                encoded[--outputStart] = ALPHABET[divmod(input, inputStart, 256, 58)];
                if (input[inputStart] == 0)
                {
                    ++inputStart; // optimization - skip leading zeros
                }
            }
            // Preserve exactly as many leading encoded zeros in output as there were leading zeros in input.
            while (outputStart < encoded.Length && encoded[outputStart] == ENCODED_ZERO)
            {
                ++outputStart;
            }
            while (--zeros >= 0)
            {
                encoded[--outputStart] = ENCODED_ZERO;
            }
            // Return encoded string (including encoded leading zeros).
            return new String(encoded, outputStart, encoded.Length - outputStart);
        }

        public static byte[] copyOfRange(byte[] src, int start, int end)
        {
            int len = end - start;
            byte[] dest = new byte[len];
            // note i is always from 0
            for (int i = 0; i < len; i++)
            {
                dest[i] = src[start + i]; // so 0..n = 0+x..n+x
            }
            return dest;
        }


        /**
         * Decodes the given base58 string into the original data bytes.
         *
         * @param input the base58-encoded string to decode
         * @return the decoded data bytes
         * @throws AddressFormatException if the given string is not a valid base58 string
         */
        public static byte[] decode(String input)
        {
            try {
                if (input.Length == 0) {
                    return new byte[0];
                }
                // Convert the base58-encoded ASCII chars to a base58 byte sequence (base58 digits).
                byte[] input58 = new byte[input.Length];
                for (int i = 0; i < input.Length; ++i) {
                    char c = input[i];
                    int digit = c < 128 ? INDEXES[c] : -1;
                    if (digit < 0) {
                        throw new InvalidCharacter(c, i);
                    }
                    input58[i] = (byte)digit;
                }
                // Count leading zeros.
                int zeros = 0;
                while (zeros < input58.Length && input58[zeros] == 0) {
                    ++zeros;
                }
                // Convert base-58 digits to base-256 digits.
                byte[] decoded = new byte[input.Length];
                int outputStart = decoded.Length;
                for (int inputStart = zeros; inputStart < input58.Length;) {
                    decoded[--outputStart] = divmod(input58, inputStart, 58, 256);
                    if (input58[inputStart] == 0) {
                        ++inputStart; // optimization - skip leading zeros
                    }
                }
                // Ignore extra leading zeroes that were added during the calculation.
                while (outputStart < decoded.Length && decoded[outputStart] == 0) {
                    ++outputStart;
                }
                // Return decoded data (including original number of leading zeros).
                return copyOfRange(decoded, outputStart - zeros, decoded.Length);
            }
            catch (AddressFormatException)
            {
                return null;
            }
        }
    }
}
