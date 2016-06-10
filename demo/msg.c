/* show different types of prompts */
#include <smt/smt.h>

int main(int argc, char **argv) {
	int ret = 1;
	/* as you can see, we can use smtMsg even before smtInit */
	smtMsg(SMT_MSG_INFO, 0, "hello", "this is a prompt");
	ret = smtInit(&argc, argv);
	if (ret != 0) goto fail;
	smtMsg(SMT_MSG_ERR, 0, "test", "this is an error prompt");
	smtMsg(SMT_MSG_WARN, 0, "goodbye", "this is a warning prompt");
fail:
	return ret;
}
