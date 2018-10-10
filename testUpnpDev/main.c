#include "sample_util.h"
#include "tv_ctrlpt.h"

#include <stdarg.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char **argv)
{
	int rc;
	ithread_t cmdloop_thread;
#ifdef WIN32
#else
	int sig;
	sigset_t sigs_to_catch;
#endif
	int code;

	rc = TvCtrlPointStart(linux_print, NULL, 0);
	if (rc != TV_SUCCESS) {
		SampleUtil_Print("Error starting UPnP TV Control Point\n");
		return rc;
	}
	/* start a command loop thread */
	code = ithread_create(&cmdloop_thread, NULL, TvCtrlPointCommandLoop, NULL);
	if (code !=  0) {
		return UPNP_E_INTERNAL_ERROR;
	}
#ifdef WIN32
	ithread_join(cmdloop_thread, NULL);
#else
	/* Catch Ctrl-C and properly shutdown */
	sigemptyset(&sigs_to_catch);
	sigaddset(&sigs_to_catch, SIGINT);
	sigwait(&sigs_to_catch, &sig);
	SampleUtil_Print("Shutting down on signal %d...\n", sig);
#endif
	rc = TvCtrlPointStop();

	return rc;
	argc = argc;
	argv = argv;
}
