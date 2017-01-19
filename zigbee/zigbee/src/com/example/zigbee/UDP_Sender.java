package com.example.zigbee;

import java.io.IOException;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;
import java.net.SocketException;
import java.net.UnknownHostException;
import java.text.SimpleDateFormat;

public class UDP_Sender {
	private int SERVER_PORT;
	private DatagramSocket dSocket = null;
	private String msg;
	private String addr;
	

	public UDP_Sender(String msg,String addr,int port) {
		super();
		this.msg = msg;
		this.addr=addr;
		this.SERVER_PORT=port;
	}


	public String makeStr(String str)
	{
		String s;
		SimpleDateFormat sdf=new SimpleDateFormat("hh:mm:ss");  
		String str_time=sdf.format(new java.util.Date());
		s="["+str_time+"][To "+addr+":"+SERVER_PORT+"]\n["+str+"]\n";
		
		return s; 
	}
	
	
	public String send() 
	{
		int msg_len = msg == null ? 0 : msg.length();
		if(msg_len>0)
		{

			StringBuilder sb = new StringBuilder();
			InetAddress local = null;

			try {
				local = InetAddress.getByName(addr); 

			} catch (UnknownHostException e) {

				e.printStackTrace();
				return "未找到服务器,请检查设置!";
			}

			try {
				dSocket = new DatagramSocket(); 
			} catch (SocketException e) {
				e.printStackTrace();
				return "服务器连接错误,请检查设置!";
			}

			DatagramPacket dPacket = new DatagramPacket(msg.getBytes(), msg_len,local, SERVER_PORT);
			try {
				dSocket.send(dPacket);

			} catch (IOException e) {
				e.printStackTrace();
				return "消息发送错误,请检查设置!";
			}
			dSocket.close();
			return "发送成功";
		}
		return "无效的消息,请检查!";
		
	}
}
