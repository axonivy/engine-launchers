#FROM purplekarrot/mingw-w64-x86-64

FROM ubuntu:16.04
RUN apt-get update -y && apt-get install -y gcc && apt-get install -y g++-mingw-w64
RUN apt-get install default-jdk -y
RUN apt-get install make -y
