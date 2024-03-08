#!/bin/bash
./swffx -s ota.xf
test -s ota.html && firefox ota.html
./swffx -s ftb.xf
test -s ftb.html && firefox ftb.html
