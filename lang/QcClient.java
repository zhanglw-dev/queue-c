package org.qc;

import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.net.InetSocketAddress;
import java.net.Socket;
import java.net.SocketAddress;


public class QcClient {
    private final String ip;
    private final int port;
    private final Socket socket;
    private DataInputStream input;
    private DataOutputStream output;

    public QcClient(String ip, int port){
        this.ip = ip;
        this.port = port;
        this.socket = new Socket();
    }

    public void connect() throws Exception{
        SocketAddress socketAddress = new InetSocketAddress(ip, port);
        try {
            socket.setTcpNoDelay(true);
            socket.setSendBufferSize(1024*1024*2);
            socket.setReceiveBufferSize(1024*1024*2);
            socket.connect(socketAddress);
        }catch (Exception ex){
            System.out.println("failed to connect node " + ip + ":" + port + ", err=" + ex.toString());
            throw ex;
        }

        try{
            input  = new DataInputStream(socket.getInputStream());
            output = new DataOutputStream(socket.getOutputStream());
        }catch (Exception ex){
            System.out.println("exception happened getting socket (input/output)stream, err=" + ex.toString());
            try{
                socket.close();
            }catch (Exception e){
                System.out.println("exception happened closing socket, err=" + e.toString());
            }
            throw ex;
        }
    }

    public void disconnect(){
        try{
            socket.close();
        }catch (Exception ex){
            System.out.println("failed to close connection " + ip + ":" + port + ", err=" + ex.toString());
        }
    }

    public void messagePut(String queueName, byte[] data, int mill_sec) throws Exception{
        byte[] q_name = queueName.getBytes();
        if(q_name.length > 32){
            throw new Exception("length of queueName>32 !");
        }
        byte[] queue_name = new byte[32];
        System.arraycopy(q_name, 0, queue_name, 0, q_name.length);

        if(null==data || data.length==0){
            throw new Exception("data can not be null!");
        }
        int msg_len = data.length;

        /*Send Request*/
        //Header
        output.writeShort(5555);    //protocol
        output.writeShort(1);       //version
        output.writeShort(120);     //type: QC_TYPE_MSGPUT
        output.writeInt(0);         //package_sn
        output.writeInt((32+4+2+4) + msg_len);  //body_len
        output.write(new byte[16]);    //reserved
        //MsgPut
        output.write(queue_name);      //qname
        output.writeInt(mill_sec);     //wait_msec
        output.writeShort(1);       //msg_prioriy
        output.writeInt(msg_len);      //msg_len
        //Msg
        output.write(data);            //message data
        output.flush();

        /*Receive Reply*/
        //Header
        int protocol = input.readShort();
        int version = input.readShort();
        int type = input.readShort();
        int pack_sn = input.readInt();
        int body_len = input.readInt();
        byte[] reserve = new byte[16];
        input.readFully(reserve, 0 , 16);
        //Reply
        int result = input.readInt();
        int _msg_len = input.readInt();

        if(result != 0){
            if(result == 100){
                throw new Exception("time out!");
            }
            else{
                throw new Exception("err result:" + result);
            }
        }
    }

    public byte[] messageGet(String queueName, int mill_sec) throws Exception{
        byte[] q_name = queueName.getBytes();
        if(q_name.length > 32){
            throw new Exception("length of queueName>32 !");
        }
        byte[] queue_name = new byte[32];
        System.arraycopy(q_name, 0, queue_name, 0, q_name.length);

        /*Send Request*/
        //Header
        output.writeShort(5555);
        output.writeShort(1);
        output.writeShort(121);
        output.writeInt(0);
        output.writeInt(32+4);
        output.write(new byte[16]);
        //MsgGet
        output.write(queue_name);
        output.writeInt(mill_sec);
        output.flush();

        /*Receive Reply*/
        //Header
        int protocol = input.readShort();
        int version = input.readShort();
        int type = input.readShort();
        int pack_sn = input.readInt();
        int body_len = input.readInt();
        byte[] reserve = new byte[16];
        input.readFully(reserve, 0 , 16);
        //Reply
        int result = input.readInt();
        int msg_len = input.readInt();

        if(result != 0){
            if(result == 100){
                throw new Exception("time out!");
            }
            else{
                throw new Exception("err result:" + result);
            }
        }

        byte[] data = new byte[msg_len];
        input.readFully(data);

        return data;
    }

    public static void main(String[] args) {
        System.out.println("Start Test!");

            String message = "Hello Qc!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!";
            QcClient qcClient = new QcClient("127.0.0.1", 5555);
            try {
                qcClient.connect();
                while(true) {
                    qcClient.messagePut("queue01", message.getBytes(), -1);
                    byte[] data = qcClient.messageGet("queue01", -1);
                    //System.out.println(new String(data));
                }
            } catch (Exception ex) {
                ex.printStackTrace();
            }
    }
}
