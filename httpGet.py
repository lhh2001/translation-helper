#百度通用翻译API,不包含词典、tts语音合成等资源，如有相关需求请联系translate_api@baidu.com
# coding=utf-8

import http.client
import hashlib
import urllib
import random
import json
import sys

f = open("settings.json", encoding='utf-8')
settings = json.load(f)

appid = settings["appid"]  # 填写你的appid
secretKey = settings["secretKey"]  # 填写你的密钥
f.close()

httpClient = None
myurl = '/api/trans/vip/translate'

fromLang = 'auto'   #原文语种
toLang = 'zh'   #译文语种
salt = random.randint(32768, 65536)
sys.argv.pop(0)
q = " ".join(sys.argv)
sign = appid + q + str(salt) + secretKey
sign = hashlib.md5(sign.encode()).hexdigest()
myurl = myurl + '?appid=' + appid + '&q=' + urllib.parse.quote(q) + '&from=' + fromLang + '&to=' + toLang + '&salt=' + str(
salt) + '&sign=' + sign

try:
    httpClient = http.client.HTTPConnection('api.fanyi.baidu.com')
    httpClient.request('GET', myurl)

    # response是HTTPResponse对象
    response = httpClient.getresponse()
    result_all = response.read().decode("utf-8")
    result = json.loads(result_all)

    print("原文: ", result['trans_result'][0]['src'])
    print("译文: ", result['trans_result'][0]['dst'])
except Exception as e:
    print (e)
finally:
    if httpClient:
        httpClient.close()
