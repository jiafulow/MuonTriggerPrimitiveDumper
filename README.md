# MuonTriggerPrimitiveDumper

### Instructions

``` bash
export SCRAM_ARCH=slc7_amd64_gcc700
source /cvmfs/cms.cern.ch/cmsset_default.sh
cmsrel CMSSW_11_0_0_patch1
cd CMSSW_11_0_0_patch1/src
cmsenv

# Checkout
git clone git@github.com:jiafulow/MuonTriggerPrimitiveDumper.git MuonTriggerPrimitiveDumper/MuonTriggerPrimitiveDumper

# Build
scram b
```
