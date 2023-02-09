FROM debian:10
WORKDIR /source
COPY . /source
VOLUME ".":"/source"
RUN apt-get update && apt-get install -y gcc && apt-get install -y make
RUN make clean && make all