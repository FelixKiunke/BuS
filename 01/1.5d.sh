#!/bin/bash
cat teamnamen.txt | sort | uniq -c | head -n 1 | sed 's/$/Studierende ohne Teamnamen/'
cat teamnamen.txt | sort | uniq -c | grep -o '^ *[123]' | grep -o '[123]' | sort | uniq -c | sed 's/$/er-Gruppen/'
