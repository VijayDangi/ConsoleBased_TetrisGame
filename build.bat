cl.exe /EHsc /c /DUNICODE /D_UNICODE Tetris.cpp

link.exe user32.lib Tetris.obj

DEL Tetris.obj
