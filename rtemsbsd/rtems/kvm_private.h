#ifndef __KVM_PRIVATE_h
#define __KVM_PRIVATE_h

#include <stdint.h>

typedef struct {
  const char *symbol;
  uintptr_t   value;
} kvm_symval_t;

extern const kvm_symval_t rtems_kvm_symbols[];

#endif
