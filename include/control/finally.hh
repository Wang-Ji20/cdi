//===------------------------------------------===
// cdi 2023
//
// Identification: include/control/finally.hh
//
// Author: Ji Wang <jiwangcdi@gmail.com>
//
//===------------------------------------------===

#ifndef CDI_CONTROL_FINALLY_HH
#define CDI_CONTROL_FINALLY_HH

namespace cdi::control {

template <class F> struct FinalAction {
  explicit FinalAction(F func) : act(func) {}
  ~FinalAction() { act(); }
  F act;
};

template <class F> [[nodiscard]] auto finally(F func) {
  return FinalAction(func);
}

} // namespace cdi::control

#endif // CDI_CONTROL_FINALLY_HH
