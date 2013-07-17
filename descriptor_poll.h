/*
 * File:   descriptor_poll.h
 * Author: am
 *
 * Created on June 17, 2013, 1:48 PM
 */

#ifndef DESCRIPTOR_POLL_H
#define	DESCRIPTOR_POLL_H

#include <stdbool.h>

#ifdef	__cplusplus
extern "C" {
#endif

typedef void (*ready_callback)(int, void*);

void desc_poll_init();
void desc_poll_cleanup();

bool desc_poll_add(int fd, ready_callback callback, void* user_data);
void desc_poll_del(int fd);

void desc_poll_process_events();

#ifdef	__cplusplus
}
#endif

#endif	/* DESCRIPTOR_POLL_H */

