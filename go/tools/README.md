# tools

A collection of small command line utilities:

* hxd - a small hexdumper
* qotd - Quote of the Day
* remtilde - remove vim backup files (\*~)
* sauk - a small static web server
* tolower - rename all files in current directory to lowercase filenames (so ABC.jpg -> abc.jpg)
* waves - create cos and sin tables
* wi - "w"here "i"s (locate file)
* zen - a small "project management" (ahem) tool


## sauk

If invoked without argument sauk binds to port 8080 and serves files from
the current directory.

The following options can be given to sauk:

```
 -d path         Document root  
 -h              Help  
 -p num          Listen on port number  
```
