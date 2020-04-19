FROM wnza:base as builder

COPY . /usr/src/projects/WNZA
WORKDIR /usr/src/projects/WNZA/build
ENV PATH_TO_BIN=/usr/src/projects/WNZA/build/bin/server_executable

RUN pwd
RUN cmake -DCMAKE_BUILD_TYPE=Coverage ..
RUN make coverage
