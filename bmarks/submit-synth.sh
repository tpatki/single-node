#!/bin/bash

#cd synthetic/no-scale-cpu
#msub rapl-runs-small.msub

#cd ../scale-cpu
#msub rapl-runs-small.msub

#cd synthetic/scale-mem
#msub rapl-runs-small.msub

cd synthetic/no-scale-mem-comm
msub rapl-runs-small.msub


cd ../no-scale-cpu-comm
msub rapl-runs-small.msub


#cd ../scale-cpu
#msub rapl-runs-small.msub

#cd ../turbo-tests
#msub script.msub
