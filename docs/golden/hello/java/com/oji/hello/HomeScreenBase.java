// asstudio:generated v1 DO NOT EDIT
package com.oji.hello;

import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;
import androidx.appcompat.app.AppCompatActivity;

public abstract class HomeScreenBase extends AppCompatActivity {
    protected TextView title;
    protected Button retry;
    private View homeStateLoading;
    private View homeStateContent;
    private View homeStateError;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.home);

        title = findViewById(R.id.title);
        retry = findViewById(R.id.retry);
        homeStateLoading = findViewById(R.id.home_state_loading);
        homeStateContent = findViewById(R.id.home_state_content);
        homeStateError = findViewById(R.id.home_state_error);

        retry.setOnClickListener(v -> onRetry());
        findViewById(R.id.home_state_error_retry).setOnClickListener(v -> onRetry());

        setState("loading");
    }

    protected abstract void onRetry();

    public void setState(String state) {
        homeStateLoading.setVisibility("loading".equals(state) ? View.VISIBLE : View.GONE);
        homeStateContent.setVisibility("content".equals(state) ? View.VISIBLE : View.GONE);
        homeStateError.setVisibility("error".equals(state) ? View.VISIBLE : View.GONE);
    }
}
