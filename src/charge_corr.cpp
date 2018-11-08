#include "charge_corr.h"

void charge_corr(const charge_objects_type& objects_1, const charge_objects_type& objects_2, charge_correlation_logic_type& chgcorr_logic)
{
#pragma HLS INTERFACE ap_none port=objects_1
#pragma HLS INTERFACE ap_none port=objects_2
#pragma HLS INTERFACE ap_none port=chgcorr_logic
#pragma HLS INTERFACE ap_ctrl_none port=return
#pragma HLS ARRAY_PARTITION variable=objects_1 complete dim=0
#pragma HLS ARRAY_PARTITION variable=objects_2 complete dim=0
#pragma HLS ARRAY_PARTITION variable=chgcorr_logic complete dim=0
    chgcorr_logic.process(objects_1, objects_2);
}
