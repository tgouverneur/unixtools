CC=gcc

all:	ifclone-client.c ifclone-server.c
	${CC} ifclone-client.c -o ifclone-client -lpcap
	${CC} ifclone-server.c -o ifclone-server -lpcap

clean:
	rm -f ifclone-client ifclone-server
