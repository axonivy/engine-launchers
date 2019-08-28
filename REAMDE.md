# ivy-engine-launchers

## projects

### JNILib
C++ Library with useful functions to work with JNI (Java Native Interface)

### JavaWindowsServiceLauncher
Builds an executable that is a windows service. If one starts the
windows server a jvm is instanciated and a java class that has to
extend the java class ch.ivyteam.server.win.WindowsService is started

### JavaWindowsServiceHandler
Implements the native methods of the java class
`ch.ivyteam.server.win.WindowsServiceHandler`. The class allows to query
all installed windows services and to start and stop the service.

### JavaLauncher
Builds an executable to launch a java program by instanciating a jvm
and start the public static main method of a java class. Similar to the
java executable.

### JVMLauncher
Builds a dynamic link library that provides functions to instanciate a
jvm and start java class. Also provides functions for windows services.

## Dev Environment Setup
- Install VisualStudio 2005 with all features (from MSDN)
- Open cpp-launcher-solution/cpp-launcher-solution.sln
- Configure VisualStudio (Tool > Options > Projects and Solutions > VC++ Directories
  - x64 / Include Files
    - C:\Program Files\Java\jdk1.8.0_121\include
    - C:\Program Files\Java\jdk1.8.0_121\include\win32
