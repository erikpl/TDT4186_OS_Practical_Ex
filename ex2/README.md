# Practical exercise 2

Guide to network programing:
https://beej.us/guide/bgnet/html/#shutdownman

Compiling project:
`gcc sem.c bbuffer.c mtwwwd.c -lpthread -o "mtwwwd"`

Starting server from html directory:

`./mtwwwd ./html 3350 4 6`

Sending data with requests, getting index.html:

GET
`curl localhost:3350/index.html --http0.9`
