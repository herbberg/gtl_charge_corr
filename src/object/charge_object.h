#ifndef charge_object_h
#define charge_object_h

#include <ap_int.h>

namespace object {

struct charge_object
{
    typedef ap_uint<2> charge_type;

    charge_type charge;
};

} // namespace object

#endif
