
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
#include <uORB/topics/etri_report.h>

extern "C" __EXPORT int mypub_main(int argc, char *argv[]);

int mypub_main(int argc, char *argv[])
{
	PX4_INFO("mypub start!");

	struct etri_report_s raw;
	memset(&raw, 0, sizeof(raw));
	orb_advert_t etri_pub = orb_advertise(ORB_ID(etri_report), &raw);

	for (int i = 0; i < 1000; i++) {
		raw.x++;
		raw.y++;
		orb_publish(ORB_ID(etri_report), etri_pub, &raw);
		usleep(500000);
	}

	PX4_INFO("exiting");

	return 0;
}
