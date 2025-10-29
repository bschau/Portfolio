# gxdavr

This is a simple DAV redirect "proxy" for Gnome Contacts (or other programs in need of such feature).

Gnome Contacts do not know how to handle DAV requests. But it knows how to handle NextCloud contacts (sort of).

So gxdavr hijacks that. You setup an account as a NextCloud-account and then configure gxdavr to use your real credentials. gxdavr will then proxy to your real CalDAV / CardDAV. No NextCloud server is needed.

## Usage
```
Usage: gxdavr [OPTIONS]

[OPTIONS]
 -c config       Configuration file (default is ~/.gxdavrrc)
 -h              Help (this page)
```

## Configuration file (.gxdavrrc)

This is the configuration file I am using - I use posteo.de as my mailbox provider:

```
{
	"Port": 8080,
	"CalendarUrl": "https://posteo.de:8843/calendars/MAILBOX-NAME/default",
	"AddressbookUrl": "https://posteo.de:8843/addressbooks/MAILBOX-NAME/default"
}
```

In the settings above, MAILBOX-NAME must be substituted with your real mailbox name (the part before @posteo.net).

I run gxdavr out of 'Startup Applications Preferences'.

Configure Gnome Contacts by first adding a NextCloud online account:

```
Server
	http://localhost:8080
Username
	MAILBOX-NAME@posteo.de
Password
	-- your Posteo password --
```

Then, in Gnome Contacts, change the Addressbook to the new NextCloud addressbok.









