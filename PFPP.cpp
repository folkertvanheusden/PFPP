// (C) 2019 by folkert van heusden, released under AGPL v3.0
#include <errno.h>
#include <getopt.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <thread>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>

#include "picio.h"

bool send_frame(const int fd, const struct sockaddr_in *const servaddr, const int sa_len, const uint8_t *const data, const int w, const int h, const bool udp)
{
	char buffer[1122];
	int o = 0;

	for(int y=0; y<h; y++) {
		for(int x=0; x<w; x++) {
			const uint8_t *const p = &data[y * w * 3 + x * 3];

			o += snprintf(&buffer[o], sizeof(buffer) - o, "PX %d %d %02X%02X%02X\n", x, y, p[0], p[1], p[2]);

			if (o >= 1122 - 24 || !udp) {
				if (udp)
					sendto(fd, buffer, o, 0, (const struct sockaddr *)servaddr, sa_len); 
				else {
					if (write(fd, buffer, o) <= 0)
						return false;
				}

				o = 0;
			}
		}
	}

	if (o) {
		if (udp)
			sendto(fd, buffer, o, 0, (const struct sockaddr *)servaddr, sa_len); 
		else {
			if (write(fd, buffer, o) <= 0)
				return false;
		}
	}

	return true;
}

void thrd(const char *const ip, const int port, const bool udp, const unsigned char *const pixels, const int w, const int h)
{
	int fd = -1;
	struct sockaddr_in servaddr;

	for(;;) {
		if (fd == -1) {
			fd = socket(AF_INET, udp ? SOCK_DGRAM : SOCK_STREAM, 0);

			memset(&servaddr, 0, sizeof(servaddr));
			servaddr.sin_family = AF_INET;
			servaddr.sin_port = htons(port);
			servaddr.sin_addr.s_addr = inet_addr(ip);

			if (!udp && connect(fd, (const sockaddr *)&servaddr, sizeof(servaddr)) == -1)
				printf("Connect failed: %s\n", strerror(errno));
		}

		if (send_frame(fd, &servaddr, sizeof(servaddr), pixels, w, h, udp) == false) {
			close(fd);
			fd = -1;
		}
	}
}

void help()
{
	printf("PFPP v" VERSION "\n");
	printf("(c) 2019 by folkert@vanheusden.com\n");
	printf("\n");
	printf("-f  file to send\n");
	printf("-i  IPv4 address of target\n");
	printf("-p  port of target\n");
	printf("-u  use UDP\n");
	printf("-n  number of threads\n");
}

int main(int argc, char *argv[])
{
	const char *file = NULL, *ip = NULL;
	int c = -1, port = 5004, n = 1;
	bool udp = false;

	while((c = getopt(argc, argv, "f:i:p:un:")) != -1)
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

			case 'u':
				udp = true;
				break;

			case 'n':
				n = atoi(optarg);
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

	signal(SIGPIPE, SIG_IGN);

	for(int i=0; i<n; i++) {
		std::thread t(thrd, ip, port, udp, pixels, w, h);
		t.detach();
	}

	printf("Threads started\n");
	getchar();

	return 0;
}
