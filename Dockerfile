FROM debian:10
WORKDIR /source
RUN apt-get update && apt-get install -y gcc && apt-get install -y make