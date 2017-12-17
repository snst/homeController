/**
 * Copyright (c) 2017 by Stefan Schmidt
 */

package de.meek.myhome;

import java.io.ByteArrayOutputStream;


public class Cmd {

    protected eCmd cmd = eCmd.NONE;
    protected byte[] param = null;
    protected int roomId = -1;
    protected BTAddr addr = null;

    public Cmd(int roomId, eCmd cmd) {
        this.cmd = cmd;
        this.roomId = roomId;
    }

    public void setParam(byte[] param) {
        this.param = param;
    }

    public void setBtAddress(BTAddr addr) {
        this.addr = addr;
    }

    public byte[] getBuffer() {
        byte[] buffer = null;
        try {
            int len = 2;
            if (addr != null) {
                len += BTAddr.LENGTH;
            }
            if (param != null) {
                len += param.length;
            }
            // CMD, LEN, BTAddr, Param
            ByteArrayOutputStream out = new ByteArrayOutputStream();
            out.write((byte) cmd.getValue());
            out.write((byte) len);
            if (addr != null) {
                out.write(addr.toByte());
            }
            if(param!=null) {
                out.write(param);
            }
            buffer = out.toByteArray();
        } catch (Exception e) { }
        return buffer;
    }
}


