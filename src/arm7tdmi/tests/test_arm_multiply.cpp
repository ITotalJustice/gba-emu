#include "../arm/multiply.hpp"

#include <array>

namespace arm7tdmi::arm::multiply {

constexpr void test_mul() {
    struct Args {
        struct Value {
            u32 cpsr, rm, rs;
        } value;
        mul_result result;
    };

    constexpr auto r = mul<flags_cond::modify>(0, 30, 30);
    static_assert(
        r.result == 30 * 30 &&
        r.cpsr == 0
    );
}

constexpr void test_mla() {
    struct Args {
        struct Value {
            u32 cpsr, rm, rs, rn;
        } value;
        mul_result result;
    };
}

constexpr void test_umull() {
    struct Args {
        struct Value {
            u32 cpsr, rm, rs;
        } value;
        mull_result result;
    };
}

constexpr void test_umlal() {
    struct Args {
        struct Value {
            u32 cpsr, rdlo, rdhi, rm, rs;
        } value;
        mull_result result;
    };
}

constexpr void test_smull() {
    struct Args {
        struct Value {
            u32 cpsr;
            s32 rm, rs;
        } value;
        mull_result result;
    };
}

constexpr void test_smlal() {
    struct Args {
        struct Value {
            u32 cpsr;
            s32 rdlo, rdhi, rm, rs;
        } value;
        mull_result result;
    };
}

// this was a quick test idea for using ref / pointers in constexpr
struct Ref {
    u32 a;
    u32 b;
};

constexpr void test_ref(Ref& r, const u32 a, const u32 b) {
    r.a = a;
    r.b = b;
}

constexpr void test_ref() {
    constexpr auto func = [](){
        Ref ref{};
        test_ref(ref, 33, 69);
        return ref.a == 33 && ref.b == 69;
    };

    static_assert(func());
}

} // namespace arm7tdmi::arm::multiply
