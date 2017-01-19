package com.example.zigbee;



import android.os.Bundle;
import android.os.Handler;
import android.os.IBinder;
import android.os.Message;

import android.app.Activity;
import android.text.InputType;
import android.util.Log;
import android.view.Menu;
import android.view.MotionEvent;
import android.view.View;
import android.view.View.*;
import android.view.inputmethod.InputMethodManager;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TabHost;
import android.widget.TabHost.TabSpec;
import android.widget.TextView;
import android.widget.Toast;
import android.widget.ToggleButton;

import java.net.InetAddress;  
import java.net.NetworkInterface;  
import java.net.SocketException;  
import java.text.SimpleDateFormat;
import java.util.Enumeration;

import java.util.concurrent.locks.Lock;
import java.util.concurrent.locks.ReentrantLock;

import com.example.zigbee.UDP_Receiver.str_addr_BUF;






public class MainActivity extends Activity {

	public
	TabHost m_TabHost;
	ToggleButton tgbtn_light;
	Button btn_1,btn_2,btn_3,btn_4,btn_5,btn_6,btn_7,btn_8,btn_9,btn_0,btn_del,btn_dot;
	Button btn_temp,btn_send,btn_addrcfg,btn_testlink;
	TextView text_temp,text_rec;
	EditText edit_localip,edit_srvip,edit_port,edit_send;

	String localip="127.0.0.1";
	String srvip="127.0.0.1";
	String str_rec="";
	Thread mReceiveThread;  
	private Lock UILock=new ReentrantLock();


	final Handler mHandler= new Handler(){
		public void handleMessage(Message msg) {// 此方法在ui线程运行

			String s="";
			if(msg.what!=UDP_Receiver.FLAG_ERR){


				str_addr_BUF sab=(UDP_Receiver.str_addr_BUF)msg.obj;
				switch (msg.what) {
				case UDP_Receiver.FLAG_LIGHT_OFF:
					tgbtn_light.setChecked(false);
					tgbtn_light.setText("打开台灯");
					break;
				case UDP_Receiver.FLAG_LIGHT_ON:
					tgbtn_light.setChecked(true);
					tgbtn_light.setText("关闭台灯");
					break;
				case UDP_Receiver.FLAG_TEMP:
					text_temp.setText(sab.str);
					break;
				}
				SimpleDateFormat sdf=new SimpleDateFormat("hh:mm:ss");  
				String str_time=sdf.format(new java.util.Date());
				s="["+str_time+"][From "+sab.addr+"]\n"+"["+sab.str+"]\n";
			}
			else s=(String)msg.obj;
			UILock.lock();
			s = s + text_rec.getText();
			text_rec.setText(s);
			UILock.unlock();
		}
	};

	int port=6000;

	public String getLocalIpAddress() {
		String ipaddress="";
		try {  
			for (Enumeration<NetworkInterface> en = NetworkInterface  
					.getNetworkInterfaces(); en.hasMoreElements();) {  
				NetworkInterface intf = en.nextElement();  
				for (Enumeration<InetAddress> enumIpAddr = intf  
						.getInetAddresses(); enumIpAddr.hasMoreElements();) {  
					InetAddress inetAddress = enumIpAddr.nextElement();  
					if (!inetAddress.isLoopbackAddress()) {  
						ipaddress=inetAddress.getHostAddress().toString();  
					}  
				}  
			}  
		} catch (SocketException ex) {  
			Log.e("WifiPreference IpAddress", ex.toString());  
		}  
		return ipaddress;  
	}




	private void hideIM(View edt){ 
		// try to hide input_method: 
		try { 
			InputMethodManager im = (InputMethodManager) getSystemService(Activity.INPUT_METHOD_SERVICE);
			IBinder  windowToken = edt.getWindowToken(); 
			if(windowToken != null) { 
				// always de-activate IM 
				im.hideSoftInputFromWindow(windowToken, 0); 
			} 
		} catch (Exception e) { 
			Log.e("HideInputMethod", "failed:"+e.getMessage()); 
		} 
	}



	@Override
	protected void onCreate(Bundle savedInstanceState) {



		super.onCreate(savedInstanceState);


		setContentView(R.layout.activity_main);
		m_TabHost=(TabHost)findViewById(android.R.id.tabhost);
		m_TabHost.setup();
		TabSpec spec1=m_TabHost.newTabSpec("tab1").setIndicator("远程控制",
				getResources().getDrawable(android.R.drawable.ic_menu_share)).setContent(R.id.tab1);
		TabSpec spec2=m_TabHost.newTabSpec("tab2").setIndicator("网络设置",
				getResources().getDrawable(android.R.drawable.ic_menu_manage)).setContent(R.id.tab2);
		m_TabHost.addTab(spec1);
		m_TabHost.addTab(spec2);

		edit_localip=(EditText)findViewById(R.id.edit_localip);
		edit_srvip=(EditText)findViewById(R.id.edit_srvip);
		edit_port=(EditText)findViewById(R.id.edit_port);
		edit_send=(EditText)findViewById(R.id.edit_send);

		text_rec=(TextView)findViewById(R.id.text_rec);
		text_temp=(TextView)findViewById(R.id.text_temp);

		btn_send=(Button)findViewById(R.id.btn_send);
		btn_temp=(Button)findViewById(R.id.btn_temp);
		btn_addrcfg=(Button)findViewById(R.id.btn_addrcfg);
		btn_testlink=(Button)findViewById(R.id.btn_testlink);
		btn_1=(Button)findViewById(R.id.btn_1);
		btn_2=(Button)findViewById(R.id.btn_2);
		btn_3=(Button)findViewById(R.id.btn_3);
		btn_4=(Button)findViewById(R.id.btn_4);
		btn_5=(Button)findViewById(R.id.btn_5);
		btn_6=(Button)findViewById(R.id.btn_6);
		btn_7=(Button)findViewById(R.id.btn_7);
		btn_8=(Button)findViewById(R.id.btn_8);
		btn_9=(Button)findViewById(R.id.btn_9);
		btn_0=(Button)findViewById(R.id.btn_0);
		btn_dot=(Button)findViewById(R.id.btn_dot);
		btn_del=(Button)findViewById(R.id.btn_del);

		tgbtn_light=(ToggleButton)findViewById(R.id.tgbtn_light);
		tgbtn_light.setText("打开台灯");

		localip=getLocalIpAddress();
		edit_localip.setText(localip);


		UDP_Receiver receiver=new UDP_Receiver(mHandler);
		mReceiveThread= new Thread(receiver);  
		mReceiveThread.start();  	
		
		
		edit_srvip.setText(srvip);
		edit_srvip.setOnFocusChangeListener(new OnFocusChangeListener(){
			@Override 
			public void onFocusChange(View v, boolean hasFocus) { 
				if (hasFocus==true) { 

					hideIM(v);
				}		
			}
		});
		edit_srvip.setOnTouchListener(new OnTouchListener(){ 
			@Override 
			public boolean onTouch(View v, MotionEvent event) { 
				edit_srvip.setInputType(InputType.TYPE_NULL); 
				return false;}});


		edit_port.setText("6000");
		edit_port.setOnFocusChangeListener(new OnFocusChangeListener(){
			@Override 
			public void onFocusChange(View v, boolean hasFocus) { 
				if (hasFocus==true) { 
					hideIM(v);
				}		
			}
		});
		edit_port.setOnTouchListener(new OnTouchListener(){ 
			@Override 
			public boolean onTouch(View v, MotionEvent event) { 
				edit_port.setInputType(InputType.TYPE_NULL); 
				return false;}});

		edit_send.setOnFocusChangeListener(new OnFocusChangeListener(){
			@Override 
			public void onFocusChange(View v, boolean hasFocus) { 
				hideIM(v);					
			}
		});



		tgbtn_light.setOnClickListener(new OnClickListener(){
			@Override
			public void onClick(View v) {
				String str_send="",str_show="";
				
				if (UDP_Receiver.isLOn) {
					str_send = "LOFF\0";
					str_show = "关灯";
					tgbtn_light.setChecked(true);
					tgbtn_light.setText("关闭台灯");

				} else {
					str_send = "LON\0";
					str_show = "开灯";
					tgbtn_light.setChecked(false);
					tgbtn_light.setText("打开台灯");
				}

				
				hideIM(v);
				UDP_Sender sender=new UDP_Sender(str_send,srvip,port);
				Toast toast;
				String s_temp=sender.send();
				if(s_temp.equals("发送成功"))
				{
					String s=sender.makeStr(str_show);
					UILock.lock();
					s=s+text_rec.getText();
					text_rec.setText(s);
					UILock.unlock();
				}
				toast=Toast.makeText(getBaseContext(),s_temp, Toast.LENGTH_SHORT);
				toast.show();

			}
		});

		btn_temp.setOnClickListener(new OnClickListener(){
			@Override
			public void onClick(View v) {
				UDP_Sender sender=new UDP_Sender("TEMP\0",srvip,port);
				Toast toast;
				String s_temp=sender.send();
				if(s_temp.equals("发送成功"))
				{
					String s=sender.makeStr("温度刷新");
					UILock.lock();
					s=s+text_rec.getText();
					text_rec.setText(s);
					UILock.unlock();
				}
				toast=Toast.makeText(getBaseContext(),s_temp, Toast.LENGTH_SHORT);
				toast.show();
			}
		});


		btn_send.setOnClickListener(new OnClickListener(){
			@Override
			public void onClick(View v) {

				hideIM(v);
				String str=edit_send.getText().toString();
				if(str.length()>0)
				{
					UDP_Sender sender=new UDP_Sender(str+"\0",srvip,port);
					Toast toast;
					String s_temp=sender.send();
					if(s_temp.equals("发送成功"))
					{

						String s=sender.makeStr(str);
						UILock.lock();
						s=s+text_rec.getText();
						text_rec.setText(s);
						UILock.unlock();

					}
					toast=Toast.makeText(getBaseContext(),s_temp, Toast.LENGTH_SHORT);
					toast.show();

				}

			}
		});



		btn_addrcfg.setOnClickListener(new OnClickListener(){
			@Override
			public void onClick(View v) {

				if(edit_srvip.length()>0&&edit_port.length()>0)
				{

					srvip=edit_srvip.getText().toString();				
					port=Integer.parseInt(edit_port.getText().toString());
					Toast toast=Toast.makeText(getBaseContext(), "修改成功",Toast.LENGTH_SHORT);
					toast.show();
				}
				else
				{
					Toast toast=Toast.makeText(getBaseContext(), "地址与端口不可为空",Toast.LENGTH_SHORT);
					toast.show();
				}		


			}
		});



		btn_testlink.setOnClickListener(new OnClickListener(){
			@Override
			public void onClick(View v) {
				UDP_Sender sender=new UDP_Sender("TEST\0",srvip,port);
				Toast toast;
				String s_temp=sender.send();
				if(s_temp.equals("发送成功"))
				{
					String s=sender.makeStr("检查连接");
					UILock.lock();
					s=s+text_rec.getText();
					text_rec.setText(s);
					UILock.unlock();
					s_temp="检查请求已发送,请查看收发记录!";
				}
				toast=Toast.makeText(getBaseContext(),s_temp, Toast.LENGTH_SHORT);
				toast.show();

			}
		});


		btn_1.setOnClickListener(new OnClickListener(){
			@Override
			public void onClick(View v) {
				if(edit_srvip.isFocused()){
					int sel=edit_srvip.getSelectionStart();
					edit_srvip.getText().insert(sel, "1");
				}					
				if(edit_port.isFocused()){
					int sel=edit_port.getSelectionStart();
					edit_port.getText().insert(sel, "1");
				}}});

		btn_2.setOnClickListener(new OnClickListener(){
			@Override
			public void onClick(View v) {
				if(edit_srvip.isFocused()){
					int sel=edit_srvip.getSelectionStart();
					edit_srvip.getText().insert(sel, "2");
				}					
				if(edit_port.isFocused()){
					int sel=edit_port.getSelectionStart();
					edit_port.getText().insert(sel, "2");
				}}});
		btn_3.setOnClickListener(new OnClickListener(){
			@Override
			public void onClick(View v) {
				if(edit_srvip.isFocused()){
					int sel=edit_srvip.getSelectionStart();
					edit_srvip.getText().insert(sel, "3");
				}					
				if(edit_port.isFocused()){
					int sel=edit_port.getSelectionStart();
					edit_port.getText().insert(sel, "3");
				}}});
		btn_4.setOnClickListener(new OnClickListener(){
			@Override
			public void onClick(View v) {
				if(edit_srvip.isFocused()){
					int sel=edit_srvip.getSelectionStart();
					edit_srvip.getText().insert(sel, "4");
				}					
				if(edit_port.isFocused()){
					int sel=edit_port.getSelectionStart();
					edit_port.getText().insert(sel, "4");
				}}});
		btn_5.setOnClickListener(new OnClickListener(){
			@Override
			public void onClick(View v) {
				if(edit_srvip.isFocused()){
					int sel=edit_srvip.getSelectionStart();
					edit_srvip.getText().insert(sel, "5");
				}					
				if(edit_port.isFocused()){
					int sel=edit_port.getSelectionStart();
					edit_port.getText().insert(sel, "5");
				}}});
		btn_6.setOnClickListener(new OnClickListener(){
			@Override
			public void onClick(View v) {
				if(edit_srvip.isFocused()){
					int sel=edit_srvip.getSelectionStart();
					edit_srvip.getText().insert(sel, "6");
				}					
				if(edit_port.isFocused()){
					int sel=edit_port.getSelectionStart();
					edit_port.getText().insert(sel, "6");
				}}});
		btn_7.setOnClickListener(new OnClickListener(){
			@Override
			public void onClick(View v) {
				if(edit_srvip.isFocused()){
					int sel=edit_srvip.getSelectionStart();
					edit_srvip.getText().insert(sel, "7");
				}					
				if(edit_port.isFocused()){
					int sel=edit_port.getSelectionStart();
					edit_port.getText().insert(sel, "7");
				}}});
		btn_8.setOnClickListener(new OnClickListener(){
			@Override
			public void onClick(View v) {
				if(edit_srvip.isFocused()){
					int sel=edit_srvip.getSelectionStart();
					edit_srvip.getText().insert(sel, "8");
				}					
				if(edit_port.isFocused()){
					int sel=edit_port.getSelectionStart();
					edit_port.getText().insert(sel, "8");
				}}});
		btn_9.setOnClickListener(new OnClickListener(){
			@Override
			public void onClick(View v) {
				if(edit_srvip.isFocused()){
					int sel=edit_srvip.getSelectionStart();
					edit_srvip.getText().insert(sel, "9");
				}					
				if(edit_port.isFocused()){
					int sel=edit_port.getSelectionStart();
					edit_port.getText().insert(sel, "9");
				}}});
		btn_0.setOnClickListener(new OnClickListener(){
			@Override
			public void onClick(View v) {
				if(edit_srvip.isFocused()){
					int sel=edit_srvip.getSelectionStart();
					edit_srvip.getText().insert(sel, "0");
				}					
				if(edit_port.isFocused()){
					int sel=edit_port.getSelectionStart();
					edit_port.getText().insert(sel, "0");
				}}});
		btn_dot.setOnClickListener(new OnClickListener(){
			@Override
			public void onClick(View v) {
				if(edit_srvip.isFocused()){
					int sel=edit_srvip.getSelectionStart();
					edit_srvip.getText().insert(sel, ".");
				}					
				if(edit_port.isFocused()){
					int sel=edit_port.getSelectionStart();
					edit_port.getText().insert(sel, ".");
				}}});
		btn_del.setOnClickListener(new OnClickListener(){
			@Override
			public void onClick(View v) {
				if(edit_srvip.isFocused()&&edit_srvip.length()>0){
					int sel=edit_srvip.getSelectionStart();
					edit_srvip.getText().delete(sel-1, sel);
				}					
				if(edit_port.isFocused()&&edit_port.length()>0){
					int sel=edit_port.getSelectionStart();
					edit_port.getText().delete(sel-1, sel);
				}}});




	}




	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		// Inflate the menu; this adds items to the action bar if it is present.
		getMenuInflater().inflate(R.menu.main, menu);
		return true;
	}




}
