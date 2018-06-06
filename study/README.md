# Study booting procedure of U-Boot

## Build and Test
*	Environment setup
	```
	export PATH=$PATH:/home/ryanyao/work/buildroot-2017.11-rc1/output/host/bin
	export CROSS_COMPILE=aarch64-linux-gnu-
	```

*	how to make
	```
	make clean
	make qemu_arm64_defconfig
	# make menuconfig
	make
	```

*	how to run
	```
	qemu-system-aarch64 -machine virt -cpu cortex-a57 -bios u-boot.bin -nographic
	qemu-system-aarch64 -machine virt -cpu cortex-a57 -kernel u-boot.elf -nographic
	```

---
## Booting procedure study:
*	Selection of Processor Architecture and Board Type (make qemu_arm64_defconfig)
	*	configs/qemu_arm64_defconfig

*	Board Initialisation Flow

---
*	Reference:
	*	u-boot/doc/README.qemu-arm
	*	u-boot/README
