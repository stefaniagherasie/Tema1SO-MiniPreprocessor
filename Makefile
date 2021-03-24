CFLAGS = /D_CRT_SECURE_NO_DEPRECATE /W3 /EHsc /Za /MD

build: so-cpp.obj hashmap.obj directory.obj
	cl.exe /Feso-cpp.exe so-cpp.obj hashmap.obj directory.obj

so-cpp.obj: so-cpp.c      
	cl.exe $(CFLAGS) /Foso-cpp.obj /c so-cpp.c

hashmap.obj: hashmap.c     
	cl.exe $(CFLAGS) /Fohashmap.obj /c hashmap.c

directory.obj: directory.c     
	cl.exe $(CFLAGS) /Fodirectory.obj /c directory.c

clean:
	del *.obj *.exe