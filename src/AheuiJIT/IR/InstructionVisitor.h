#pragma once
#include <AheuiJIT/IR/Instruction.h>

namespace aheuijit {

template <typename T, size_t N, typename... Args, size_t... I>
void callVisitorImpl(T &obj, void (T::*mf)(Args...), const std::array<Value *, N> &args,
                     std::index_sequence<I...>) {
    return (obj.*mf)(args[I]...);
}

template <typename T, auto mf, size_t N>
auto callVisitor(T &obj, const std::array<Value *, N> &args) {
    callVisitorImpl(obj, mf, args, std::make_index_sequence<N>{});
}

template <typename... Args>
struct SizeGetter {
    constexpr static size_t size = sizeof...(Args);
};

template <typename Impl>
struct InstructionVisitorCaller {
    void call(Impl &impl, Instruction *inst) {
        switch (inst->opcode) {
#define INST(name, ret, ...)                                                                       \
    case Opcode::name:                                                                             \
        _call##name(impl, inst);                                                                   \
        break;
#include "Instruction.inc"
#undef INST
        }
    }

#define INST(name, retr, ...)                                                                      \
    void _call##name(Impl &impl, Instruction *inst) {                                              \
        std::array<Value *, SizeGetter<__VA_ARGS__>::size> tmp;                                    \
        for (int i = 0; i < SizeGetter<__VA_ARGS__>::size; ++i) {                                  \
            tmp[i] = inst->args[i];                                                                \
        }                                                                                          \
        callVisitor<Impl, &Impl::name, SizeGetter<__VA_ARGS__>::size>(impl, tmp);                  \
    }
#include "Instruction.inc"
#undef INST
};

}  // namespace aheuijit
