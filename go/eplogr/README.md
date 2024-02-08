# eplogr
eplogr is developer oriented Endpoint Logger. It must be used together with ngrok.
With ngrok you build a tunnel between your development PC and the internet.
eplogr will log what you post to that tunnel for offsite viewing.

## How to build eplogr
Install go lang 1.21.3 or newer, then:
```
cd eplogr
go mod tidy
cd cmd/epilogr
go install -ldflags '-s' ./...
```

## How to run eplogr
Open a command line prompt and type:
```
eplogr
```
... this will run eplogr in it's default configuration where it will allocate a new tunnel only for you.
If you POST to that tunnel the request will be stored in the current directory. The client will get a ngrok page about an authtoken.
You should really set that up - follow the link in the article.

## Configuration
eplogr can be configured by:

* a configuration file - .eplogrrc on Linux, \_eplogrrc on Windows. The files must be stored in the user's home folder.
* overridden by environment variables EPLOGR_.
* and further overriden by command line arguments.

## Configuration file
The configuration file is a simple json-fil:
```
{
	"Domain": "ngrok-domain or blank to autogenerate",
	"AuthToken": "ngrok Auth Token",
	"Extension": "file extension",
	"DestDir": "Where to store files",
	"MaxSize": "Maximum size of files as int"
}
```
Please note, that in the above the value for MaxSize is given as string - it must be an integer (a number).

## Environment variables
Set any or all of *EPLOGR_DOMAIN*, *EPLOGR_AUTHTOKEN*, *EPLOGR_EXTENSION*, *EPLOGR_DESTDIR* and/or *EPLOGR_MAXSIZE* in your environment to override the corresponding configuration items.

## Command-line configuration
```
Usage: eplogr [OPTIONS] [destination-dir]

[OPTIONS]
 -c config       Configuration file (default is ~/[._]eplogrrc)
 -D              Disable file-writing
 -d domain       Ngrok domain
 -e extension    File-extension of received file
 -m size         Max size of request data shown / written to file (default is 10KB)
 -h              Help (this page)
 -t token        Authentication token

These can also be set in the environment (EPLOGR_DOMAIN, EPLOGR_AUTHTOKEN,
EPLOGR_EXTENSION, EPLOGR_DESTDIR, EPLOGR_MAXSIZE).
```
