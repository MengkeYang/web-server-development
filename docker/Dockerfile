### Build/test container ###
# Define builder stage
FROM wnza:base as builder

# Share work directory
COPY . /usr/src/projects/WNZA
WORKDIR /usr/src/projects/WNZA/build
ENV PATH_TO_BIN=/usr/src/projects/WNZA/build/bin/server_executable

# Build and test
RUN cmake ..
RUN make -j
RUN ctest --output-on_failure


### Deploy container ###
# Define deploy stage
FROM ubuntu:bionic as deploy

# Copy server output binary to "."
COPY --from=builder /usr/src/projects/WNZA/build/bin/server_executable .
COPY --from=builder /usr/src/projects/WNZA/tests/deploy_config .
COPY --from=builder /usr/src/projects/WNZA/tests/snake.html .
COPY --from=builder /usr/src/projects/WNZA/tests/cat.jpg .
COPY --from=builder /usr/src/projects/WNZA/tests/404page.html .
COPY --from=builder /usr/src/projects/WNZA/tests/mobydick.txt .
COPY --from=builder /usr/src/projects/WNZA/tests/r.zip .

# Expose some port(s)
EXPOSE 80

# Use ENTRYPOINT to specify the binary name
ENTRYPOINT ["./server_executable"]

# Use CMD to specify arguments to ENTRYPOINT
CMD ["deploy_config"]
