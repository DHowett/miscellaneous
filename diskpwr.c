/*
 * Dustin L. Howett
 * 2013-04-21
 *
 * Lovingly stolen from hdparm, and is therefore released under a compatible license.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */

#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <stdio.h>

enum {
	HDIO_DRIVE_CMD          = 0x031f,
} _ioctls;

enum {
	ATA_OP_CHECKPOWERMODE2          = 0x98,
	ATA_OP_CHECKPOWERMODE1          = 0xe5,
} _ataops;

int main(int argc, const char *argv[], const char *envp[])
{
	if(argc < 2) {
		fprintf(stderr, "Hey. %s <device>.\n", argv[0]);
		return 1;
	}

	int fd = open(argv[1], O_RDONLY|O_NONBLOCK);
	if(fd < 0) {
		fprintf(stderr, "Hey. Couldn't open %s\n", argv[1]);
		return 1;
	}
	unsigned char args[4] = {ATA_OP_CHECKPOWERMODE1,0,0,0};
	if(ioctl(fd, HDIO_DRIVE_CMD, args) &&
			(args[0] = ATA_OP_CHECKPOWERMODE2) &&
			ioctl(fd, HDIO_DRIVE_CMD, args)) {
		fprintf(stderr, "failed to get power mode\n");
		close(fd);
		return 1;
	}
	char *status = "unknown";
	switch(args[2]) {
		case 0x00: status = "standby"; break;
		case 0x40: status = "spindown"; break;
		case 0x41: status = "spinup"; break;
		case 0x80: status = "idle"; break;
		case 0xFF: status = "active"; break;
	}
	printf("%s\n", status);
	close(fd);
	return 0;
}
