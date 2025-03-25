@ECHO OFF

rem csc *.cs -define:DEBUGPRINT -out:spider.exe

csc *.cs /nowarn:CS0168 -out:spider.exe

