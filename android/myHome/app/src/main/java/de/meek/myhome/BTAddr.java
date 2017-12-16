/**
 * Copyright (c) 2017 by Stefan Schmidt
 */

package de.meek.myhome;

public class BTAddr {

    protected long addr;
    public static final int LENGTH = 6;

    public BTAddr() {
        addr = 0;
    }

    public boolean convertFromString(String str) {
        addr = 0;
        try {
            addr = Long.parseLong(str, 16);
            return true;
        } catch(Exception e) {
            return false;
        }
    }

    public boolean convertFromBytes(byte[] b, int offset) {
        addr = 0;
        if(b.length<LENGTH+offset) {
            return false;
        }
        for (int i = offset; i < LENGTH+offset; i++) {
            addr <<= 8;
            addr |= (b[i] & 0xFF);
        }
        return true;
    }

    public byte[] toByte() {
        byte[] b = new byte[LENGTH];
        for(int i=0; i<LENGTH;i++) {
            int shift = i*8;
            b[LENGTH-1-i] = (byte)((addr >> shift) & 0xFF);
        }
        return b;
    }

    public void convertFromLong(long _addr) {
        addr = _addr;
    }

    public long toLong() {
        return addr;
    }

    @Override
    public String toString() {
        return Long.toString(addr, 16);
    }

    public boolean isSameAddr(BTAddr _addr) {
        return addr == _addr.toLong();
    }
}
