/*
 * File:   interfaces_register.h
 * Author: am
 *
 * Created on June 18, 2013, 2:00 PM
 */

#ifndef INTERFACES_REGISTER_H
#define	INTERFACES_REGISTER_H

#include <stdbool.h>

#ifdef	__cplusplus
extern "C" {
#endif

typedef void * ifreader_t;

typedef struct interface {
	const char *interface_name;
	void (*init)();
	ifreader_t (*open)(const char *target, int channel);
	bool (*start)(ifreader_t handle);
	void (*stop)(ifreader_t handle);
	void (*close)(ifreader_t handle);
} interface_t;

typedef void (*interface_enum_function_t)(const char* interface_name);
typedef interface_t (*interface_register_function_t)();

interface_t *interface_get(const char *name);

#ifdef	__cplusplus
}
#endif

#endif	/* INTERFACES_REGISTER_H */

