#include <stdio.h>
#include <unistd.h>
#include <signal.h>

#include "descriptor_poll.h"
#include "interface_readers/interfaces_register.h"
#include "sniffer_packet_parser.h"
#include "data_collector/rpl_collector.h"

void onAlarm(int a) {
	rpl_collector_dump();
	alarm(3);
}

int main(int argc, char **argv) {
	desc_poll_init();
	interface_register_all();

	if(argc > 1)
		interface_init("telos", argv[1]);
	else interface_init("telos", "/dev/ttyUSB0");

	sniffer_parser_init();
	
	signal(SIGALRM, &onAlarm);
	alarm(3);
	
	while(1) {
		desc_poll_process_events();
	}
}
