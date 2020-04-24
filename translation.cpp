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
    if (hData == NULL)
    {
        return "__ERROR__";
    }
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
    bool flag;
    
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

    void printJsonWord(json j)
    {
        string s = j["word"];
        printf(s.c_str());
        cout << '\t' << "[";
        s = j["phonetic"];
        printf(s.c_str());
        cout << "]" << endl;
        s = j["definition"];
        printf(s.c_str());
        cout << endl;
        s = j["translation"];
        printf(s.c_str());
        cout << endl;
    }

    void lookUpDic(const char* str, int n)
    {
        if (n > 50)
        {
            cout << "单词过长" << endl;
            return;
        }
        string order = "getWord ";
        order += str;
        int re = system(order.c_str());
        if (re == 3)
        {
            cout << "单词未在词典中找到" << endl;
            return;
        }
        json word, root;
        if ((re & 1) == 0) //找到了原词
        {
            ifstream i("word.json");
            i >> word;
            i.close();
            cout << "原词: ";
            printJsonWord(word);
        }
        if ((re & 2) == 0) //找到了词根
        {
            ifstream i("root.json");
            i >> root;
            i.close();
            if (word != root)
            {
                cout << endl << "词根: ";
                printJsonWord(root);
            }
        }
    }

    void setFontFace()
    {
        HANDLE stdOut = GetStdHandle(STD_OUTPUT_HANDLE);
        CONSOLE_FONT_INFOEX cfie;
        ZeroMemory(&cfie, sizeof(cfie));
        cfie.cbSize = sizeof(cfie);
        cfie.dwFontSize.Y = 18;
        lstrcpyW(cfie.FaceName, L"Consolas");
        SetCurrentConsoleFontEx(stdOut, false, &cfie);
    }
    
public:
    TranslateHelper() : flag(false)
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
        
        setFontFace();
        
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
                flag = false;
                system("cls");
                string order = getClipboardStr();
                if (order == "__ERROR__")
                {
                    Sleep(5);
                    continue;
                }
                ShowWindow(hWnd, SW_RESTORE);
                SetWindowPos(hWnd, HWND_TOPMOST,
                settings["paddingLeft"],
                settings["paddingTop"],
                settings["width"],
                settings["height"], SWP_SHOWWINDOW);
                
                if (settings["isUseDicv"] == true)
                {
                    lookUpDic(order.c_str(), order.size());
                }
                if (settings["isUseBaiduAPI"] == true)
                {
                    cout << endl << "----------百度翻译----------" << endl;
                    order = "httpGet " + order;
                    system(order.c_str());
                }
                if (settings["isPause"])
                {
                    mouse_event(
                    MOUSEEVENTF_ABSOLUTE |  MOUSEEVENTF_MOVE,
                    ((int)settings["paddingLeft"] + (int)settings["width"] / 2) * 65535 / clientWidth,
                    ((int)settings["paddingTop"] + 10) * 65535 / clientHeight,
                    0, 0);
                    mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
                    mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
                    system("pause");
                }
                else
                {
                    Sleep(settings["showInterval"]);
                }
                t = clock();
            }
            
            if (flag == false)
            {
                ShowWindow(hWnd, SW_MINIMIZE);
            }
            if (!settings["showWindow"])
            {
                flag = true;
                ShowWindow(hWnd, SW_HIDE);
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
        system("start %windir%\\system32\\notepad.exe settings.json");
        system("pause");
        ifstream i("settings.json");
        i >> settings;
        i.close();
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
