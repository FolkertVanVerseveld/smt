#include <smt/smt.h>

int main(int argc, char **argv) {
	int ret = 1;
	ret = smtInit(&argc, argv);
	if (ret != 0) goto fail;
	ret = 0;
fail:
	return ret;
}
