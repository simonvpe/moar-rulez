FROM gcc:latest
RUN wget https://cmake.org/files/v3.8/cmake-3.8.1-Linux-x86_64.sh
RUN echo y | sh ./cmake-3.8.1-Linux-x86_64.sh  --prefix=/usr/local --exclude-subdir
RUN apt-get update -y && apt-get install -y git

