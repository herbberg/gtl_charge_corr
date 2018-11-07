#include "../src/charge_corr.h"

#include "transaction_writer.h"
#include <cstdio>

int main()
{
    charge_objects_type test_data = {1, 2, 2, 0, 3, 2, 0, 3};
    charge_correlation_logic_type output;

    charge_corr(test_data, output);

    for (size_t i = 0; i < MAX_OBJ; ++i)
    {
        for (size_t j = 0; j < MAX_OBJ; ++j)
        {
            if (i < j)
            {
                std::cout << "state_double[" << i << "][" << j << "]: " << output.state_double[i][j] << "\n";
            }
        }
    }
}
