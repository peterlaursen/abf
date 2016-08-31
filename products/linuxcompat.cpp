/* $Id$
The cfmakesane function does not exist on Linux. We have therefore taken straight from the FreeBSD source.
We do not own it. The license is below.
*/
#include "linuxcompat.h"
void
cfmakesane(struct termios *t)
{

	t->c_cflag = TTYDEF_CFLAG;
	t->c_iflag = TTYDEF_IFLAG;
	t->c_lflag = TTYDEF_LFLAG;
	t->c_oflag = TTYDEF_OFLAG;
	t->c_ispeed = TTYDEF_SPEED;
	t->c_ospeed = TTYDEF_SPEED;
//	memcpy(&t->c_cc, ttydefchars, sizeof ttydefchars);
}


