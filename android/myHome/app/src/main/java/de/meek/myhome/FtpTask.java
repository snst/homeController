/**
 * Copyright (c) 2017 by Stefan Schmidt
 */

package de.meek.myhome;
import android.util.Log;
import org.apache.commons.net.ftp.FTPClient;
import java.io.IOException;
import java.io.OutputStream;
import java.io.PipedInputStream;
import java.io.PipedOutputStream;
import java.util.Scanner;
import java.util.zip.ZipEntry;
import java.util.zip.ZipInputStream;
import android.os.AsyncTask;

public class FtpTask extends AsyncTask<Void, Void, String> {

    private FTPClient client = null;
    public String servername = null;
    public String file = null;
    public String username = "anonymous";
    public String password = "hdfhdhjj@hjhdkdajs.com";

    public FtpTask(String servername, String file){
        this.servername = servername;
        this.file = file;
        client = new FTPClient();
        this.client.setConnectTimeout(10*1000);
//        client.setFileTransferMode(org.apache.commons.net.ftp.FTP.COMPRESSED_TRANSFER_MODE);
    }

    @Override
    protected String doInBackground(Void... params) {
        String str = null;
        try {
            PipedOutputStream data = new PipedOutputStream();
            PipedInputStream in = new PipedInputStream(data, 20480);
            if (connect(servername, username, password)) {
                if (downloadFile(file, data)) {
                    ZipInputStream zis = new ZipInputStream(in);
                    ZipEntry entry;
                    while ((entry = zis.getNextEntry()) != null)
                    {
    //                  Log.d("zip", "entry: " + entry.getName() + ", " + entry.getSize());
                        Scanner s = new Scanner(zis).useDelimiter("\\A");
                        str = s.hasNext() ? s.next() : "";
                    }
                }
            }
        } catch (IOException e) {
            e.printStackTrace();
        }
        disconnect();
        return str;
    }

    public void setTimeout (int seconds) throws  Exception{
        try {
            client.setConnectTimeout(seconds * 1000);
        }catch (Exception e){
            throw e;
        }
    }

    public void disconnect(){
        try {
            client.disconnect();
        }catch (Exception e ){
            e.printStackTrace();
        }
    }

    public boolean connect(String server, String name, String password) {
        boolean ret = false;
        try {
            client.connect(server);
            ret = client.login(name, password);
 //           Log.e("ftp", String.valueOf(status));
        }
        catch (Exception e) {
            e.printStackTrace();
        }
        return ret;
    }

    public boolean downloadFile(String remoteFilePath, OutputStream out) {
        boolean ret = false;
        try {
            client.setFileType(FTPClient.BINARY_FILE_TYPE);
            ret = client.retrieveFile(remoteFilePath, out);
//            Log.e("Status", String.valueOf(status));
        } catch (Exception e) {
            e.printStackTrace();
        }
        return ret;
    }
}




