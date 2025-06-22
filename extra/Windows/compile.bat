@ECHO OFF

cl.exe /nologo /Ox /MT /W0 /GS- /DNDEBUG /D_ITERATOR_DEBUG_LEVEL=0 /std:c++20 /EHsc /Tp *.cpp /link /OUT:spider.exe /SUBSYSTEM:CONSOLE /MACHINE:x64

del *.obj
