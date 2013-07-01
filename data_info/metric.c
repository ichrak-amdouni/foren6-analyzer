#include "metric.h"
#include "../utlist.h"
#include <stdlib.h>
#include <string.h>

static di_metric_type_list_t metrics = NULL;

di_metric_type_t *metric_get_type(const char *name) {
	di_metric_type_el_t *metric_type;

	LL_FOREACH(metrics, metric_type) {
		if(!strcmp(metric_type->type->name, name))
			return metric_type->type;
	}
	
	return NULL;
}

char *metric_to_string(di_metric_t *metric_value) {
	return metric_value->type->to_string(metric_value->value);
}


double metric_get_display_value(di_metric_t *metric_value) {
	return metric_value->type->to_display_value(metric_value->value);
}

void metric_enumerate(metric_enum_callback_t callback) {
	di_metric_type_el_t *metric_type;

	LL_FOREACH(metrics, metric_type) {
		callback(metric_type->type);
	}
}

