# udp-client-server-echo-example
Simple client-server echo example.

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
* Should run in two termal
### Server
```shell
# Server
./server.out

# client
./client.out 127.0.0.1 8888 'test test 123' 10 jitter
./client.out <server_ip> <port> <message> <max_retry> <retry_algorithms>
#retry_algorithms: 
#    exponential
#    jitter
#    equal_jitter
#    decorrelated_jitter
```

## Test
Suppose you want to test the retry algorithm. You should run the client program only or set --echo 0 in the server program.
```shell
./server.out --echo 0
```

## Reference 
C logging library
https://github.com/rxi/log.c
Backoff Algoritm 
https://aws.amazon.com/tw/blogs/architecture/exponential-backoff-and-jitter/
https://www.baeldung.com/resilience4j-backoff-jitter