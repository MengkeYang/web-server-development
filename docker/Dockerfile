### Build/test container ###
# Define builder stage
FROM wnza:base as builder

# Share work directory
COPY . /usr/src/projects/WNZA
WORKDIR /usr/src/projects/WNZA/build

# Build and test
RUN cmake ..
RUN make
RUN ctest --output-on_failure


### Deploy container ###
# Define deploy stage
FROM ubuntu:latest as deploy

# TODO(!): Copy server output binary to "."
COPY --from=builder /usr/src/projects/WNZA/build/bin/server_executable .
COPY --from=builder /usr/src/projects/WNZA/tests/example_server_config .

# TODO(!): Expose some port(s)
EXPOSE 80

# Use ENTRYPOINT to specify the binary name
# TODO(!): Update with real server name
ENTRYPOINT ["./server_executable"]

# Use CMD to specify arguments to ENTRYPOINT
# TODO(!): Update with real server args
CMD ["example_server_config"]
