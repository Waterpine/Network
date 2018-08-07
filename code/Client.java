/**
 * Created by Administrator on 2018/6/16 0016.
 */

import java.io.PrintStream;
import java.net.Socket;
import java.util.Scanner;

public class Client {
    public static void main(String[] args) {
        Scanner in = new Scanner(System.in);
        String str = "";
        try {
            Socket socket = new Socket("127.0.0.1", 3696);
            System.out.println("客户端已经开启----");
            PrintStream printStream = new PrintStream(socket.getOutputStream()); //将客户端套接字的输出流用printStream包装起来，类似于C语言中的fprintf类型转换
            while(true) {
                System.out.println("Input a number to choose the action:");
                System.out.println("1.Get Text");
                System.out.println("2.Get Picture");
                System.out.println("3.Post login and pass");
                System.out.println("4.Quit");
                int num = in.nextInt();
                switch (num) {
                    case 1:
                        str = "GET /test.txt HTTP/1.1\n";
                        str = str + "Host: 127.0.0.1\n";
                        str = str + "Content-Type: text/html\n";
                        str = str + "Content-Length: ";
                        str = str + "128";
                        str = str + "\n\n";
                        //System.out.println(str);
                        printStream.println(str);
                        break;
                    case 2:
                        str = "GET /logo.jpg HTTP/1.1\n";
                        str = str + "Host: 127.0.0.1\n";
                        str = str + "Content-Type: text/html\n";
                        str = str + "Content-Length: ";
                        str = str + "128";
                        str = str + "\n\n";
                        //System.out.println(str);
                        printStream.println(str);
                        break;
                    case 3:
                        str = "GET /ymnets/p/6255674.html HTTP/1.1\n";
                        str = str + "Host: 127.0.0.1\n";
                        str = str + "Content-Type: text/html\n";
                        str = str + "Content-Length: ";
                        str = str + "128";
                        str = str + "\n\n";
                        //System.out.println(str);
                        printStream.println(str);
                        break;
                    case 4:
                        printStream.close();
                        socket.close();
                        break;
                }
                if (num == 4) {
                    break;
                }
                //System.out.print("请输入需要发送到服务器的内容：");
                //Scanner scanner = new Scanner(System.in);
                //String str = scanner.nextLine();
                //printStream.println(str); //把控制台输入的内容送入被printstream类包装的输出流里面
            }
            //printStream.close();//关闭输出流包装
            //socket.close();//关闭socket套接字，已经传完数据，才能关闭
        }
        catch(Exception e){
            e.printStackTrace();
        }
    }
}