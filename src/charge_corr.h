#ifndef CHARGE_CORR_H_
#define CHARGE_CORR_H_

#include "definitions.h"
#include "logic.h"
#include "input.h"

typedef logic::charge_correlation<MAX_OBJ> charge_correlation_logic_type;

void charge_corr(const charge_objects_type& objects, charge_correlation_logic_type& chgcorr_logic);

#endif
