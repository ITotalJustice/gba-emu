#include "../arm/data_processing.hpp"

namespace arm7tdmi::arm::data_processing {

static_assert(
    branch_offset(0b1100'1111'1111'1111'1111'1111) == (s32)0b1111'1111'0011'1111'1111'1111'1111'1100 &&
    branch_offset(0b0100'1111'1111'1111'1111'1111) == 0b0000'0001'0011'1111'1111'1111'1111'1100
);

} // namespace arm7tdmi::arm::data_processing
