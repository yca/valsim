# syntax = docker/dockerfile:1.0-experimental

FROM ubuntu:jammy AS valsim

RUN apt-get update

RUN apt-get update && \
    DEBIAN_FRONTEND=noninteractive apt-get install -y vim python3 wget \
        pip curl git ffmpeg build-essential \
        cmake clang libelf1 libelf-dev zlib1g-dev libbpf-dev bpfcc-tools \
        pkg-config libqt5core5a qt5-qmake qtbase5-dev qt5-qmake libprocps-dev libopencv-dev apt-utils

RUN echo 'root:root' | chpasswd

RUN useradd -ms /bin/bash valsimdev

FROM valsim
#placeholder target, this will be built w/o any build target flags
