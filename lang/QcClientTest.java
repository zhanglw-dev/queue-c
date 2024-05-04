package org.qc;

import java.util.Random;


public class QcClientTest {
    private static String ip = "127.0.0.1";
    private static int port = 5555;
    private static String queueName = "queue01";
    private static int cycles = 100000000;
    private static Random random = new Random();

    public QcClientTest(){
    }

    static class TestGet extends Thread{
        private QcClient qcClient;
        public TestGet(QcClient qcClient){
            this.qcClient = qcClient;
        }
        @Override
        public void run(){
            for(int i = 0; i< cycles; i++){
                int size = random.nextInt(4096);
                if(size<=0) size=100;
                try {
                    byte[] data = new byte[size];
                    qcClient.messagePut(queueName, new byte[size], -1);
                    //System.out.println("send data i=" + i + " len=" + data.length);
                } catch (Exception e) {
                    e.printStackTrace();
                }
            }
        }
    }

    static class TestPut extends Thread{
        private QcClient qcClient;
        public TestPut(QcClient qcClient){
            this.qcClient = qcClient;
        }
        @Override
        public void run(){
            for(int i = 0; i< cycles; i++){
                try {
                    byte[] data = qcClient.messageGet(queueName, -1);
                    //System.out.println("received data i=" + i + " len=" + data.length);
                } catch (Exception e) {
                    e.printStackTrace();
                }
            }
        }
    }

    public static void main(String[] args) {
        System.out.println("Start Test!");
        QcClient putClient1 = new QcClient(ip, port);
        QcClient putClient2 = new QcClient(ip, port);
        QcClient putClient3 = new QcClient(ip, port);

        QcClient getClient1 = new QcClient(ip, port);
        QcClient getClient2 = new QcClient(ip, port);
        QcClient getClient3 = new QcClient(ip, port);

        try{
            putClient1.connect();
            putClient2.connect();
            putClient3.connect();

            getClient1.connect();
            getClient2.connect();
            getClient3.connect();

            TestPut testPut1 = new TestPut(putClient1);
            TestPut testPut2 = new TestPut(putClient2);
            TestPut testPut3 = new TestPut(putClient3);

            TestGet testGet1 = new TestGet(getClient1);
            TestGet testGet2 = new TestGet(getClient2);
            TestGet testGet3 = new TestGet(getClient3);

            testPut1.start();
            testPut2.start();
            testPut3.start();
            System.out.println("Test Put started.");

            testGet1.start();
            testGet2.start();
            testGet3.start();
            System.out.println("Test Get started.");

            testPut1.join();
            testPut2.join();
            testPut3.join();

            testGet1.join();
            testGet2.join();
            testGet3.join();

            putClient1.disconnect();
            putClient2.disconnect();
            putClient3.disconnect();

            getClient1.disconnect();
            getClient2.disconnect();
            getClient3.disconnect();

        }catch (Exception ex){
            ex.printStackTrace();
            System.exit(-1);
        }

        System.out.println("Test Finished!");
    }
}
