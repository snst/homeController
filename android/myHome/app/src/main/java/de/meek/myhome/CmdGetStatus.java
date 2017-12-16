/**
 * Copyright (c) 2017 by Stefan Schmidt
 */

package de.meek.myhome;

import java.util.Calendar;
import java.util.Date;


public class CmdGetStatus extends Cmd {
    public CmdGetStatus(int roomId) {
        super(roomId, eCmd.GETSTATUS);
//        DateFormat dateFormat = new SimpleDateFormat("yyMMddHHmmss");

        Date today = new Date(); // Fri Jun 17 14:54:28 PDT 2016
        Calendar cal = Calendar.getInstance();
        cal.setTime(today); // don't forget this if date is arbitrary e.g. 01-01-2014

        byte[] param = new byte[6];
        param[0] = (byte)(cal.get(Calendar.YEAR)%100);
        param[1] = (byte)(cal.get(Calendar.MONTH)+1);
        param[2] = (byte)(cal.get(Calendar.DAY_OF_MONTH));
        param[3] = (byte)(cal.get(Calendar.HOUR_OF_DAY));
        param[4] = (byte)(cal.get(Calendar.MINUTE));
        param[5] = (byte)(cal.get(Calendar.SECOND));

        setParam(param);
    }
}
