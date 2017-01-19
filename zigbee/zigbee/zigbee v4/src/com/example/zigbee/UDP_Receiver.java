package com.example.zigbee;

import java.io.IOException;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.SocketException;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.List;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import android.os.Handler;
import android.util.Log;
import android.widget.TextView;
import android.widget.ToggleButton;

public class UDP_Receiver implements Runnable

{
	private static final int PORT = 6000;
	private byte[] msg = new byte[1024];
	private boolean life = true;
	private TextView text_rec,text_temp;
	private ToggleButton tgbtn_light,tgbtn_fan,tgbtn_egg;
	
	public Handler mHandler;
	
	public final static int FLAG_ERR=0;
	public final static int FLAG_LIGHT_ON=1;
	public final static int FLAG_LIGHT_OFF=2;
	public final static int FLAG_SRV_OK=3;
	public final static int FLAG_ZSRV_OK=4;
	public final static int FLAG_ZEND_OK=5;
	public final static int FLAG_TEMP=6;
	public final static int FLAG_REC=7;
	public final static int FLAG_FAN_ON=8;
	public final static int FLAG_FAN_OFF=9;
	public final static int FLAG_FAN_OK=10;
	public final static int FLAG_EGG_ON=11;
	public final static int FLAG_EGG_OFF=12;
	public final static int FLAG_EGG_OK=13;
	
	public final static String Srv_OK="SOK";
	public final static String Zigbee_Srv_OK="ZSOK";
	public final static String Zigbee_End_OK="ZEOK";
	public final static String Light_On_OK="LONOK";
	public final static String Light_Off_OK="LOFFOK";
	
	public final static String Zigbee_Egg_OK="EEOK";
	public final static String Egg_On_OK="EONOK";
	public final static String Egg_Off_OK="EOFFOK";
	public final static String Zigbee_Fan_OK="FEOK";
	public final static String Fan_On_OK="FONOK";
	public final static String Fan_Off_OK="FOFFOK";
	
	public final static String Temp_Val="\\d{1,4}";
	public static boolean isLOn=false;
	public static boolean isFOn=false;
	public static boolean isEOn=false;
	
	public UDP_Receiver(Handler mHandler) {
		this.mHandler=mHandler;
		
	}
	
	public class str_addr_BUF
	{
		public String addr;
		public String str;
	}
	
	public boolean linktest=false;
	
	
	public void msgproc(String msg_rec,String addr)
	{
		String str_rec="";
		String text_rec="";
		int flag=FLAG_REC;
		Pattern pattern = Pattern.compile("\\(("+Srv_OK+"|"+Zigbee_Srv_OK+"|"+Zigbee_End_OK+"|"+Zigbee_Egg_OK+"|"+Zigbee_Fan_OK+"|"+
				Light_On_OK+"|"+Light_Off_OK+"|"+Fan_Off_OK+"|"+Egg_Off_OK+"|"+Fan_On_OK+"|"+Egg_On_OK+"|"+Temp_Val+"|)\\)");
		Matcher matcher = pattern.matcher(msg_rec);
		while (matcher.find()) 
			str_rec = matcher.group();
		pattern=Pattern.compile("\\w{1,}");
		matcher = pattern.matcher(str_rec);
		while (matcher.find()) 
			str_rec= matcher.group();
			
		if(str_rec.equals(Light_On_OK)){
			str_rec="��״̬:��";
			flag=FLAG_LIGHT_ON;	
			isLOn=true;
			
		}
		if(str_rec.equals(Light_Off_OK)){
			str_rec="��״̬:�ر�";
			flag=FLAG_LIGHT_OFF;	
			isLOn=false;
		}
		
		
		if(str_rec.equals(Fan_On_OK)){
			str_rec="����״̬:��";
			flag=FLAG_FAN_ON;	
			isFOn=true;
			
		}
		if(str_rec.equals(Fan_Off_OK)){
			str_rec="����״̬:�ر�";
			flag=FLAG_FAN_OFF;	
			isFOn=false;
		}
		if(str_rec.equals(Egg_On_OK)){
			str_rec="ե֭״̬:��";
			flag=FLAG_EGG_ON;	
			isEOn=true;
			
		}
		if(str_rec.equals(Egg_Off_OK)){
			str_rec="ե֭״̬:�ر�";
			flag=FLAG_EGG_OFF;	
			isEOn=false;
		}
		
		
		if(str_rec.equals(Srv_OK)){
			str_rec="��������ת������,��ȷ��Zigbeeģ��״̬";
			flag=FLAG_SRV_OK;			
		}
		if(str_rec.equals(Zigbee_Srv_OK)){
			str_rec="ZigbeeЭ��������,��ȷ��Zigbee�ն�״̬";
			flag=FLAG_ZSRV_OK;			
		}
		if(str_rec.equals(Zigbee_End_OK)){
			str_rec="̨���ն�����";
			flag=FLAG_ZEND_OK;			
		}
		if(str_rec.equals(Zigbee_Egg_OK)){
			str_rec="ե֭�ն�����";
			flag=FLAG_EGG_OK;			
		}
		if(str_rec.equals(Zigbee_Fan_OK)){
			str_rec="�����ն�����";
			flag=FLAG_FAN_OK;			
		}
		if(flag==FLAG_REC){
			pattern=Pattern.compile(Temp_Val);
			matcher=pattern.matcher(str_rec);
			if(matcher.find()){			
				int i=Integer.parseInt(str_rec);
				float t=(float)i/100;
				str_rec="��ǰ�ڵ��¶�:"+t+"��";
				flag=FLAG_TEMP;			
			}
		}
		if(flag==FLAG_REC)str_rec=msg_rec;
		str_addr_BUF sab=new str_addr_BUF();
		sab.addr=addr;
		sab.str=str_rec;
		mHandler.obtainMessage(flag,sab).sendToTarget();
		
	}
	
	
	public boolean isLife() {
		return life;
	}
	
	
	
	public void setLife(boolean life) {
		this.life = life;
	}

	@Override
	public void run() {
		DatagramSocket dSocket = null;
		DatagramPacket dPacket = new DatagramPacket(msg, msg.length);
		try {
			dSocket = new DatagramSocket(PORT);
			while (life) {
				
				try {
					dSocket.receive(dPacket);	
					if(dPacket.getLength()>0)
					{
						String msg_rec=(new String(dPacket.getData(),0,dPacket.getLength()-1))+"";
						msgproc(msg_rec,dPacket.getAddress().getHostAddress());					
					}
					
					
				} catch (IOException e) {
					e.printStackTrace();					
				}
			}
		} catch (SocketException e) {
			e.printStackTrace();
			String s;
			SimpleDateFormat sdf=new SimpleDateFormat("hh:mm:ss");  
			String str_time=sdf.format(new java.util.Date());
			s="["+str_time+"][�����߳���������,�����ֻ����ú���������]\n";
			mHandler.obtainMessage(FLAG_ERR,s).sendToTarget();
		}

	}

}
