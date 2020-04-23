# 翻译小助手

2020.4.22 by lhh

## 语言

1. C++11 （编译时需要附加`-O2 -lgdi32`指令），使用[JSON for Modern C++](https://nlohmann.github.io/json/)的单文件库解析JSON文件
2. Python 3.7

## 功能

1. 实时查询选中单词，离线查询，词典为`ecdict.db`，词典及词典接口为github用户[skywind3000](https://github.com/skywind3000/ECDICT)整理。

2. 实时对选中的内容进行翻译（英译汉），在线查询，需要联网并拥有百度翻译API账号（申请方式见下），同时设置修改`settings`里的`"isUseBaiduAPI"`属性为`true`。
3. 本程序的特点是，按下快捷键即可唤出窗口显示查询、翻译的结果，之后按任意键即可隐藏窗口，或者经过设置的时间之后窗口自动消失（需要自行设置相关参数），无需手动最小化，无需打开网页复制粘贴。

实现原理为检测用户按键，获取剪切板内容，然后通过百度翻译的api获取译文并显示在屏幕上。

## 使用

1. 打开`translation.exe`
2. 输入`run`
3. 选中需要翻译的部分，按下`ctrl` + `C` + `Y`
4. 窗口会弹出一段时间，显示翻译内容，然后自动隐藏（或者等待用户按任意键隐藏，可选）
5. 如果长时间不唤出窗口，窗口将自行关闭
6. 如果想关闭窗口，请按快捷键唤出窗口之后再按`ctrl` + `c` 关闭窗口或者点叉

## 配置

开通百度翻译开放平台的[通用翻译API](https://api.fanyi.baidu.com/product/11)服务(目前免费)，然后复制你的申请信息到`httpgets.py`相应位置:

```python
appid = ''  # 填写你的appid
secretKey = ''  # 填写你的密钥
```

保存之后，设置`"isUseBaiduAPI"`为`true` 即可开启翻译语句的功能。



目前`set`功能还未添加，可以编辑`settings.json`文件进行相关设置。

```
//settings里各属性的说明
{
    "isPause": true, //在弹窗之后，是否需要按任意键才可隐藏窗口
    "showWindow": false, //是否在非唤出状态下在任务栏显示窗口
    "isUseDicv": true, //是否显示词典
    "isUseBaiduAPI": false, //是否显示百度翻译
    "paddingLeft": 0, //窗口弹出时与左侧的距离
    "paddingTop": 0, //窗口弹出时与上方的距离
    "width": 900, //弹出窗口的宽度
    "height": 200, //弹出窗口的高度
    "closeAfterSecs": 600, //经历该时间(s)后, 程序将自动关闭，需要将"isPause"设置成 false
    "showInterval": 3000, //窗口弹出后显示的时间(ms)
    "keys": [17, 67, 89] //唤出窗口, 进行翻译操作的组合键, 数字为相应的ASCII键码, 默认ctrl+c+y
    //如果keys组合当中不包含ctrl+c, 需要先复制待翻译内容再按下组合键才可翻译
}
```

