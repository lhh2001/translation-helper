#include <Windows.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <cstring>
#include <iostream>
#include <fstream>
#include <wingdi.h> //O2 -lgdi32
#include "include/json.hpp"

using namespace std;
using json = nlohmann::json;

string getClipboardStr()
{
    char *pData;
    OpenClipboard(NULL);
    HANDLE hData = GetClipboardData(CF_TEXT);
    assert(hData != NULL);
    pData = (char *)GlobalLock(hData);
    string url = pData;
    GlobalUnlock(hData);
    CloseClipboard();
    return url;
}

class TranslateHelper
{
private:
    time_t t;
    json settings;
    HWND hWnd;
    int clientWidth;
    int clientHeight;
    
    float getInterval()
    {
        return (float)(clock() - t) / CLOCKS_PER_SEC;
    }

    bool check()
    {
        for (auto iter = settings["keys"].begin(); iter < settings["keys"].end(); iter++)
        {
            if (!GetAsyncKeyState(*iter))
            {
                return false;
            }
        }
        return true;
    }

public:
    TranslateHelper()
    {
        system("color f0"); //控制台颜色改变
        system("chcp 65001"); //UTF-8 编码

        t = clock();
        SetConsoleTitle("TranslateHelper");
        Sleep(100);
        hWnd = FindWindow(NULL, "TranslateHelper");
        if (hWnd == NULL)
        {
            cout << "程序初始化失败!" << endl;
            exit(0);
        }
        
        HDC hdc = GetDC(NULL);
        clientWidth = GetDeviceCaps(hdc, HORZRES);
        clientHeight = GetDeviceCaps(hdc, VERTRES);
        ReleaseDC(NULL, hdc);
        
        ifstream i("settings.json");
        i >> settings;
        i.close();
    }
    
    void run()
    {
        while (true)
        {
            if (check())
            {
                system("cls");
                string order = getClipboardStr();
                order = "python httpgets.py " + order;
                SetWindowPos(hWnd, HWND_TOPMOST,
                settings["paddingLeft"],
                settings["paddingTop"],
                settings["width"],
                settings["height"], SWP_SHOWWINDOW);
                system(order.c_str());
                if (settings["isPause"])
                {
                    mouse_event(
                    MOUSEEVENTF_ABSOLUTE |  MOUSEEVENTF_MOVE,
                    ((int)settings["paddingLeft"] + (int)settings["width"] / 2) * 65535 / clientWidth,
                    ((int)settings["paddingTop"] + (int)settings["height"] / 2) * 65535 / clientHeight,
                    0, 0);
                    mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
                    Sleep(10);
                    mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
                    system("pause");
                }
                else
                {
                    Sleep(settings["showInterval"]);
                }
                t = clock();
            }
            
            if (!settings["showWindow"])
            {
                ShowWindow(hWnd, SW_HIDE);
            }
            else
            {
                ShowWindow(hWnd, SW_MINIMIZE);
            }
            
            if (getInterval() > settings["closeAfterSecs"])
            {
                ShowWindow(hWnd, SW_SHOW);
                SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE|SWP_NOMOVE);
                cout << "长时间未操作, 程序自动结束!" << endl;
                system("pause");
                exit(0);
            }
            Sleep(5);
        }
    }

    void set()
    {
        cout << "敬请期待" << endl;
        system("pause");
    }
};

int main()
{
    TranslateHelper myHelper;
    while (true)
    {
        string input;
        system("cls");
        cout << "欢迎使用翻译小助手!" << endl;
        cout << "输入 run 运行小助手" << endl;
        cout << "输入 set 更改设置" << endl;
        cout << "输入 end 结束程序" << endl;
        cin >> input;
        if (input == "run")
        {
            myHelper.run();
        }
        else if (input == "set")
        {
            myHelper.set();
        }
        else if (input == "end")
        {
            break;
        }
    }
    return 0;
}
