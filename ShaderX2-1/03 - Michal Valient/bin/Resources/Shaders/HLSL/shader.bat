@echo off
call fx2vsh CDX9_MaterialCookTorrance >fxc.log
call fx2psh CDX9_MaterialCookTorrance >>fxc.log

call fx2vsh CDX9_MaterialDiffuseSpecularSpot.2_0 >>fxc.log
call fx2psh CDX9_MaterialDiffuseSpecularSpot.2_0 >>fxc.log

call fx2vsh CDX9_MaterialOrenNayar >>fxc.log
call fx2psh CDX9_MaterialOrenNayar >>fxc.log

call fx2vsh CDX9_MaterialReflective2 >>fxc.log
call fx2psh CDX9_MaterialReflective2 >>fxc.log
