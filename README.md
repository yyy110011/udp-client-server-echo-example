
# udp-client-server-echo-example
Simple client-server echo example.
[![C/C++ CI](https://github.com/yyy110011/udp-client-server-echo-example/actions/workflows/c-cpp.yml/badge.svg)](https://github.com/yyy110011/udp-client-server-echo-example/actions/workflows/c-cpp.yml)
## Compile
 
```shell
# In your machine
root@your-pc:/> make all

# Using Docker
root@your-pc:/> docker build -t <image_name> .
root@your-pc:/> docker run -it -v $(pwd):/source <image_name>
root@debian:/> make all
```
## Usage
* It should run in two terminals. (one for the server and one for the client).
#### Server
```shell
root@debian:/> ./server.out
```
#### Client
```shell
root@debian:/> ./client.out 127.0.0.1 8888 'test test 123' 10 jitter
root@debian:/> ./client.out <server_ip> <port> <message> <max_retry> <retry_algorithms>
#retry_algorithms: 
#    exponential
#    jitter
#    equal_jitter
#    decorrelated_jitter
```

## Test
Suppose you want to test the retry algorithm. You should run the client program only or set --echo 0 in the server program.
```shell
root@debian:/> ./server.out --echo 0
```

## Reference 
##### C logging library
https://github.com/rxi/log.c
##### Backoff Algoritm 
https://aws.amazon.com/tw/blogs/architecture/exponential-backoff-and-jitter/
https://www.baeldung.com/resilience4j-backoff-jitter