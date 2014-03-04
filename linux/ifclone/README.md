Aim:
====

I was personally interested into dumping some private vlan traffic on some
blackbox MIPS device to anotherbox for further analysis. I don't know
if anybody else can be interested of doing anything with this tho.


Compilation:
============

If you don't need the little-endianess correction on the server,
just comment the #define REMOTE_LE inside ifclone-sever.c.

$ make
gcc ifclone-client.c -o ifclone-client -lpcap
gcc ifclone-server.c -o ifclone-server -lpcap
$

Cross-compilation:
==================

I've personlly used the client to dump an iface on a MIPS architecture modem.
It is then compatible with MIPS/Linux arch. If you are using a client with
little-endianess, don't forget to look at the server.c file to remove the comment
on the #define REMOTE_LE, which activate the little-endianess correction.


Contact:
========

For any suggestion or question, just drop me an e-mail: thomas@espix.net

Greetings:
==========

 - NESS, my love ;]
 - GMsoft, for the target_tap part of packet-o-matic which I reused ;]
