#ifndef gtl_logic_charge_correlation_h
#define gtl_logic_charge_correlation_h

#include "../utils/math.h"
#include "../definitions.h"

namespace logic {

// Charge correlation logic
//
// Example usage:
//
// gtl::logic::charge_correlation<12> chgcorr_logic;
// chgcorr_logic.process(data.muon);
// comb(..., data.muon, chgcorr_logic);
//
template<size_t N>
struct charge_correlation
{
    typedef ap_uint<2> value_type;
    typedef size_t size_type;

    static const size_type size = N;
    static const size_type sign_bit = 0; // charge[0] is sign bit
    static const size_type valid_bit = 1; // charge[1] is valid bit

    typedef value_type double_type[size][size];
    typedef value_type triple_type[size][size][size];
    typedef value_type quad_type[size][size][size][size];

    // States
    enum charge_correlation_type
    {
        IGNORE,
        LIKE_SIGN,
        OPPOSITE_SIGN,
    };

    double_type state_double;
    triple_type state_triple;
    quad_type state_quad;

    // calculates charge state for two objects
    template<typename T>
    value_type state(const T& objects_1, const T& objects_2, size_t a, size_t b)
    {
//         std::cout << "objects_1[" << a << "].charge[valid_bit]: " << objects_1[a].charge[valid_bit] << " objects_2[" << b << "].charge[valid_bit]: " << objects_2[b].charge[valid_bit] << "\n"; 
        if (objects_1[a].charge[valid_bit] and objects_2[b].charge[valid_bit])
        {
            if (objects_1[a].charge[sign_bit] == objects_2[b].charge[sign_bit]) // is like sign?
                return LIKE_SIGN;
            else
                return OPPOSITE_SIGN;
        }
        return IGNORE;
    }

    // calculates charge state for three objects
    template<typename T>
    value_type state(const T& objects_1, const T& objects_2, size_t a, size_t b, size_t c)
    {
        if (objects_1[a].charge[valid_bit] and objects_1[b].charge[valid_bit] and objects_1[c].charge[valid_bit])
        {
            if (objects_1[a].charge[sign_bit] and objects_1[b].charge[sign_bit] and objects_1[c].charge[sign_bit])
            {
                return LIKE_SIGN;
            }
            if (not objects_1[a].charge[sign_bit] and not objects_1[b].charge[sign_bit] and not objects_1[c].charge[sign_bit])
            {
                return LIKE_SIGN;
            }
            else
                return OPPOSITE_SIGN;
        }
        return IGNORE;
    }

    // calculates charge state for four objects
    template<typename T>
    value_type state(const T& objects_1, const T& objects_2, size_t a, size_t b, size_t c, size_t d)
    {
        if (objects_1[a].charge[valid_bit] and objects_1[b].charge[valid_bit] and objects_1[c].charge[valid_bit] and objects_1[d].charge[valid_bit])
        {
            if (objects_1[a].charge[sign_bit] and objects_1[b].charge[sign_bit] and objects_1[c].charge[sign_bit] and objects_1[d].charge[sign_bit])
            {
                return LIKE_SIGN;
            }
            if (not objects_1[a].charge[sign_bit] and not objects_1[b].charge[sign_bit] and not objects_1[c].charge[sign_bit] and not objects_1[d].charge[sign_bit])
            {
                return LIKE_SIGN;
            }
            else
                return OPPOSITE_SIGN;
        }
        return IGNORE;
     }

    template<typename T>
    void process(const T& objects_1, const T& objects_2)
    {
        // Almost insane optimization? Does it gain something?
        using utils::math::bitwidth;
        typedef ap_uint<bitwidth<size>::value> iterator_type;

        // charge bits
        static const size_type sign = 0; // charge[0] is sign bit
        static const size_type valid = 1; // charge[1] is valid bit

        for (iterator_type i = 0; i < size; ++i)
        {
#pragma HLS unroll
            for (iterator_type j = 0; j < size; ++j)
            {
#pragma HLS unroll
                // calculate double charge states
                state_double[i][j] = state(objects_1, objects_2, i, j);

                for (iterator_type k = 0; k < size; ++k)
                {
#pragma HLS unroll
                    // calculate triple charge states
                    state_triple[i][j][k] = state(objects_1, objects_1, i, j, k);

                    for (iterator_type l = 0; l < size; ++l)
                    {
#pragma HLS unroll
                        // calculate quad charge states
                        state_quad[i][j][k][l] = state(objects_1, objects_1, i, j, k, l);
                    }
                }
            }
        }
    }
};

} // namespace logic

#endif
