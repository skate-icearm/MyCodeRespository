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
int powerFeedbackSensor = 0; // ���紫����

int main()
{
    WSADATA wsaData;
    SOCKET sock = INVALID_SOCKET;
    // ������һ���׽��ֱ��� sock���������ʼ��Ϊ INVALID_SOCKET����ʾ��Ч���׽��־����
    // �ں����Ĵ����У���ʹ������׽�������������ͨ�š�
    struct sockaddr_in server;
    // �洢�������ĵ�ַ��Ϣ���ڴ˽ṹ���У�sockaddr_in ��һ�����ڱ�ʾ IPv4 ��ַ�Ľṹ�塣
    // ����ṹ������˷������� IP ��ַ���˿ںŵ���Ϣ��
    char message[1024];

    // ��ʼ�� Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        printf("�޷���ʼ�� Winsock��������룺%d\n", WSAGetLastError());
        return 1;
    }

    // �����׽���
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET) // ��Ч���׽��־����
    {
        printf("�޷������׽��֡�������룺%d\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }

    printf("�׽����Ѵ�����\n");

    // ���÷�������ַ
    server.sin_addr.s_addr = inet_addr("127.0.0.1"); // ���ķ����� IP ��ַ
    server.sin_family = AF_INET;                     // IPv4��ַ�塣
    server.sin_port = htons(12345);                  // �˿ں�

    // ���ӵ�������
    if (connect(sock, (struct sockaddr *)&server, sizeof(server)) < 0)
    {
        printf("����ʧ�ܡ�������룺%d\n", WSAGetLastError());
        closesocket(sock);
        WSACleanup();
        return 1;
    }

    printf("�����ӵ���������\n");

    // ��ʼ�������������
    srand((unsigned int)time(NULL));

    // ģ�⴫�������ݲ����͵�������
    while (1)
    {
        time_t currentTime = time(NULL);   // long long 8���ֽ�
        int methaneLevel = rand() % 65536; // ģ�����ˮƽ��16 λ��4�ֽ�
        int temperature = rand() % 256;    // ģ���¶ȣ�8 λ��2�ֽ�
        int oxygenLevel = rand() % 256;    // ģ������ˮƽ��8 λ�� 2�ֽ�
        int fanState = rand() % 4;         // ģ����״̬��2 λ����λ�����������λ�Ǹ����  1�ֽ�

        sprintf(message, "TIME:%ld,METHANE:%d,TEMP:%d,OXYGEN:%d,FAN:%d,POWER:%d,powerFeedbackSensor:%d\n",
                (long)currentTime, methaneLevel, temperature, oxygenLevel, fanState, powerFeedbackSensor);
        send(sock, message, strlen(message), 0);

        int bytesRead = recv(sock, message, sizeof(message), 0);
        if (bytesRead == SOCKET_ERROR || bytesRead == 0)
        {
            printf("��������ʱ�������󡣴�����룺%d\n", WSAGetLastError());
            break;
        }
        cout << "���յ�������Ϣ" << endl;

        // ������յ�������
        message[bytesRead] = '\0'; // ����ַ�����ֹ��
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
            cout << "������ѿ���" << endl;
        else
            cout << "�������ѹر�" << endl;

        if (reply[0] == 1 || reply[0] == 3)
            cout << "���÷����ѿ���" << endl;
        else
            cout << "���÷����ѹر�" << endl;

        if (reply[4] == 2)
        {
            cout << "���¶Ⱥ�����Ũ�����ˣ�������ѹر�" << endl;
        }

        if (reply[1] == 1)
            cout << "�ѵ���ʱ��, ����������һ��" << endl;
        // cout << message << endl;
        // cout << "�����" << reply[0] << endl;
        // cout << "ʱ��У�飺" << reply[1] << endl;
        // cout << "�����豸��" << reply[2] << endl;
        // cout << "�����豸��" << reply[3] << endl;

        int relay_main = rand() % 5;
        int relay_trans = rand() % 5;
        int alarm1 = 0, alarm2 = 0;
        if (reply[2] == 1)
            cout << "�����豸����ͨ������" << endl;
        else
        {
            if (relay_main == 0)
            {
                alarm1 = 1;
                cout << "�����豸����ͨ������" << endl;
            }
            else
                cout << "�����豸�Ѿ��ϵ�" << endl;
        }

        if (reply[3] == 1)
            cout << "�����豸����ͨ������" << endl;
        else
        {
            if (relay_trans == 0)
            {
                alarm2 = 1;
                cout << "�����豸����ͨ������" << endl;
            }
            else
                cout << "�����豸�Ѿ��ϵ�" << endl;
        }

        if (reply[2] == 1)
            cout << "�ѵ���ʱ�ӣ�����������һ��" << endl;

        char back_message[1024];
        sprintf(back_message, "%d#%d",
                alarm1, alarm2);
        printf("--------------------------------�ָ���-------------------------------\n");
        send(sock, back_message, strlen(back_message), 0);

        Sleep(5000); // �ڷ�����һ������֮ǰ�ȴ� 2 ��
    }

    // ...�����ᵽ�������Ϊ������һ������ѭ����
    closesocket(sock);
    WSACleanup();
    return 0;
}
