#include <cstdio>
#include <fstream>
#include <iostream>
#include <string>

#include "../src/charge_corr.h"

#include "transaction_writer.h"

int main()
{
    charge_objects_type test_data_1 = {1, 2, 2, 0, 3, 2, 2, 3};
    charge_objects_type test_data_2 = {1, 0, 3, 2, 2, 3, 0, 3};
    charge_correlation_logic_type output;

    charge_corr(test_data_1, test_data_2, output);
    
    std::ofstream out_file("charge_corr_test_out.txt"); 
    
    std::cout << "***************************************\n";
    std::cout << "*** Results in charge_corr_test_out.txt\n";
    std::cout << "***************************************\n";
    out_file << "*** Results charge_corr_test:\n"; 
    
    out_file << "test_data_1: {"; 
    for (size_t i = 0; i < MAX_OBJ; ++i)
    {
        if (i != 0) out_file << ", "; 
        out_file << test_data_1[i].charge; 
    }
    out_file << "}\ntest_data_2: {"; 

    for (size_t i = 0; i < MAX_OBJ; ++i)
    {
        if (i != 0) out_file << ", "; 
        out_file << test_data_2[i].charge; 
    }
    out_file << "}\n"; 
                
    for (size_t i = 0; i < MAX_OBJ; ++i)
    {
        for (size_t j = 0; j < MAX_OBJ; ++j)
        {
//             std::cout << "state_double[" << i << "][" << j << "]: " << output.state_double[i][j] <<  " <= charges: " << test_data_1[i].charge << " " << test_data_2[j].charge << "\n";
            out_file << "state_double[" << i << "][" << j << "]: " << output.state_double[i][j] << " <= charges: " << test_data_1[i].charge << " " << test_data_2[j].charge << "\n";
            for (size_t k = 0; k < MAX_OBJ; ++k)
            {
                if (i < j and i < k and j < k)
                {
//                     std::cout << "state_triple[" << i << "][" << j << "][" << k << "]: " << output.state_triple[i][j][k] << " <= charges: " << test_data_1[i].charge << " " << test_data_1[j].charge << " " << test_data_1[k].charge << "\n";
                    out_file << "state_triple[" << i << "][" << j << "][" << k << "]: " << output.state_triple[i][j][k] << " <= charges: " << test_data_1[i].charge << " " << test_data_1[j].charge << " " << test_data_1[k].charge << "\n";
                }
                for (size_t l = 0; l < MAX_OBJ; ++l)
                {
                    if (i < j and i < k and i < l and j < k and j < l and k < l)
                    {
//                         std::cout << "state_quad[" << i << "][" << j << "][" << k << "][" << l << "]: " << output.state_quad[i][j][k][l] << " <= charges: " << test_data_1[i].charge << " " << test_data_1[j].charge << " " << test_data_1[k].charge <<  " " << test_data_1[l].charge << "\n";
                        out_file << "state_quad[" << i << "][" << j << "][" << k << "][" << l << "]: " << output.state_quad[i][j][k][l] << " <= charges: " << test_data_1[i].charge << " " << test_data_1[j].charge << " " << test_data_1[k].charge << " " << test_data_1[l].charge <<  "\n";
                    }
                }
            }
        }
    }
}
