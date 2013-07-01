#include "rpl_packet_parser.h"
#include "descriptor_poll.h"
#include "interface_readers/interfaces_register.h"
#include <pthread.h>
#include <stdbool.h>
#include <unistd.h>
#include <signal.h>
#include <stdio.h>

static pthread_t poll_thread;
static bool thread_started = false;

static void* rpl_poll_descriptors(void *ptr);

void rpl_tool_set_callbacks(rpl_event_callbacks_t *callbacks) {
	rpl_event_set_callbacks(callbacks);
}


void rpl_tool_init(const char *name, const char *target) {
	desc_poll_init();
	interface_register_all();
	interface_init(name, target);
	sniffer_parser_init();
}

void rpl_tool_start() {
	if(thread_started == false) {
		thread_started = true;
		pthread_create(&poll_thread, NULL, &rpl_poll_descriptors, NULL);
	}
}

void rpl_tool_stop() {
	if(thread_started) {
		thread_started = false;
		pthread_join(poll_thread, NULL);
	}
}

rpl_collector_t *rpl_tool_get_collected_data() {
	return rpl_collector_get();
}

static void* rpl_poll_descriptors(void *ptr) {
	while(1) {
		if(!thread_started) {
			fprintf(stderr, "THREAD STOPPED \n");
			break;
		}

		desc_poll_process_events();
	}
	
	return NULL;
}
