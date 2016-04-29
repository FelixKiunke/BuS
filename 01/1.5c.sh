#!/bin/bash
cat "teamnamen.txt" | sort | uniq -c | grep '^ *3 '
# Um Zweiergruppen auszugeben, muss im GREP-Regex die 3 durch 2 ersetzt werden.
# Es gibt 9 einmalige Teamnamen.
# 214 Studierende haben keinen Teamnamen angegeben.
