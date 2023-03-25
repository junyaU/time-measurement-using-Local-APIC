#!/bin/zsh

export PATH=/usr/local/Cellar/dosfstools/4.2/sbin:$PATH
export PATH=/usr/local/opt/llvm/bin:$PATH

WORKING_DIRECTORY=~/sandbox/time-measurement-using-Local-APIC
BOOT_LOADER_PATH=mnt/EFI/BOOT

cd ~/edk2
source edksetup.sh
Build

cd $WORKING_DIRECTORY
source $WORKING_DIRECTORY/build-env.sh
cd kernel
make kernel.elf

cd ~/
if [ -e disk.img ]; then
    rm disk.img
fi
qemu-img create -f raw disk.img 200M
mkfs.fat -n 'URUUNARI' -s 2 -f 2 -R 3 -F 32 disk.img
hdiutil attach -mountpoint mnt disk.img

# Place the bootloader
mkdir -p $BOOT_LOADER_PATH
cp ~/edk2/Build/boot_loaderX64/DEBUG_CLANGPDB/X64/Loader.efi ~/$BOOT_LOADER_PATH
mv ~/$BOOT_LOADER_PATH/Loader.efi ~/$BOOT_LOADER_PATH/BOOTX64.EFI

# Place the kernel
cp $WORKING_DIRECTORY/kernel/kernel.elf mnt
hdiutil detach mnt

qemu-system-x86_64 -m 1G -drive if=pflash,format=raw,file=OVMF_CODE.fd -drive if=pflash,format=raw,file=OVMF_VARS.fd -drive if=ide,index=0,media=disk,format=raw,file=disk.img -device nec-usb-xhci,id=xhci -device usb-mouse -device usb-kbd -monitor stdio -S -gdb tcp::12345
cd $WORKING_DIRECTORY