SWEEP := splitkb/aurora/sweep/rev1
QMK_FIRMWARE_ROOT = ${HOME}/Projects/qmk_firmware
QMK_USERSPACE = ${HOME}/Projects/qmk_userspace

sweep:
	toolbox run --container qmk qmk flash -kb ${SWEEP} -km yorickpeterse

sweep-clang:
	toolbox run --container qmk qmk compile --compiledb -kb ${SWEEP} -km yorickpeterse

.PHONY: sweep sweep-clang
