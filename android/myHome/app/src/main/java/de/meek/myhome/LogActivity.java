/**
 * Copyright (c) 2017 by Stefan Schmidt
 */

package de.meek.myhome;

import android.content.Context;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.ListView;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;

public class LogActivity extends AppCompatActivity {

    protected MyApplication getMyApp() {
        return ((MyApplication)getApplicationContext());
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_log);

        final ListView listview = (ListView) findViewById(R.id.listLog);

        final StableArrayAdapter adapter = new StableArrayAdapter(this,
                android.R.layout.simple_list_item_1, getMyApp().getLogger().getLog());

        listview.setAdapter(adapter);
    }

     private class StableArrayAdapter extends ArrayAdapter<Logger.LogEntry> {

        public StableArrayAdapter(Context context, int textViewResourceId,
                                  List<Logger.LogEntry> objects) {
            super(context, textViewResourceId, objects);
        }
     }
}
