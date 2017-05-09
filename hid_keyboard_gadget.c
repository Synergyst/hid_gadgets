/* hid_keyboard_gadget.c */

#include <pthread.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define BUF_LEN 512

#define SHIFT 0x80

struct options {
	const char    *opt;
	unsigned char val;
};

static struct options kmod[] = {
	{.opt = "left-ctrl",		.val = 0x01},
	{.opt = "right-ctrl",		.val = 0x10},
	{.opt = "left-shift",		.val = 0x02},
	{.opt = "right-shift",		.val = 0x20},
	{.opt = "left-alt",		.val = 0x04},
	{.opt = "right-alt",		.val = 0x40},
	{.opt = "left-meta",		.val = 0x08},
	{.opt = "right-meta",		.val = 0x80},
	{.opt = NULL}
};

static struct options kval[] = {
	{.opt = "65",                   .val = 0x04|SHIFT}, // A
        {.opt = "66",                   .val = 0x05|SHIFT}, // B
        {.opt = "67",                   .val = 0x06|SHIFT}, // C
        {.opt = "68",                   .val = 0x07|SHIFT}, // D
        {.opt = "69",                   .val = 0x08|SHIFT}, // E
        {.opt = "70",                   .val = 0x09|SHIFT}, // F
        {.opt = "71",                   .val = 0x0a|SHIFT}, // G
        {.opt = "72",                   .val = 0x0b|SHIFT}, // H
        {.opt = "73",                   .val = 0x0c|SHIFT}, // I (WILL NOT PRINT:- FIXME)
        {.opt = "74",                   .val = 0x0d|SHIFT}, // J
        {.opt = "75",                   .val = 0x0e|SHIFT}, // K
        {.opt = "76",                   .val = 0x0f|SHIFT}, // L
        {.opt = "77",                   .val = 0x10|SHIFT}, // M
        {.opt = "78",                   .val = 0x11|SHIFT}, // N
        {.opt = "79",                   .val = 0x12|SHIFT}, // O
        {.opt = "80",                   .val = 0x13|SHIFT}, // P
        {.opt = "81",                   .val = 0x14|SHIFT}, // Q
        {.opt = "82",                   .val = 0x15|SHIFT}, // R
        {.opt = "83",                   .val = 0x16|SHIFT}, // S
        {.opt = "84",                   .val = 0x17|SHIFT}, // T
        {.opt = "85",                   .val = 0x18|SHIFT}, // U
        {.opt = "86",                   .val = 0x19|SHIFT}, // V
        {.opt = "87",                   .val = 0x1a|SHIFT}, // W
        {.opt = "88",                   .val = 0x1b|SHIFT}, // X
        {.opt = "89",                   .val = 0x1c|SHIFT}, // Y
        {.opt = "90",                   .val = 0x1d|SHIFT}, // Z
	{.opt = "97",			.val = 0x04}, // a
	{.opt = "98",			.val = 0x05}, // b
	{.opt = "99",			.val = 0x06}, // c
	{.opt = "100",			.val = 0x07}, // d
	{.opt = "101",			.val = 0x08}, // e
	{.opt = "102",			.val = 0x09}, // f
	{.opt = "103",			.val = 0x0a}, // g
	{.opt = "104",			.val = 0x0b}, // h
	{.opt = "105",			.val = 0x0c}, // i
	{.opt = "106",			.val = 0x0d}, // j
	{.opt = "107",			.val = 0x0e}, // k
	{.opt = "108",			.val = 0x0f}, // l
	{.opt = "109",			.val = 0x10}, // m
	{.opt = "110",			.val = 0x11}, // n
	{.opt = "111",			.val = 0x12}, // o
	{.opt = "112",			.val = 0x13}, // p
	{.opt = "113",			.val = 0x14}, // q
	{.opt = "114",			.val = 0x15}, // r
	{.opt = "115",			.val = 0x16}, // s
	{.opt = "116",			.val = 0x17}, // t
	{.opt = "117",			.val = 0x18}, // u
	{.opt = "118",			.val = 0x19}, // v
	{.opt = "119",			.val = 0x1a}, // w
	{.opt = "120",			.val = 0x1b}, // x
	{.opt = "121",			.val = 0x1c}, // y
	{.opt = "122",			.val = 0x1d}, // z
	{.opt = "49",			.val = 0x1e}, // 1
	{.opt = "50",			.val = 0x1f}, // 2
	{.opt = "51",			.val = 0x20}, // 3
	{.opt = "52",			.val = 0x21}, // 4
	{.opt = "53",			.val = 0x22}, // 5
	{.opt = "54",			.val = 0x23}, // 6
	{.opt = "55",			.val = 0x24}, // 7
	{.opt = "56",			.val = 0x25}, // 8
	{.opt = "57",			.val = 0x26}, // 9
	{.opt = "48",			.val = 0x27}, // 0
	{.opt = "return",		.val = 0x28}, // 
	{.opt = "enter",		.val = 0x28}, // 
	{.opt = "27",			.val = 0x29}, // esc(escape)
	{.opt = "escape",		.val = 0x29}, // 
	{.opt = "8",			.val = 0x2a}, // bckspc(backspace)
	{.opt = "backspace",		.val = 0x2a}, // 
	{.opt = "tab",			.val = 0x2b}, // 
	{.opt = "32",			.val = 0x2c}, // space
	{.opt = "45",			.val = 0x2d}, // minus(hyphen)
	{.opt = "dash",			.val = 0x2d}, // minus(hyphen)
	{.opt = "61",			.val = 0x2e}, // equal symbol
	{.opt = "equal",		.val = 0x2e}, // equal symbol
	{.opt = "91",			.val = 0x2f}, // [
	{.opt = "93",			.val = 0x30}, // ]
	{.opt = "92",			.val = 0x31}, // backslash
	{.opt = "hash",			.val = 0x32}, // number(hash symbol)
	{.opt = "35",			.val = 0x32}, // number(hash symbol)
	{.opt = "59",			.val = 0x33}, // semicolon
	{.opt = "34",			.val = 0x34}, // qoute(double qoutation mark)
	{.opt = "96",			.val = 0x35}, // grave
	{.opt = "44",			.val = 0x36}, // comma
	{.opt = "46",			.val = 0x37}, // period
	{.opt = "33",			.val = 0x9e|SHIFT}, // exclamation point(NON-WORKING!)
	{.opt = "stop",			.val = 0x37}, // full-stop(period)
	{.opt = "slash",		.val = 0x38}, // 
	{.opt = "caps-lock",		.val = 0x39}, // 
	{.opt = "capslock",		.val = 0x39}, // 
	{.opt = "f1",			.val = 0x3a}, // 
	{.opt = "f2",			.val = 0x3b}, // 
	{.opt = "f3",			.val = 0x3c}, // 
	{.opt = "f4",			.val = 0x3d}, // 
	{.opt = "f5",			.val = 0x3e}, // 
	{.opt = "f6",			.val = 0x3f}, // 
	{.opt = "f7",			.val = 0x40}, // 
	{.opt = "f8",			.val = 0x41}, // 
	{.opt = "f9",			.val = 0x42}, // 
	{.opt = "f10",			.val = 0x43}, // 
	{.opt = "f11",			.val = 0x44}, // 
	{.opt = "f12",			.val = 0x45}, // 
	{.opt = "print",		.val = 0x46}, // 
	{.opt = "scroll-lock",		.val = 0x47}, // 
	{.opt = "scrolllock",		.val = 0x47}, // 
	{.opt = "pause",		.val = 0x48}, // 
	{.opt = "insert",		.val = 0x49}, // 
	{.opt = "home",			.val = 0x4a}, // 
	{.opt = "pageup",		.val = 0x4b}, // 
	{.opt = "pgup",			.val = 0x4b}, // 
	{.opt = "del",			.val = 0x4c}, // 
	{.opt = "delete",		.val = 0x4c}, // 
	{.opt = "end",			.val = 0x4d}, // 
	{.opt = "pagedown",		.val = 0x4e}, // 
	{.opt = "pgdown",		.val = 0x4e}, // 
	{.opt = "right",		.val = 0x4f}, // 
	{.opt = "left",			.val = 0x50}, // 
	{.opt = "down",			.val = 0x51}, // 
	{.opt = "up",			.val = 0x52}, // 
	{.opt = "num-lock",		.val = 0x53}, // 
	{.opt = "numlock",		.val = 0x53}, // 
	{.opt = "kp-divide",		.val = 0x54}, // 
	{.opt = "kp-multiply",		.val = 0x55}, // 
	{.opt = "kp-minus",		.val = 0x56}, // 
	{.opt = "kp-plus",		.val = 0x57}, // 
	{.opt = "kp-enter",		.val = 0x58}, // 
	{.opt = "kp-return",		.val = 0x58}, // 
	{.opt = "kp-1",			.val = 0x59}, // 
	{.opt = "kp-2",			.val = 0x5a}, // 
	{.opt = "kp-3",			.val = 0x5b}, // 
	{.opt = "kp-4",			.val = 0x5c}, // 
	{.opt = "kp-5",			.val = 0x5d}, // 
	{.opt = "kp-6",			.val = 0x5e}, // 
	{.opt = "kp-7",			.val = 0x5f}, // 
	{.opt = "kp-8",			.val = 0x60}, // 
	{.opt = "kp-9",			.val = 0x61}, // 
	{.opt = "kp-0",			.val = 0x62}, // 
	{.opt = "kp-period",		.val = 0x63}, // 
	{.opt = "kp-stop",		.val = 0x63}, // 
	{.opt = "application",		.val = 0x65}, // 
	{.opt = "power",		.val = 0x66}, // 
	{.opt = "kp-equals",		.val = 0x67}, // 
	{.opt = "kp-equal",		.val = 0x67}, // 
	{.opt = "f13",			.val = 0x68}, // 
	{.opt = "f14",			.val = 0x69}, // 
	{.opt = "f15",			.val = 0x6a}, // 
	{.opt = "f16",			.val = 0x6b}, // 
	{.opt = "f17",			.val = 0x6c}, // 
	{.opt = "f18",			.val = 0x6d}, // 
	{.opt = "f19",			.val = 0x6e}, // 
	{.opt = "f20",			.val = 0x6f}, // 
	{.opt = "f21",			.val = 0x70}, // 
	{.opt = "f22",			.val = 0x71}, // 
	{.opt = "f23",			.val = 0x72}, // 
	{.opt = "f24",			.val = 0x73}, // 
	{.opt = "execute",		.val = 0x74}, // 
	{.opt = "help",			.val = 0x75}, // 
	{.opt = "menu",			.val = 0x76}, // 
	{.opt = "select",		.val = 0x77}, // 
	{.opt = "cancel",		.val = 0x78}, // 
	{.opt = "redo",			.val = 0x79}, // 
	{.opt = "undo",			.val = 0x7a}, // 
	{.opt = "cut",			.val = 0x7b}, // 
	{.opt = "copy",			.val = 0x7c}, // 
	{.opt = "paste",		.val = 0x7d}, // 
	{.opt = "find",			.val = 0x7e}, // 
	{.opt = "mute",			.val = 0x7f}, // 
	{.opt = "volume-up",		.val = 0x80}, // These are multimedia keys, they will not work on standard keyboard, they need a different USB descriptor
	{.opt = "volume-down",		.val = 0x81}, // 
	{.opt = NULL}
};

int itoa(int value, char *ptr) {
    int count=0,temp;
  if(ptr==NULL)
    return 0;
  if(value==0) {
    *ptr='0';
    return 1;
  }

  if(value<0) {
    value*=(-1);
    *ptr++='-';
    count++;
  }
  for(temp=value;temp>0;temp/=10,ptr++);
    *ptr='\0';
  for(temp=value;temp>0;temp/=10) {
    *--ptr=temp%10+'0';
    count++;
  }
  return count;
}

void sendSingleChar(const char *mechar) {
  char mahchar[48];
  char strbuf[48];
  int intbuf[48];

  strcpy(strbuf, "echo ");
  strcat(strbuf, "left-shift ");
  //sprintf(mahchar, "%s", sprintf(mahchar, "%s", tolower(sprintf(mechar, "%d", mechar)))); // fixme
  /*sprintf(mahchar, "%s", mechar);
  strcat(strbuf, mahchar);*/
  itoa(tolower(atoi(mechar)), mahchar);
  //sprintf(mahchar, "%s", tolower(atoi(mechar)));
  strcat(strbuf, mahchar/*tolower(atoi(mechar))*/); // close, but no cigar. maybe
  strcat(strbuf, " | hid_gadget_test /dev/hidg0 keyboard");
  int exitState = system(strbuf);
  printf("%s(str), %d(ASCII dec), %d(exit code)\n", mechar, mechar, exitState);
  if (exitState == 0) {
    printf("Sending lower case char now!\n");
  } else {
    printf("Exiting due to exit failure on last attempt..\n");
    exit(EXIT_FAILURE);
  }
}

int keyboard_fill_report(char report[8], char buf[BUF_LEN], int *hold) {
	char *tok = strtok(buf, " ");
	int key = 0;
	int i = 0;

	for (; tok != NULL; tok = strtok(NULL, " ")) {

		if (strncmp(tok, "--", 2) == 0)
			tok += 2;

		if (strcmp(tok, "quit") == 0)
			return -1;

		if (strcmp(tok, "hold") == 0) {
			*hold = 1;
			continue;
		}

		if (key < 6) {
			for (i = 0; kval[i].opt != NULL; i++)
				if (strcmp(tok, kval[i].opt) == 0) {
					if (kmod[i].val) {
						if (kval[i].val & 0x80) {
							report[2 + key++] = kval[i].val; // comment me?
							/*for (int r = 0; r < 8; r++) {
								printf("caught shifted: %d(hex)\n", report[r]);
							}*/
							printf("caught shifted: 0x%x(hex), %d(dec), %s(ASCII)\n", report[2], report[2], kval[i].opt);
							sendSingleChar(kval[i].opt); // resend value with left-shift modifier
						} else {
							report[2 + key++] = kval[i].val;
							/*for (int r = 0; r < 8; r++) {
								printf("caught else: %d(hex)\n", report[r]);
							}*/
							printf("caught else: 0x%x(hex), %d(dec), %s(ASCII)\n", report[2], report[2], kval[i].opt);
						}
					}
					break;
				}
			if (kval[i].opt != NULL)
				continue;
		}

		for (i = 0; kmod[i].opt != NULL; i++)
			if (strcmp(tok, kmod[i].opt) == 0) {
				report[0] = report[0] | kmod[i].val;
				break;
			}
		// SHIFT modified keys
		/*for (i = 0; kmod[i].opt != NULL; i++)
			printf("caught\n");
			if (strcmp(tok, kmod[i].opt) == 0) {
				if (kmod[i].val & 0x80) {
					report[0] = report[0] | kmod[i].val;
					break;
				}
			}*/
		if (kmod[i].opt != NULL)
			continue;

		if (key < 6)
			fprintf(stderr, "unknown option: %s\n", tok);
	}
	return 8;
}

static struct options mmod[] = {
	{.opt = "--b1", .val = 0x01},
	{.opt = "--b2", .val = 0x02},
	{.opt = "--b3", .val = 0x04},
	{.opt = NULL}
};

int mouse_fill_report(char report[8], char buf[BUF_LEN], int *hold) {
	char *tok = strtok(buf, " ");
	int mvt = 0;
	int i = 0;
	for (; tok != NULL; tok = strtok(NULL, " ")) {

		if (strcmp(tok, "--quit") == 0)
			return -1;

		if (strcmp(tok, "--hold") == 0) {
			*hold = 1;
			continue;
		}

		for (i = 0; mmod[i].opt != NULL; i++)
			if (strcmp(tok, mmod[i].opt) == 0) {
				report[0] = report[0] | mmod[i].val;
				break;
			}
		if (mmod[i].opt != NULL)
			continue;

		if (!(tok[0] == '-' && tok[1] == '-') && mvt < 2) {
			errno = 0;
			report[1 + mvt++] = (char)strtol(tok, NULL, 0);
			if (errno != 0) {
				fprintf(stderr, "Bad value:'%s'\n", tok);
				report[1 + mvt--] = 0;
			}
			continue;
		}

		fprintf(stderr, "unknown option: %s\n", tok);
	}
	return 3;
}

static struct options jmod[] = {
	{.opt = "--b1",		.val = 0x10},
	{.opt = "--b2",		.val = 0x20},
	{.opt = "--b3",		.val = 0x40},
	{.opt = "--b4",		.val = 0x80},
	{.opt = "--hat1",	.val = 0x00},
	{.opt = "--hat2",	.val = 0x01},
	{.opt = "--hat3",	.val = 0x02},
	{.opt = "--hat4",	.val = 0x03},
	{.opt = "--hatneutral",	.val = 0x04},
	{.opt = NULL}
};

int joystick_fill_report(char report[8], char buf[BUF_LEN], int *hold) {
	char *tok = strtok(buf, " ");
	int mvt = 0;
	int i = 0;

	*hold = 1;

	/* set default hat position: neutral */
	report[3] = 0x04;

	for (; tok != NULL; tok = strtok(NULL, " ")) {

		if (strcmp(tok, "--quit") == 0)
			return -1;

		for (i = 0; jmod[i].opt != NULL; i++)
			if (strcmp(tok, jmod[i].opt) == 0) {
				report[3] = (report[3] & 0xF0) | jmod[i].val;
				break;
			}
		if (jmod[i].opt != NULL)
			continue;

		if (!(tok[0] == '-' && tok[1] == '-') && mvt < 3) {
			errno = 0;
			report[mvt++] = (char)strtol(tok, NULL, 0);
			if (errno != 0) {
				fprintf(stderr, "Bad value:'%s'\n", tok);
				report[mvt--] = 0;
			}
			continue;
		}

		fprintf(stderr, "unknown option: %s\n", tok);
	}
	return 4;
}

void print_options(char c) {
	int i = 0;

	if (c == 'k') {
		printf("	keyboard options:\n"
		       "		hold\n");
		for (i = 0; kmod[i].opt != NULL; i++)
			printf("\t\t%s\n", kmod[i].opt);
		printf("\n	keyboard values:\n"
		       "		[a-z] or [0-9] or\n");
		for (i = 0; kval[i].opt != NULL; i++)
			printf("\t\t%-8s%s", kval[i].opt, i % 2 ? "\n" : "");
		printf("\n");
	} else if (c == 'm') {
		printf("	mouse options:\n"
		       "		--hold\n");
		for (i = 0; mmod[i].opt != NULL; i++)
			printf("\t\t%s\n", mmod[i].opt);
		printf("\n	mouse values:\n"
		       "		Two signed numbers\n\n");
	} else {
		printf("	joystick options:\n");
		for (i = 0; jmod[i].opt != NULL; i++)
			printf("\t\t%s\n", jmod[i].opt);
		printf("\n	joystick values:\n"
		       "		three signed numbers\n"
		       "--quit to close\n");
	}
}

int main(int argc, const char *argv[]) {
	const char *filename = NULL;
	int fd = 0;
	char buf[BUF_LEN];
	int cmd_len;
	char report[8];
	int to_send = 8;
	int hold = 0;
	fd_set rfds;
	int retval, i;

	if (argc < 3) {
		fprintf(stderr, "Usage: %s devname mouse|keyboard|joystick\n",
			argv[0]);

		print_options('k');
		print_options('m');
		print_options('j');

		return 1;
	}

	if (argv[2][0] != 'k' && argv[2][0] != 'm' && argv[2][0] != 'j')
	  return 2;

	filename = argv[1];

	if ((fd = open(filename, O_RDWR, 0666)) == -1) {
		perror(filename);
		return 3;
	}

	while (42) {

		FD_ZERO(&rfds);
		FD_SET(STDIN_FILENO, &rfds);
		FD_SET(fd, &rfds);

		retval = select(fd + 1, &rfds, NULL, NULL, NULL);
		if (retval == -1 && errno == EINTR)
			continue;
		if (retval < 0) {
			perror("select()");
			return 4;
		}

		if (FD_ISSET(fd, &rfds)) {
			cmd_len = read(fd, buf, BUF_LEN - 1);
			printf("recv report:");
			for (i = 0; i < cmd_len; i++)
				printf(" %02x", buf[i]);
			printf("\n");
		}

		if (FD_ISSET(STDIN_FILENO, &rfds)) {
			memset(report, 0x0, sizeof(report));
			cmd_len = read(STDIN_FILENO, buf, BUF_LEN - 1);

			if (cmd_len == 0)
				break;

			buf[cmd_len - 1] = '\0';
			hold = 0;

			memset(report, 0x0, sizeof(report));
			if (argv[2][0] == 'k')
				to_send = keyboard_fill_report(report, buf, &hold);
			else if (argv[2][0] == 'm')
				to_send = mouse_fill_report(report, buf, &hold);
			else
				to_send = joystick_fill_report(report, buf, &hold);

			if (to_send == -1)
				break;

			if (write(fd, report, to_send) != to_send) {
				perror(filename);
				return 5;
			}
			if (!hold) {
				memset(report, 0x0, sizeof(report));
				if (write(fd, report, to_send) != to_send) {
					perror(filename);
					return 6;
				}
			}
		}
	}

	close(fd);
	return 0;
}
