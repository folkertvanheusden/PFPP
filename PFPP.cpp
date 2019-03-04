// (C) 2019 by folkert van heusden, released under AGPL v3.0
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>

#include "picio.h"

void send_frame(const char *const host, const int port, const uint8_t *const data, const int w, const int h)
{
	int fd = socket(AF_INET, SOCK_DGRAM, 0);
	struct sockaddr_in servaddr;
	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(port);
	servaddr.sin_addr.s_addr = inet_addr(host);

	char buffer[1122];

	buffer[0] = 0; // protocol version 0
	buffer[1] = 0;
	int o = 2;
	for(int y=0; y<h; y++) {
		for(int x=0; x<w; x++) {
			const uint8_t *const p = &data[y * w * 3 + x * 3];

			buffer[o++] = x;
			buffer[o++] = x >> 8;
			buffer[o++] = y;
			buffer[o++] = y >> 8;
			buffer[o++] = p[0];
			buffer[o++] = p[1];
			buffer[o++] = p[2];

			if (o >= 1122 - 6) {
				sendto(fd, buffer, o, 0, (const struct sockaddr *) &servaddr, sizeof(servaddr)); 
				o = 2;
			}
		}
	}

	if (o > 2)
		sendto(fd, buffer, o, 0, (const struct sockaddr *) &servaddr, sizeof(servaddr)); 

	close(fd);
}

void help()
{
	printf("PFPP v" VERSION "\n");
	printf("(c) 2019 by folkert@vanheusden.com\n");
	printf("\n");
	printf("-f  file to send\n");
	printf("-i  IPv4 address of target\n");
	printf("-p  port of target\n");
}

int main(int argc, char *argv[])
{
	const char *file = NULL, *ip = NULL;
	int c = -1, port = 5004;

	while((c = getopt(argc, argv, "f:i:p:")) != -1)
	{
		switch(c) {
			case 'f':
				file = optarg;
				break;

			case 'i':
				ip = optarg;
				break;

			case 'p':
				port = atoi(optarg);
				break;

			default:
				help();
				return 0;
		}
	}

	if (!file || !ip) {
		help();
		return 1;
	}

	unsigned char *pixels = NULL;
	int w = 0, h = 0;
	if (!read_JPEG_memory(file, &w, &h, &pixels)) {
		printf("Cannot read JPEG file\n");
		return 1;
	}

	for(;;)
		send_frame(ip, port, pixels, w, h);

	return 0;
}
