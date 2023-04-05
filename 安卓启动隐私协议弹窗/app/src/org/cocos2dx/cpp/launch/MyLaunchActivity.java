package org.cocos2dx.cpp.launch;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.view.KeyEvent;
import android.view.View;
import android.webkit.WebSettings;
import android.webkit.WebView;
import android.widget.Button;
import android.widget.FrameLayout;
import android.widget.TextView;

import com.sant.test.R;

import org.cocos2dx.cpp.AppActivity;

public class MyLaunchActivity extends Activity {

	private static String _agreedSymbol = "AGREE_HQ_SERVICE";
	private MyPrivacyDialog _agrementDialog=null;
	private FrameLayout _bgLayout=null;
	private TextView _titleTxt=null;
	private WebView _webView=null;
	private Button _exitBtn=null;
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.my_launch);
		initShow();
		initDialog();
	}

	private void initShow(){
		_bgLayout=findViewById(R.id.launch_con_node);
		_titleTxt=findViewById(R.id.launch_title);
		_exitBtn=findViewById(R.id.launch_exitBtn);
		_exitBtn.setOnClickListener(new View.OnClickListener(){
			@Override
			public void onClick(View v){
				if(_agrementDialog!=null){
					_agrementDialog.show();
				}
				_bgLayout.setVisibility(View.GONE);
			}
		});

		_webView=findViewById(R.id.launch_WebView);
		WebSettings settings = _webView.getSettings();
		settings.setJavaScriptEnabled(true);
		settings.setDefaultTextEncodingName("UTF-8");
		//自适应手机屏幕
		settings.setUseWideViewPort(true);
		settings.setLoadWithOverviewMode(true);

		_bgLayout.setVisibility(View.GONE);
	}

	private void initDialog(){
		boolean states= (boolean) SPUtil.get(getApplication(),_agreedSymbol,false);
		if (states==false)
		{
			_agrementDialog = new MyPrivacyDialog(this);
			_agrementDialog.show();
			_agrementDialog.setOnBtnClickListener(new MyPrivacyDialog.OnBtnClickListener(){
				@Override
				public void onClick(int type) {
					switch (type) {
						case MyPrivacyDialog.ARGEEMENT_TEXT_CLICK://查看用户协议
							loadUrl(getResources().getString(R.string.url_service),getResources().getString(R.string.user_service));
							break;
						case MyPrivacyDialog.SECRET_TEXT_CLICK://查看隐私协议
							_agrementDialog.hide();
							loadUrl(getResources().getString(R.string.url_private),getResources().getString(R.string.user_private));
							break;
						case MyPrivacyDialog.NOT_ARGEE_BTN_CLICK://不同意按钮
							_agrementDialog.dismiss();
							finish();
							break;
						case MyPrivacyDialog.ARGEE_BTN_CLICK: //同意按钮
							_agrementDialog.dismiss();
							SPUtil.put(getApplication(),_agreedSymbol,true);
							jumpGameActivity();
							break;
					}
				}
			});
		}
		else{
			jumpGameActivity();
		}
	}

	private void loadUrl(String url,String str){
		_bgLayout.setVisibility(View.VISIBLE);
		_titleTxt.setText(str);
		_webView.loadUrl(url);

		_agrementDialog.hide();
	}

	public void jumpGameActivity() {
		// 闪屏结束后，跳转到游戏界面
		Intent intent = new Intent(this, AppActivity.class);
		startActivity(intent);
		this.finish();
	}

	@Override
	public boolean onKeyDown(int keyCode, KeyEvent event)
	{
		if (keyCode == KeyEvent.KEYCODE_BACK)
		{

		}
		return true;
	}
}
