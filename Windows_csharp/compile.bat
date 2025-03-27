@ECHO OFF

csc *.cs -define:DEBUGPRINT -nowarn:CS0168 -out:spider.exe

rem csc *.cs -nowarn:CS0168 -out:spider.exe

