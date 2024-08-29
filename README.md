# LVGL <-> D/AVE2D integration layer for Alif Ensemble devices

This repository contains the integration layer between LVGL and D/AVE2D Layer 2 driver for Alif Ensemble devices

## Requirements

This CMSIS pack requires some packs to be installed and added to the project:
* [AlifSemiconductor::Dave2DDriver@1.0.1](https://github.com/alifsemi/alif_dave2d-driver)
* [LVGL::lvgl@9.1.0](https://github.com/lvgl/lvgl/tree/v9.1.0/env_support/cmsis-pack)

## How to create and install CMSIS-Pack

1. Make sure CMSIS Toolbox installed. Check `packchk` is available (add CMSIS Toolbox utils path to `PATH` if necessary).
2. Set `CMSIS_PACK_ROOT` environment variables to cmsis-packs installation directory.
3. Run `./gen_pack.sh` script
4. Install generated CMSIS pack by following command:
`cpackget add ./output/AlifSemiconductor.LVGL_DAVE2D.1.0.1.pack`
