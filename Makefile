CC=i686-w64-mingw32-g++

JDK_INCLUDE=/usr/lib/jvm/java-8-openjdk-amd64/include
JDK_INCLUDE_LINUX=$(JDK_INCLUDE)/linux
JDK_LINKING=-I $(JDK_INCLUDE) -I $(JDK_INCLUDE_LINUX)

compile:
	mkdir bin
	$(CC) JNILib/JNILib.cpp -o bin/JNILib.dll $(JDK_LINKING) -shared
	$(CC) JvmLauncher/JVMLauncher.h -o bin/JVMLauncher.dll $(JDK_LINKING) -shared
	#$(CC) JavaLauncher/AxonIvyEngine/AxonIvyEngine.cpp -o AxonIvyEngine.exe $(JDK_LINKING)
	#$(CC) JavaWindowsServiceLauncher/JavaWindowsServiceLauncher.cpp -o AxonIvyEngineService.exe $(JDK_LINKING) 

clean:
	rm -rf bin
