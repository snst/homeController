package helpers;

import de.meek.myhome.Const;

/**
 * Created by stefan on 10.12.2017.
 */

public class Format {
    public static String tempToString(int temp, int percent) {

        String strTemp;
        if(temp <= Const.TEMP_MIN) {
            strTemp = "off";
        } else if(temp >= Const.TEMP_MAX) {
            strTemp = "on";
        } else {
            strTemp = (Float.toString(temp/10f) +  "°  (" + percent + "%)");
        }

        return strTemp;
    }
}
