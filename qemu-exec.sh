#!/bin/zsh

export PATH=/usr/local/Cellar/dosfstools/4.2/sbin:$PATH
export PATH=/usr/local/opt/llvm/bin:$PATH

cd ~/edk2
source edksetup.sh
Build
cd ~/

if [ -e disk.img ]; then
    rm disk.img
fi

qemu-img create -f raw disk.img 200M
mkfs.fat -n 'URUUNARI' -s 2 -f 2 -R 3 -F 32 disk.img
hdiutil attach -mountpoint mnt disk.img

# Place the bootloader
mkdir -p mnt/EFI/BOOT
cp ~/edk2/Build/boot_loaderX64/DEBUG_CLANGPDB/X64/Loader.efi ~/mnt/EFI/BOOT
mv ~/mnt/EFI/BOOT/Loader.efi ~/mnt/EFI/BOOT/BOOTX64.EFI


qemu-system-x86_64 -m 1G -drive if=pflash,format=raw,file=OVMF_CODE.fd -drive if=pflash,format=raw,file=OVMF_VARS.fd -drive if=ide,index=0,media=disk,format=raw,file=disk.img -device nec-usb-xhci,id=xhci -device usb-mouse -device usb-kbd -monitor stdio -S -gdb tcp::12345
cd ~/sandbox/time-measurement-using-Local-APIC