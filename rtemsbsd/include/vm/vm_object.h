#ifndef RTEMS_VM_OBJCT_H
#define RTEMS_VM_OBJCT_H

/*
 * Helpers to perform conversion between vm_object page indexes and offsets.
 * IDX_TO_OFF() converts an index into an offset.
 * OFF_TO_IDX() converts an offset into an index.
 * OBJ_MAX_SIZE specifies the maximum page index corresponding to the
 *   maximum unsigned offset.
 */
#define IDX_TO_OFF(idx) (((vm_ooffset_t)(idx)) << PAGE_SHIFT)
#define OFF_TO_IDX(off) ((vm_pindex_t)(((vm_ooffset_t)(off)) >> PAGE_SHIFT))
#define UOFF_TO_IDX(off) OFF_TO_IDX(off)
#define OBJ_MAX_SIZE    (OFF_TO_IDX(UINT64_MAX) + 1)

#endif
