#include "descriptor_poll.h"
#include <sys/epoll.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <errno.h>

#define EPOLL_MAX_EVENTS 5

static int epoll_fd;
typedef struct desc_poll_info {
	int fd;
	ready_callback callback;
} desc_poll_info_t;

static desc_poll_info_t *manual_poll[10];
static int manual_poll_size = 0;

void desc_poll_init() {
	epoll_fd = epoll_create(2);
	if(epoll_fd == -1)
		perror("epoll_create");
}

void desc_poll_add(int fd, ready_callback callback) {
	struct epoll_event ev;
	desc_poll_info_t *data = (desc_poll_info_t*) malloc(sizeof(desc_poll_info_t));
	
	data->fd = fd;
	data->callback = callback;
	
	ev.events = EPOLLIN;
	ev.data.ptr = data;
	if(epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &ev) == -1) {
		perror("epoll_ctl");
		
		if(errno == EPERM) {
			assert(manual_poll_size < 10-1);

			manual_poll[manual_poll_size] = data;
			manual_poll_size++;
		}
	}
}

void desc_poll_del(int fd) {
	int i;
	
	for(i = 0; i < manual_poll_size; i++) {
		desc_poll_info_t *data = manual_poll[i];
		if(data && data->fd == fd) {
			free(manual_poll[i]);
			manual_poll[i] = NULL;
		}
	}
	epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, NULL);
}

void desc_poll_process_events() {
	struct epoll_event events[EPOLL_MAX_EVENTS];
	int nbfd, i;
	
	if(manual_poll_size == 0)
		nbfd = epoll_wait(epoll_fd, events, EPOLL_MAX_EVENTS, -1);
	else
		nbfd = epoll_wait(epoll_fd, events, EPOLL_MAX_EVENTS, 0);
	for(i = 0; i < nbfd; i++) {
		desc_poll_info_t *data = (desc_poll_info_t*)events[i].data.ptr;
		data->callback(data->fd);
	}
	
	for(i = 0; i < manual_poll_size; i++) {
		desc_poll_info_t *data = manual_poll[i];
		if(data)
			data->callback(data->fd);
	}
}
