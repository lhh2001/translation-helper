/*************************

main.cpp

Created by Li Henghao on 21st, April, 2020.
Last modified on 29th, April, 2020.

*************************/
#include <Windows.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <cstring>
#include <iostream>
#include <fstream>
#include <future>
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
    bool flag; //判断窗口是否已经最小化
    
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

    int lookUpDic(const char* str, int n)
    {
        if (n > 50)
        {
            return -1;
        }
        string order = "getWord ";
        order += str;
        return system(order.c_str());
    }

    void printWord(int re)
    {
        if (re == 3)
        {
            cout << "单词未在词典中找到" << endl;
            return;
        }
        if (re == -1)
        {
            cout << "单词过长!" << endl;
            return;
        }
        
        json word, root;
        if ((re & 1) == 0) //找到了原词
        {
            ifstream i("word.json");
            i >> word;
            i.close();
            cout << "------------原词------------" << endl;
            printJsonWord(word);
        }
        if ((re & 2) == 0) //找到了词根
        {
            ifstream i("root.json");
            i >> root;
            i.close();
            if (word != root)
            {
                cout << endl;
                cout << "------------词根------------" << endl;
                printJsonWord(root);
            }
        }
    }

    void callBaiduAPI(string order)
    {
        system(order.c_str());
    }

    void printBaidu()
    {
        json transResult;
        ifstream i("transResult.json");
        i >> transResult;
        i.close();

        cout << endl << "----------百度翻译----------" << endl;
        if (!transResult["error_code"].is_null())
        {
            cout << "错误代码: " << transResult["error_code"] << endl;
            cout << "错误信息: " << transResult["error_msg"] << endl;
            return;
        }

        cout << "原文: " << transResult["trans_result"][0]["src"] << endl;
        cout << "译文: " << transResult["trans_result"][0]["dst"] << endl;
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
                future<int> futWord;
                future<void> futBaidu;
                bool isUseFutWord = false;
                bool isUseFutBaidu = false;

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
                    isUseFutWord = true;
                    futWord = async(lookUpDic, this, order.c_str(), order.size());
                }
                if (settings["isUseBaiduAPI"] == true)
                {
                    isUseFutBaidu = true;
                    order = "httpGet " + order;
                    futBaidu = async(callBaiduAPI, this, order);
                }

                if (isUseFutWord)
                {
                    printWord(futWord.get());
                }
                
                if (isUseFutBaidu)
                {
                    futBaidu.get();
                    printBaidu();
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
