package com.netease.ldnetdiagnoservicedemo_android;

import com.netease.LDNetDiagnoService.*;
import com.netease.LDNetDiagnoService.LDNetDiagnoListener;
import com.netease.ldnetdiagnoservicedemo_android.R;

import android.app.Activity;
import android.os.Bundle;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.ProgressBar;
import android.widget.TextView;

public class MainActivity extends Activity implements OnClickListener, LDNetDiagnoListener{
	Button btn;
	ProgressBar progress;
	TextView text;
	private String showInfo = "";
	private boolean isRunning = false;
	private LDNetDiagnoService _netDiagnoService;
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_main);
		btn = (Button) findViewById(R.id.btn);
		btn.setOnClickListener(this);

		progress = (ProgressBar) findViewById(R.id.progress);
		progress.setVisibility(View.INVISIBLE);
		text = (TextView) findViewById(R.id.text);
	}

	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		// Inflate the menu; this adds items to the action bar if it is present.
		getMenuInflater().inflate(R.menu.main, menu);
		return true;
	}

	@Override
	public boolean onOptionsItemSelected(MenuItem item) {
		// Handle action bar item clicks here. The action bar will
		// automatically handle clicks on the Home/Up button, so long
		// as you specify a parent activity in AndroidManifest.xml.
		int id = item.getItemId();
		if (id == R.id.action_settings) {
			return true;
		}
		return super.onOptionsItemSelected(item);
	}


	@Override
	public void onClick(View v) {
		if(v == btn) {
			if(!isRunning){
				showInfo = "";
				_netDiagnoService = new LDNetDiagnoService(
						"testDemo", "网络诊断应用", "1.0.0",
						"huipang@corp.netease.com",  "deviceID(option)", "caipiao.163.com",
						"carriname", "ISOCountyCode", "MobilCountryCode", "MobileNetCode", this);
				//设置是否使用JNIC 完成traceroute
				this._netDiagnoService.setIfUseJNICTrace(true);
				_netDiagnoService.execute();
				progress.setVisibility(View.VISIBLE);
				text.setText("Traceroute with max 30 hops...");
				btn.setText("停止诊断");
				btn.setEnabled(false);
			} else {
				progress.setVisibility(View.GONE);
				btn.setText("开始诊断");
				_netDiagnoService.cancel(true);
				_netDiagnoService = null;
				btn.setEnabled(true);
			}

			isRunning = !isRunning;
		}
	}


	@Override
	public void OnNetDiagnoFinished(String log) {
		text.setText(log);
		progress.setVisibility(View.GONE);
		btn.setText("开始诊断");
		btn.setEnabled(true);
		_netDiagnoService = null;
		isRunning = false;
	}

	@Override
	public void OnNetDiagnoUpdated(String log) {
		showInfo += log;
		text.setText(showInfo);
	}
}
