/*
 * Copyright (c) 2015 embedded brains GmbH
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _ASM_MPC85XX_H
#define	_ASM_MPC85XX_H

#include <libcpu/powerpc-utility.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define	SVR_SOC_VER(svr) (((svr) >> 8) & 0xfff7ff)
#define	SVR_REV(svr)	((svr) & 0xff)
#define	SVR_MAJ(svr)	(((svr) >> 4) & 0xf)
#define	SVR_MIN(svr)	((svr) & 0xf)

#define	SVR_B4860	0X868000

#define	SPRN_ATBL	FSL_EIS_ATBL
#define	SPRN_ATBU	FSL_EIS_ATBU
#define	SPRN_SVR	FSL_EIS_SVR

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _ASM_MPC85XX_H */
