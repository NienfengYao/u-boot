#!/bin/bash
#set -x

UBOOT=$PWD
# Gnerate the cscope.files 
#################################################################################
# Stage 1: Remove the directories of top level
find $UBOOT \
	-path "$UBOOT/arch"  									-prune -o 	\
	-path "$UBOOT/include" 									-prune -o	\
	-path "$UBOOT/nand_spl" 								-prune -o	\
	-path "$UBOOT/onenand_ipl" 								-prune -o	\
	-path "$UBOOT/doc" 										-prune -o	\
	-path "$UBOOT/tools" 									-prune -o	\
	-path "$UBOOT/examples" 								-prune -o	\
	-path "$UBOOT/board" 									-prune -o	\
	-path "$UBOOT/post" 									-prune -o	\
	-name "*.[chsS]" -print > $UBOOT/cscope.files
#################################################################################

#################################################################################
# Stage 2: find the necessary files that removed in Stage 1
#arch/arm/cpu/arm920t/*
#find_dir="$UBOOT/arch/arm/cpu/arm920t"
#find $find_dir \
#	-path "$find_dir/a320"  								-prune -o 	\
#	-path "$find_dir/at91" 									-prune -o	\
#	-path "$find_dir/ep93xx" 								-prune -o	\
#	-path "$find_dir/imx" 									-prune -o	\
#	-path "$find_dir/ks8695" 								-prune -o	\
#	-name "*.[chsS]" -print >> $UBOOT/cscope.files

find_dir="$UBOOT/arch/arm/cpu/armv8"
find $find_dir \
	-name "*.[chsS]" -print >> $UBOOT/cscope.files

find_dir="$UBOOT/arch/arm/cpu/armv7"
find $find_dir \
	-name "*.[chsS]" -print >> $UBOOT/cscope.files

find_dir="$UBOOT/arch/arm/lib"
find $find_dir -name "*.[chsS]" -print >> $UBOOT/cscope.files

find_dir="$UBOOT/arch/arm/include/asm"
find $find_dir -maxdepth 1 -name "*.[chsS]" -print >> $UBOOT/cscope.files

# find include/* expect configs/*
find_dir="$UBOOT/include"
find $find_dir \
	-path "$find_dir/configs"  									-prune -o 	\
	-name "*.[chsS]" -print >> $UBOOT/cscope.files

find_dir="$UBOOT/include/configs/qemu-arm.h"
find $find_dir -print >> $UBOOT/cscope.files

#################################################################################
cscope -bkq -i $UBOOT/cscope.files

#generate the cppcomplete 
ctags -n -f tags --fields=+ai --C++-types=+p * -L $UBOOT/cscope.files

# Try setting the $CSCOPE_DB environment variable to point to a Cscope database
# you create, so you won't always need to launch Vim in the same directory as
# the database.
export CSCOPE_DB=$UBOOT/cscope.out
