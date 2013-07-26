#include "types.h"
#include "atags.h"
#include "print.h"
#include "board.h"
#include "led.h"

extern __u32 _binary_input_zImage_start;

void main(__u32 dummy, __u32 machid, const struct tag *tags)
{
	const struct tag *t;
	struct board *board;
	__u32 system_rev = 0;
	void (*start_kernel)(__u32 dummy, __u32 machid, void *dtb) =
		(void *) &_binary_input_zImage_start;

	putstr("++ Impedance Matcher (3rd stage loader) ++\n");

	/* walk the atags to determine the system revision */
	for_each_tag(t, tags)
		switch (t->hdr.tag) {
			case ATAG_REVISION:
				system_rev = t->u.rev.rev;
				break;
		}

	board = match_board(machid, system_rev & 0xff);
	if (!board) {
		putstr("ERROR MATCHING BOARD!\n");
		putstr("MACHID: 0x");
		printhex(machid);
		putstr("\n");
		putstr("SYSTEM_REV: 0x");
		printhex(system_rev);
		putstr("\n");
		led_panic();
	}

	putstr("Detected board: ");
	putstr(board->name);
	putstr("\n");

	putstr("Booting into Linux kernel ...\n");
	start_kernel(0, 0xffffffff, board->dtb);
}
