/**
 * @file mypub_main.cpp
 * Minimal application example for PX4 autopilot
 *
 * @author jeyong <jeyong@subak.io>
 */
#include <px4_config.h>
#include <px4_tasks.h>
#include <px4_posix.h>
#include <unistd.h>
#include <stdio.h>
#include <poll.h>
#include <string.h>
#include <math.h>

#include <uORB/uORB.h>
#include <uORB/topics/subak_info.h>

extern "C" __EXPORT int mysub_main(int argc, char *argv[]);

int mysub_main(int argc, char *argv[])
{
        PX4_INFO("Start mysub!");

	int mysub_fd = orb_subscribe(ORB_ID(subak_info));

	px4_pollfd_struct_t fds{};
	fds.fd = mysub_fd;
	fds.events = POLLIN;


	for (int i = 0; i < 5; i++) {
		int poll_ret = px4_poll(&fds, 1, 1000);

		if (poll_ret == 0) {
			PX4_ERR("Got no data within a second");
		} else if (poll_ret < 0) {
			PX4_ERR("ERROR return value from poll()");
		} else {
			if (fds.revents & POLLIN) {
				struct subak_info_s raw;
				/* copy sensors raw data into local buffer */
				orb_copy(ORB_ID(subak_info), mysub_fd, &raw);

				PX4_INFO("Get SUBAK_INFO : %d, %d", raw.x, raw.y);

			}
		}	
	}

	PX4_INFO("exiting");

	return 0;
}
