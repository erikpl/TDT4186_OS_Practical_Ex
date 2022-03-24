# Practical exercise 2

Guide to network programing:
https://beej.us/guide/bgnet/html/#shutdownman

Starting server from html directory:

`./mtwwwd ./html 3350`

Sending data with requests, getting index.html:

GET
`curl localhost:3350/index.html --http0.9`
