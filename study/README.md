# Study booting procedure of U-Boot (QEMU: Cortex-A57 + virt)

## Build and Test
*	Get code
	```
	git clone https://github.com/NienfengYao/u-boot
	get checkout freertos
	```
*	Environment setup
	```
	export PATH=$PATH:/home/ryanyao/work/buildroot-2017.11-rc1/output/host/bin
	export CROSS_COMPILE=aarch64-linux-gnu-
	```

*	How to make
	```
	make clean
	make qemu_arm64_defconfig
	# make menuconfig
	make
	# make | tee make.log (save log to file)
	```

*	How to run
	```
	qemu-system-aarch64 -machine virt -cpu cortex-a57 -bios u-boot.bin -nographic
	qemu-system-aarch64 -machine virt -cpu cortex-a57 -kernel u-boot.elf -nographic
	```

---
## Debug
*	Show more debug info in default
	```
	Add "#define DEBUG" in begining of include/common.h
	```

*	GDB (Terminal 1/2 should be in the same directory.)
	```
	Terminal 1:
		qemu-system-aarch64 -machine virt -cpu cortex-a57 -kernel u-boot -nographic -S -s
	Terminal 2:
		aarch64-linux-gnu-gdb u-boot
		target remote :1234
	```

---
## Booting procedure study:
*	Selection of Processor Architecture and Board Type (make qemu_arm64_defconfig)
	*	configs/qemu_arm64_defconfig

*	Board Initialisation Flow
	* ./arch/arm/cpu/armv8/start.S

*	main_loop() in common/main.c
	* [TBD]

---
## Reference:
*	U-Boot
	*	./doc/README.qemu-arm
	*	./README
*	GDB
	*	[GDB Debugging ARM U-Boot on QEMU](http://winfred-lu.blogspot.com/2011/12/arm-u-boot-on-qemu.html)
	*	[GNU GDB Debugger Command Cheat Sheet](http://www.yolinux.com/TUTORIALS/GDB-Commands.html)
