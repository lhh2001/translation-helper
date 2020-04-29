# coding=utf-8
import json
import sys
import stardict

t = stardict.StarDict('ecdict.db')
sys.argv.pop(0)
s = " ".join(sys.argv)
result = t.query(s)
re = 0

if result == None:
    re = re|1

with open("word.json", "w", encoding='utf-8') as f:
    f.write(json.dumps(result, ensure_ascii=False))

lemma = stardict.LemmaDB()
lemma.load('lemma.en.txt')
root = lemma.word_stem(s)

if root != None:
    result = t.query(root[0])
else:
    re = re|2

with open("root.json", "w", encoding='utf-8') as f:
    f.write(json.dumps(result, ensure_ascii=False))

sys.exit(re)
