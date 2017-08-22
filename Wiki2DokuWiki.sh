#! /usr/bin/bash
#
# Convert 'Page-Title.en.txt' files to DokuWiki named 'page_title.txt'.
# Obviously Linux/Unix/Mac only. Windows, sorry. :(
#
# Norman Dunbar
# 18th August 2017.
#
for x in Pages/*.en.txt
do    
    i="$x"
    i=`basename "$i"`
    o=`echo "$i" | tr '[A-Z]-' '[a-z]_'`
    o="${o%.en.txt}.txt"
    echo "$x" = "$o"
    ./rwapWiki "$x" convert.DOKUWIKI.txt > "DW/$o"
done
