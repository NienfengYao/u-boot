#!/bin/bash
set -x

# qemu-system-aarch64 -machine virt -cpu cortex-a57 -bios u-boot.bin -nographic
qemu-system-aarch64 -machine virt -cpu cortex-a57 -kernel u-boot.elf -nographic
