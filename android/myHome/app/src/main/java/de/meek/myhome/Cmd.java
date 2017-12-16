/**
 * Copyright (c) 2017 by Stefan Schmidt
 */

package de.meek.myhome;

import java.io.ByteArrayOutputStream;


public class Cmd {

    protected eCmd cmd = eCmd.NONE;
    protected byte[] param = null;
    protected byte[] buffer = null;
    public int roomId = -1;

    public Cmd(int roomId, eCmd cmd) {
        this.cmd = cmd;
        this.roomId = roomId;
    }

    public void setParam(byte[] param) {
        this.param = param;
    }

    public void setBtAddress(BTAddr addr) {

        try {
            int len = 2 + BTAddr.LENGTH;
            if (param != null) {
                len += param.length;
            }

            ByteArrayOutputStream out = new ByteArrayOutputStream();
            out.write((byte) len);
            out.write(addr.toByte());
            out.write((byte) cmd.getValue());
            if(param!=null) {
                out.write(param);
            }

            buffer = out.toByteArray();
        } catch (Exception e) {

        }
    }
}


