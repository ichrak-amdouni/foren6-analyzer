/* 
 * File:   interfaces_register.h
 * Author: am
 *
 * Created on June 18, 2013, 2:00 PM
 */

#ifndef INTERFACES_REGISTER_H
#define	INTERFACES_REGISTER_H

#include <stdbool.h>

typedef struct interface {
	const char *interface_name;
	void (*init)(const char *target);
} interface_t;

typedef void (*interface_enum_function_t)(const char* interface_name);
typedef interface_t (*interface_register_function_t)();

void interface_register_all();
bool interface_init(const char *name, const char *target);

#endif	/* INTERFACES_REGISTER_H */

