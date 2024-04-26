#include <vector>
#include <string>
#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <thread>

using namespace std;

#pragma comment(lib, "Ws2_32.lib")
int powerFeedbackSensor = 0; // 馈电传感器

int main()
{
    WSADATA wsaData;
    SOCKET sock = INVALID_SOCKET;
    // 声明了一个套接字变量 sock，并将其初始化为 INVALID_SOCKET，表示无效的套接字句柄。
    // 在后续的代码中，将使用这个套接字来进行网络通信。
    struct sockaddr_in server;
    // 存储服务器的地址信息。在此结构体中，sockaddr_in 是一个用于表示 IPv4 地址的结构体。
    // 这个结构体包含了服务器的 IP 地址、端口号等信息。
    char message[1024];

    // 初始化 Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        printf("无法初始化 Winsock。错误代码：%d\n", WSAGetLastError());
        return 1;
    }

    // 创建套接字
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET) // 无效的套接字句柄。
    {
        printf("无法创建套接字。错误代码：%d\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }

    printf("套接字已创建。\n");

    // 设置服务器地址
    server.sin_addr.s_addr = inet_addr("127.0.0.1"); // 您的服务器 IP 地址
    server.sin_family = AF_INET;                     // IPv4地址族。
    server.sin_port = htons(12345);                  // 端口号

    // 连接到服务器
    if (connect(sock, (struct sockaddr *)&server, sizeof(server)) < 0)
    {
        printf("连接失败。错误代码：%d\n", WSAGetLastError());
        closesocket(sock);
        WSACleanup();
        return 1;
    }

    printf("已连接到服务器。\n");

    // 初始化随机数生成器
    srand((unsigned int)time(NULL));

    // 模拟传感器数据并发送到服务器
    while (1)
    {
        time_t currentTime = time(NULL);   // long long 8个字节
        int methaneLevel = rand() % 65536; // 模拟甲烷水平（16 位）4字节
        int temperature = rand() % 256;    // 模拟温度（8 位）2字节
        int oxygenLevel = rand() % 256;    // 模拟氧气水平（8 位） 2字节
        int fanState = rand() % 4;         // 模拟风机状态（2 位）高位是主风机，低位是副风机  1字节

        sprintf(message, "TIME:%ld,METHANE:%d,TEMP:%d,OXYGEN:%d,FAN:%d,POWER:%d,powerFeedbackSensor:%d\n",
                (long)currentTime, methaneLevel, temperature, oxygenLevel, fanState, powerFeedbackSensor);
        send(sock, message, strlen(message), 0);

        int bytesRead = recv(sock, message, sizeof(message), 0);
        if (bytesRead == SOCKET_ERROR || bytesRead == 0)
        {
            printf("接收数据时发生错误。错误代码：%d\n", WSAGetLastError());
            break;
        }
        cout << "已收到主机消息" << endl;

        // 处理接收到的数据
        message[bytesRead] = '\0'; // 添加字符串终止符
        vector<int> reply;
        int begin = 0;
        for (int i = 0; i < bytesRead; i++)
        {
            if (message[i] == '#')
            {
                string str;
                for (int j = begin; j < i; j++)
                    str += message[j];
                reply.push_back(stoi(str));
                begin = i + 1;
            }
        }

        if (reply[0] == 2 || reply[0] == 3)
            cout << "主风机已开启" << endl;
        else
            cout << "主风扇已关闭" << endl;

        if (reply[0] == 1 || reply[0] == 3)
            cout << "备用风扇已开启" << endl;
        else
            cout << "备用风扇已关闭" << endl;

        if (reply[4] == 2)
        {
            cout << "因温度和氧气浓度适宜！副风机已关闭" << endl;
        }

        if (reply[1] == 1)
            cout << "已调整时钟, 与主机保持一致" << endl;
        // cout << message << endl;
        // cout << "风机：" << reply[0] << endl;
        // cout << "时间校验：" << reply[1] << endl;
        // cout << "开采设备：" << reply[2] << endl;
        // cout << "传输设备：" << reply[3] << endl;

        int relay_main = rand() % 5;
        int relay_trans = rand() % 5;
        int alarm1 = 0, alarm2 = 0;
        if (reply[2] == 1)
            cout << "开采设备正在通电运行" << endl;
        else
        {
            if (relay_main == 0)
            {
                alarm1 = 1;
                cout << "开采设备正在通电运行" << endl;
            }
            else
                cout << "开采设备已经断电" << endl;
        }

        if (reply[3] == 1)
            cout << "传输设备正在通电运行" << endl;
        else
        {
            if (relay_trans == 0)
            {
                alarm2 = 1;
                cout << "传输设备正在通电运行" << endl;
            }
            else
                cout << "传输设备已经断电" << endl;
        }

        if (reply[2] == 1)
            cout << "已调整时钟，保持与主机一致" << endl;

        char back_message[1024];
        sprintf(back_message, "%d#%d",
                alarm1, alarm2);
        printf("--------------------------------分割线-------------------------------\n");
        send(sock, back_message, strlen(back_message), 0);

        Sleep(5000); // 在发送下一组数据之前等待 2 秒
    }

    // ...（不会到达这里，因为上面是一个无限循环）
    closesocket(sock);
    WSACleanup();
    return 0;
}
