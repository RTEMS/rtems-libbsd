#ifndef RTEMS_VM_PAGER_H
#define RTEMS_VM_PAGER_H

/*
 * get/put return values
 * OK    operation was successful
 * BAD   specified data was out of the accepted range
 * FAIL  specified data was in range, but doesn't exist
 * PEND  operations was initiated but not completed
 * ERROR error while accessing data that is in range and exists
 * AGAIN temporary resource shortage prevented operation from happening
 */
#define VM_PAGER_OK     0
#define VM_PAGER_BAD    1
#define VM_PAGER_FAIL   2
#define VM_PAGER_PEND   3
#define VM_PAGER_ERROR  4
#define VM_PAGER_AGAIN  5

#endif
