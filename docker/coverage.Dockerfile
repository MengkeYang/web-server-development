FROM wnza:base as builder

COPY . /usr/src/projects/WNZA
WORKDIR /usr/src/projects/WNZA/build

RUN pwd
RUN cmake -DCMAKE_BUILD_TYPE=Coverage ..
RUN make coverage