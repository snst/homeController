/**
 * Copyright (c) 2017 by Stefan Schmidt
 */

package de.meek.myhome;

import java.text.DateFormat;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Date;

public class Logger {
    public class LogEntry {
        public Date date;
        public String msg;
        public BTAddr addr;

        public LogEntry(String msg, BTAddr addr) {
            date = new Date();
            this.msg = msg;
            this.addr = addr;
        }

        public String toString() {
            DateFormat dateFormat = new SimpleDateFormat("HH:mm:ss dd.MM.yyyy");
            String bt = "";
            if(addr!=null) {
                bt = "  [" + addr.toString() + "]";
            }
            return dateFormat.format(date) + bt + "\n" + msg;
        }
    }

    protected ArrayList<LogEntry> log = new ArrayList<LogEntry>();
    public void add(String msg) {
        log.add(new LogEntry(msg, null));
    }
    public void add(String msg, BTAddr addr) {
        log.add(new LogEntry(msg, addr));
    }

    public ArrayList<LogEntry> getLog() {
        return log;
    }
}
