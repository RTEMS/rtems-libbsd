#!/usr/bin/env python

#
# Copyright (c) 2018 embedded brains GmbH.  All rights reserved.
#
#  embedded brains GmbH
#  Dornierstr. 4
#  82178 Puchheim
#  Germany
#  <rtems@embedded-brains.de>
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
# 1. Redistributions of source code must retain the above copyright
#    notice, this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright
#    notice, this list of conditions and the following disclaimer in the
#    documentation and/or other materials provided with the distribution.
#
# THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
# ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
# FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
# DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
# OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
# HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
# LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
# OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
# SUCH DAMAGE.
#

import re
import libxml2
from libxml2 import xmlNode
import matplotlib.pyplot as plt
data = open('epoch01.scn').read()
data = re.sub(r'\*\*\*.*\*\*\*', '', data)
doc = libxml2.parseDoc(data)
ctx = doc.xpathNewContext()

plt.yscale('log')
plt.title('Epoch Performance')
plt.xlabel('Active Workers')
plt.ylabel('Operation Count')

def m(n):
	return int(n.getContent())

def getCounterSums(variant):
	w = 1
	y = []
	while True:
		c = map(m, ctx.xpathEval('/TestEpoch01/' + variant + '[@activeWorker="' + str(w) + '"]/Counter'))
		if not c:
			break
		y.append(sum(c))
		w = w + 1
	return y

y = getCounterSums('ThreadLocalMutex')
x = range(1, len(y) + 1)
plt.plot(x, y, label = 'Thread-Local Mutex', marker = 'o')

y = getCounterSums('EnterExit')
plt.plot(x, y, label = 'Enter Exit', marker = 'o')

y = getCounterSums('EnterExitPreempt')
plt.plot(x, y, label = 'Enter Exit Preempt', marker = 'o')

y = getCounterSums('EnterListOpExit')
plt.plot(x, y, label = 'Enter List Op Exit', marker = 'o')

y = getCounterSums('EnterListOpExitPreempt')
plt.plot(x, y, label = 'Enter List Op Exit Preempt', marker = 'o')

plt.legend(loc = 'best')
plt.show()
