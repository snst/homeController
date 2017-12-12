/**
 * Copyright (c) 2017 by Stefan Schmidt
 */

package de.meek.myhome;


public class Format {
    public static String tempAndPercentToString(int temp, int percent) {

        String strTemp = tempToString(temp);
        strTemp += "  (" + percent + "%)";
        return strTemp;
    }

    public static String tempToString(int temp) {

        String strTemp;
        if(temp <= Const.TEMP_MIN) {
            strTemp = "off";
        } else if(temp >= Const.TEMP_MAX) {
            strTemp = "on";
        } else {
            strTemp = Float.toString(temp/10f) + "°";
        }
        return strTemp;
    }

    public static long getBTAddress(String str) {
        long ret = 0;
        try {
             if(str.length() == 12) {
                 ret = Long.parseLong(str, 16);
            }
            return ret;
        } catch(Exception e) {
            return 0;
        }
    }
}
