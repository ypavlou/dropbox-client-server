# TCP-dropbox-client-server
Simple dropbox  client-server simulation using sockets

This is an implementation of a simplified version of dropbox where different clients will synchronize a set of files.

Each client will be connected to a dropbox server, will learn which other clients are connected as well using a synchronized access client list as information on how to communicate with them. 
Each client will connect to each of the other clients in order to sync their files so that eventually everyone has up-to-date copies of all the files.


 ##  Compilation: 
 In each directory: 
  ``` Ruby
  make
```
Remove object files: 
 ``` Ruby
  make clean
```
 ##  Running server:
  ``` Ruby
./dropbox_server –p portNum
```
 ##  Running client:
  ``` Ruby
./dropbox_client –d dirName –p portNum –w workerThreads –b bufferSize –sp serverPort –sip serverIP
```
- dirName: the directory with files to synchronize.
- portNum: the port number that the server/client listens.
- workerThreads: number of threads the client creates.
- bufferSize: size of a <b> circular </b> buffer that threads share.
- serverPort: port number of server that the client will connect to.
- serverIP: IP address of server.



