/****************************************************************************
Copyright (c) 2015-2016 Chukong Technologies Inc.
Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 
http://www.cocos2d-x.org

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
****************************************************************************/
package org.cocos2dx.cpp;

import android.graphics.Color;
import android.net.http.SslError;
import android.os.Bundle;
import org.cocos2dx.lib.Cocos2dxActivity;
import android.os.Build;
import android.util.DisplayMetrics;
import android.view.View;
import android.view.ViewGroup;
import android.view.WindowManager;
import android.view.WindowManager.LayoutParams;
import android.webkit.SslErrorHandler;
import android.webkit.WebSettings;
import android.webkit.WebView;
import android.webkit.WebViewClient;
import android.widget.Button;
import android.widget.FrameLayout;
import android.widget.LinearLayout;
import android.widget.TextView;

import com.sant.sant.R;

public class AppActivity extends Cocos2dxActivity {

    private static AppActivity _activity;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.setEnableVirtualButton(false);
        super.onCreate(savedInstanceState);
        // Workaround in https://stackoverflow.com/questions/16283079/re-launch-of-activity-on-home-button-but-only-the-first-time/16447508
        if (!isTaskRoot()) {
            // Android launched another instance of the root activity into an existing task
            //  so just quietly finish and go away, dropping the user back into the activity
            //  at the top of the stack (ie: the last state of this task)
            // Don't need to finish it again since it's finished in super.onCreate .
            return;
        }
        // Make sure we're running on Pie or higher to change cutout mode
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.P) {
            // Enable rendering into the cutout area
            WindowManager.LayoutParams lp = getWindow().getAttributes();
            lp.layoutInDisplayCutoutMode = WindowManager.LayoutParams.LAYOUT_IN_DISPLAY_CUTOUT_MODE_SHORT_EDGES;
            getWindow().setAttributes(lp);
        }
        // DO OTHER INITIALIZATION BELOW
        _activity=this;
    }

    private static String    	_url="";
    private static String    	_titleStr="";
    private static ViewGroup m_layout;
    private static LinearLayout m_polBgView,m_lineView;
    private static TextView m_titleT;
    private static Button m_polBackBtn;
    private static WebView m_webView;
    public static void loadPolicyView(String url,String titleStr){
        _activity._url=url;
        _activity._titleStr=titleStr;
        _activity.runOnUiThread(new Runnable() {
            @Override
            public void run() {
                //获取屏幕大小
                DisplayMetrics dm=new DisplayMetrics();
                _activity.getWindowManager().getDefaultDisplay().getMetrics(dm);
                int width=dm.widthPixels;
                int height=dm.heightPixels;
                //获取当前显示view
                FrameLayout contentView=(FrameLayout)_activity.getWindow().findViewById(android.R.id.content);
                m_layout =  (ViewGroup)contentView.getChildAt(0);

                int titleHight=40;
                m_polBgView=new LinearLayout(_activity);
                m_polBgView.setBackgroundColor(Color.rgb(255,255,255));
                m_layout.addView(m_polBgView,width,height);

                m_lineView=new LinearLayout(_activity);
                m_lineView.setBackgroundColor(Color.rgb(128,128,128));
                m_lineView.setX(0);
                m_lineView.setY(titleHight-2);
                m_layout.addView(m_lineView,width,2);

                m_titleT=new TextView(_activity);
                m_titleT.setText(_activity._titleStr);
                m_titleT.setTextSize(12);
                m_titleT.setTextColor(Color.rgb(0,0,0));
                m_titleT.setX(0);
                m_titleT.setY(5);
                m_layout.addView(m_titleT,width,titleHight);

                m_polBackBtn=new Button(_activity);
                m_polBackBtn.setOnClickListener(new View.OnClickListener(){
                    @Override
                    public void onClick(View v){
                        if(m_layout!=null){
                            m_layout.removeView(m_polBgView);
                            m_layout.removeView(m_lineView);
                            m_layout.removeView(m_titleT);
                            m_layout.removeView(m_polBackBtn);
                            m_layout.removeView(m_webView);
                        }
                    }
                });
                m_polBackBtn.setBackgroundResource(R.drawable.zres_back);
                m_polBackBtn.setX(width-50);
                m_polBackBtn.setY(5);
                m_layout.addView(m_polBackBtn,34,30);

                m_webView = new WebView(_activity);
                WebSettings settings = m_webView.getSettings();
                settings.setJavaScriptEnabled(true);
                settings.setDefaultTextEncodingName("UTF-8");
                //自适应手机屏幕
                settings.setUseWideViewPort(true);
                settings.setLoadWithOverviewMode(true);
                m_webView.loadUrl(_activity._url);
                m_webView.setY(titleHight);
                m_layout.addView(m_webView,width,height-titleHight);

                m_webView.setWebViewClient(new WebViewClient() {
                    @Override
                    public boolean shouldOverrideUrlLoading(WebView view, String url) {
                        view.loadUrl(url);
                        return super.shouldOverrideUrlLoading(view, url);
                    }

                    public void onReceivedSslError(WebView view, SslErrorHandler handler, SslError error) {
                        //handler.cancel(); // Android默认的处理方式
                        handler.proceed();  // 接受所有网站的证书
                        //handleMessage(Message msg); // 进行其他处理
                    }
                });
            }
        });
    }

}
