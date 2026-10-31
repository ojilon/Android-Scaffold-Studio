// asstudio:generated v1 DO NOT EDIT
package com.oji.hello;

import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;
import androidx.appcompat.app.AppCompatActivity;

public class HomeScreenBase extends AppCompatActivity {
    protected TextView title;
    protected Button retry;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.res_layout_home);
        
        title = findViewById(R.id.title);
        retry = findViewById(R.id.retry);
        
        retry.setOnClickListener(v -> onRetry());
    }

    protected void onRetry() {
        // abstract handler
    }

    public void setState(String state) {
        // state handling logic (loading, content, error)
    }
}