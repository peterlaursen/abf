g++ -I /opt/local/include/ -I ../libabf/ -I /usr/local/include/ -o player /usr/local/lib/libaudiere-1.9.4.dylib  /opt/local/lib/libspeex.a ../libabf/libabf.cpp player.cpp database.cpp sqlite3.o -lcurses
