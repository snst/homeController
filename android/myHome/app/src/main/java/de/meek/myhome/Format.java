package de.meek.myhome;

import de.meek.myhome.Const;

/**
 * Created by stefan on 10.12.2017.
 */

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
}
