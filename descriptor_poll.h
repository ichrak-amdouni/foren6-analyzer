/* 
 * File:   descriptor_poll.h
 * Author: am
 *
 * Created on June 17, 2013, 1:48 PM
 */

#ifndef DESCRIPTOR_POLL_H
#define	DESCRIPTOR_POLL_H

typedef void (*ready_callback)(int);

void desc_poll_init();

void desc_poll_add(int fd, ready_callback callback);
void desc_poll_del(int fd);

void desc_poll_process_events();

#endif	/* DESCRIPTOR_POLL_H */

