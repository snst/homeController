/**
 * Copyright (c) 2017 by Stefan Schmidt
 */

package de.meek.myhome;


public class CmdSetTemp extends Cmd {
    public CmdSetTemp(int roomId, int temp) {
        super(roomId, eCmd.SETTEMP);
        setParam(new byte[] { (byte) (temp/5)});
    }
}
