popper
======

A Linux daemon program launcher


##Description

The application is a tool I wrote in a weekend using c++14 to automate some of the tasks I repeat when using the terminal.

It has a client-server architecture, where *popper* is the server and *poppe* is the client. The server is istructed to recognise
some class of inputs from the clients, and launch a command on it. It can match some kind of file by extension for example, like 
xdg-open does.


###Popper

This is the server that is in charge of linstening to the client requests. It gets a *command* input and an optional *stdin* input;

The *command* gets modified by an internal regex machinery, executed and the optional *stdin* forwarded to the new process.

```
         .---------------.                           .--------.          .----------------.
STDIN--->| poppe COMMAND |--STDIN-STDINLEN-COMMAND-->| popper |--STDIN-->| regex(COMMAND) |
         '---------------'                           '--------'          '----------------'
```


####Configuration

This is a "semi-formal" specification for the configuration, which is assumed to be in the /etc/popper.conf file

```
SEPA      := "(())"
SPACE     := " " | SPACE SPACE
VALID     := "(?:" VALID ")" | [:alnum:]
CAPTURE   := "(" VALID ")" | CAPTURE CAPTURE
COMMAND   := [:alnum:] "\" [:num:] | "\" [:num:] [:alnum:] | COMMAND COMMAND
CONF_LINE := CAPTURE SPACE SEPA SPACE COMMAND
CONF      := CONF_LINE | CONF "\n" CONF_LINE
```

- The syntax for the regex is the same as the c++11(or higher) ISO.
- Captures can't be nested, but non capturing groups can freely be used inside them.
- The command must have: 
 * The same number of \{0..n} compared to the relative capture, starting from \0, which indicates the left most capture
 * None, in this case the captures are assumed to be consecutive


###Poppe

This is the client. It's used just to pass the inputs to the server.

####Usage
poppe input [-]
command<string>: the input to send to the server
*-*:             forward the stdin to the server.


####*exmaples:*

```
(.*some)  (())  cat ->\0
```

Tells the server to match anything that ends with *some* redirecting it's stdin to it

> echo "hello world" | poppe awesome

> cat awesome

> hello world


####Stopping the service

> kill -INT $(pidof popper)

for now...


###Installing

Since compiling  the project requires g++ 4.9 or clang 3.4 install it and switch to it.

Install [libextra](https://github.com/ltlollo/libextra)

Then, in the project directory:

> mkdir build

> cd build

then to install:

> cmake -DCMAKE_BUILD_TYPE=Release .. && make

> su -c'make install'

or, to produce a debian package:

> make package && su -c'dpkg -i $(ls *.deb|sort|tail -n1)'
