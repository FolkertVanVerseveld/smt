/* create and destroy window */
#include <smt/smt.h>

int main(int argc, char **argv) {
	int ret = 1;
	ret = smtInit(&argc, argv);
	if (ret != 0) goto fail;
	unsigned win;
	ret = smtCreatewin(&win, 400, 300, "test", SMT_WIN_VISIBLE);
	if (ret != 0) goto fail;
	ret = smtFreewin(win);
fail:
	return ret;
}
