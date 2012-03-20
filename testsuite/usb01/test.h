/**
 * @file
 *
 * @ingroup demo
 *
 * @brief Test samples.
 */

/*
 * Copyright (c) 2010 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Obere Lagerstr. 30
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#ifndef DEMO_TEST_H
#define DEMO_TEST_H

#include <rtems.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @addtogroup demo
 *
 * @{
 */

void test_file_system(unsigned index, const char *disk_path, const char *mount_path);

/** @} */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* DEMO_TEST_H */
