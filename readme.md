# Webserv

This project is about writing your own HTTP server. You will be able to test it with a real browser. HTTP is one of the most used protocol on internet. Knowing its arcane will be useful.


## Documentation

- Beej's guide to network programming
    - https://beej.us/guide/bgnet/html/#poll
- Detailed info on file descriptors (sockets)
    - https://copyconstruct.medium.com/nonblocking-i-o-99948ad7c957
- Detailed info on epoll
    - https://copyconstruct.medium.com/the-method-to-epolls-madness-d9d2d6378642
    - https://programmer.ink/think/epoll-for-linux-programming.html
    - https://web.archive.org/web/20170810154343/https://idndx.com/2014/09/01/the-implementation-of-epoll-1/ 
- Epoll example code
    - https://github.com/onestraw/epoll-example/blob/master/epoll.c 
    - https://github.com/CGQAQ/how-to-epoll/blob/main/main.c
- Simple HTTP server written in C by IBM guy
    - https://drive.google.com/file/d/0B3msld7qnNOhN1NXaFIwSFU2Mjg/view?resourcekey=0-ngY0neP78dxJKlFv0PJoDQ
- Book on HTTP
    - https://www.manning.com/books/illustrated-guide-to-http?new=true&experiment=B
- Better RFC
    - https://httpwg.org

The following example illustrates a typical HTTP/1.1 message exchange for a GET request (Section 9.3.1) on the URI "http://www.example.com/hello.txt":

Client request:

GET /hello.txt HTTP/1.1
User-Agent: curl/7.64.1
Host: www.example.com
Accept-Language: en, mi

Server response:

HTTP/1.1 200 OK
Date: Mon, 27 Jul 2009 12:28:53 GMT
Server: Apache
Last-Modified: Wed, 22 Jul 2009 19:15:56 GMT
ETag: "34aa387-d-1568eb00"
Accept-Ranges: bytes
Content-Length: 51
Vary: Accept-Encoding
Content-Type: text/plain

Hello World! My content includes a trailing CRLF.
