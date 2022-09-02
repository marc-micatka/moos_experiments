FROM ubuntu:20.04
ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y \
	subversion \ 
	build-essential \
	cmake \
	xterm \
	libfltk1.3-dev  \
	freeglut3-dev \
	libpng-dev \
	libjpeg-dev \
	libxft-dev \
	libxinerama-dev \
	libtiff5-dev \
	protobuf-compiler \
	libboost-all-dev

RUN svn co https://oceanai.mit.edu/svn/moos-ivp-aro/releases/moos-ivp-19.8.1 moos-ivp
RUN cd moos-ivp && \
	chmod +x build-moos.sh && \
	./build-moos.sh
