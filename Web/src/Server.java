/**
 * Created by Administrator on 2018/6/16 0016.
 */

import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.io.PrintStream;
import java.net.ServerSocket;
import java.net.Socket;

//是一个能够接受其他通信实体请求的类
//用一个while循环可以同时响应多个客户端的请求
//服务器监听对应端口的输入
//创建一个线程，用线程创建一个套接字
public class Server {
    public static void main(String[] args) throws Exception {
        ServerSocket serverSocket = new ServerSocket(3696);
        System.out.println("服务器正在等待客户端的连接请求----");
        while(true) {
            Socket socket= serverSocket.accept();
            ServerThread serverThread = new ServerThread(socket);
            serverThread.start();
        }
    }
}

//服务器线程类
class ServerThread extends Thread {
    Socket socket;
    public ServerThread(Socket socket) {
        this.socket= socket;
    }
    public void run() {
        File textFile = new File("G:\\Web\\html\\txt\\test.txt");
        File pictureFile = new File("G:\\Web\\html\\img\\logo.jpg");
        File testHtml = new File("G:\\Web\\html\\test.html");
        File noimgHtml = new File("G:\\Web\\html\\noimg.html");
        File success = new File("G:\\Web\\html\\success.html");
        File failure = new File("G:\\Web\\html\\failure.html");
        String str = "";
        String response = "";
        int count = 0;
        try {
            PrintStream writer = new PrintStream(socket.getOutputStream());
            //while(true) {
            while (count == 0) {
                count = socket.getInputStream().available();
            }
            byte[] b = new byte[count];
            socket.getInputStream().read(b);
            //System.out.println(new String(b));
            str = new String(b);
            System.out.println("来自客户端的数据：" + str);
            String[] s1 = str.trim().split(" ");
            if(s1[0].equals("GET")) {
                if(s1[1].equals("/dir/txt/test.txt")) {
                    FileInputStream fileInputStream = new FileInputStream(textFile);
                    writer.println("HTTP/1.1 200/OK");
                    writer.println("Content-Type: text/html");
                    writer.println("Content-Length: " + textFile.length());
                    writer.println();
                    byte[] buf = new byte[fileInputStream.available()];
                    fileInputStream.read(buf);
                    writer.write(buf);
                    writer.close();
                    socket.close();
                }
                else if(s1[1].equals("/dir/img/logo.jpg")) {
                    FileInputStream fileInputStream = new FileInputStream(pictureFile);
                    writer.println("HTTP/1.1 200/OK");
                    writer.println("Content-Type: image/jpeg");
                    writer.println("Content-Length: " + pictureFile.length());
                    writer.println();
                    byte[] buf = new byte[fileInputStream.available()];
                    fileInputStream.read(buf);
                    writer.write(buf);
                    writer.close();
                    socket.close();
                }
                else if(s1[1].equals("/dir/test.html")) {
                    FileInputStream fileInputStream = new FileInputStream(testHtml);
                    writer.println("HTTP/1.1 200/OK");
                    writer.println("Content-Type: text/html");
                    writer.println("Content-Length: " + testHtml.length());
                    writer.println();
                    byte[] buf = new byte[fileInputStream.available()];
                    fileInputStream.read(buf);
                    writer.write(buf);
                    writer.close();
                    socket.close();
                }
                else if(s1[1].equals("/dir/noimg.html")) {
                    FileInputStream fileInputStream = new FileInputStream(noimgHtml);
                    writer.println("HTTP/1.1 200/OK");
                    writer.println("Content-Type: text/html");
                    writer.println("Content-Length: " + noimgHtml.length());
                    writer.println();
                    byte[] buf = new byte[fileInputStream.available()];
                    fileInputStream.read(buf);
                    writer.write(buf);
                    writer.close();
                    socket.close();
                }
                else {
                    writer.println("HTTP/1.1 404/Bad Request");
                    writer.println("Content-Type: text/html");
                    writer.println("Content-Length: 0");
                    writer.println();
                    writer.close();
                    socket.close();
                }
            }
            else if (s1[0].equals("POST")) {
                if (s1[1].equals("/dir/dopost")) {
                    String[] s2 = s1[s1.length - 1].split("\n");
                    String[] s3 = s2[s2.length - 1].split("&");
                    if (s3[0].equals("login=3696") && s3[1].equals("pass=3696")) {
                        FileInputStream fileInputStream = new FileInputStream(success);
                        writer.println("\nHTTP/1.1 200/OK");
                        writer.println("Content-Type: text/html");
                        writer.println("Content-Length: " + success.length());
                        writer.println();
                        byte[] buf = new byte[fileInputStream.available()];
                        fileInputStream.read(buf);
                        writer.write(buf);
                        writer.close();
                        socket.close();
                    }
                    else {
                        FileInputStream fileInputStream = new FileInputStream(failure);
                        writer.println("\nHTTP/1.1 200/OK");
                        writer.println("Content-Type: text/html");
                        writer.println("Content-Length: " + failure.length());
                        writer.println();
                        byte[] buf = new byte[fileInputStream.available()];
                        fileInputStream.read(buf);
                        writer.write(buf);
                        writer.close();
                        socket.close();
                    }
                }
                else {
                    writer.println("HTTP/1.1 404/Bad Request");
                    writer.println("Content-Type: text/html");
                    writer.println("Content-Length: 0");
                    writer.println();
                    writer.close();
                    socket.close();
                }
            }
            else {
                writer.println("HTTP/1.1 404/Bad Request");
                writer.println("Content-Type: text/html");
                writer.println("Content-Length: 0");
                writer.println();
                writer.close();
                socket.close();
            }
        }
        catch(IOException e) {
            e.printStackTrace();
        }
    }
}