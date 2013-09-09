/*
 * File:   metric.h
 * Author: am
 *
 * Created on June 20, 2013, 4:17 PM
 */

#ifndef METRIC_H
#define	METRIC_H

#include <stdint.h>

#ifdef	__cplusplus
extern "C" {
#endif

typedef struct di_metric_type {
	const char *name;
	char *(*to_string)(uint64_t value);
	double (*to_display_value)(uint64_t value);
} di_metric_type_t;

typedef struct di_metric_type_el {
	di_metric_type_t *type;
	struct di_metric_type_el *next;
} di_metric_type_el_t, *di_metric_type_list_t;

typedef struct di_metric {
	di_metric_type_t *type;
	uint64_t value;
} di_metric_t;

typedef void (*metric_enum_callback_t)(di_metric_type_t *metric_type);

void metric_add_type(const di_metric_type_t *metric_model);
di_metric_type_t *metric_get_type(const char *name);
char *metric_to_string(const di_metric_t *metric_value);
double metric_get_display_value(const di_metric_t *metric_value);
void metric_enumerate(metric_enum_callback_t callback);

#ifdef	__cplusplus
}
#endif

#endif	/* METRIC_H */

