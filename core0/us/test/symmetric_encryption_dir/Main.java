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
import us.wallet.*;
import java.security.PrivateKey;
import java.security.PublicKey;
import java.security.GeneralSecurityException;

public class Main{
    public static void main(String [ ] args) throws GeneralSecurityException{
        if (args.length > 3){
            try{

                byte[] priv=Base58.decode(args[0]);
                byte[] pub=Base58.decode(args[1]);
                String command = args[2];

                PrivateKey privateKey = EllipticCryptography.secp256k1.getPrivateKey(priv);
                PublicKey publicKey = EllipticCryptography.secp256k1.getPublicKey(pub);

                SymmetricEncryption se = new SymmetricEncryption(privateKey, publicKey);

                if(command.equals("decrypt")){

                    byte[] message = Base58.decode(args[3]);
                    byte[] decrypted = se.decrypt(message);
                    String decrypted_string = new String(decrypted);
                    System.out.println(decrypted_string);
                }
                if(command.equals("encrypt")){

                    byte[] message = args[3].getBytes();
                    byte[] encrypted = se.encrypt(message);
                    System.out.println(Base58.encode(encrypted));
                }
            }
            catch(GeneralSecurityException e){
                System.out.println("error in java code: " + e);
            }
        }



    }



}
