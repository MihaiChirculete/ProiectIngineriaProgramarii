FROM ubuntu:latest
LABEL maintainer="mihai.chirculete98@gmail.com"

USER root
WORKDIR /root/

EXPOSE 80/tcp
EXPOSE 80/udp

EXPOSE 443/tcp
EXPOSE 443/udp

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get -y update
RUN apt-get -y upgrade
# sudo doesn't come installed by default
# unless we are going to create other users than root, we don't actually need sudo
# RUN apt-get -y install sudo
RUN apt-get -y install tzdata
RUN apt-get -y install build-essential
RUN apt install -y git-all
RUN apt-get -y install cmake
RUN apt-get -y install make
RUN apt-get -y install nano
RUN apt-get -y install vim
RUN apt-get -y install wget
RUN apt-get -y install curl

RUN mkdir -p /root/install/
COPY setup-pistache.sh /root/install/

RUN chmod +x /root/install/setup-pistache.sh

RUN /root/install/setup-pistache.sh
