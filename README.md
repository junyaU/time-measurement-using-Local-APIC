# Time Measurement Using Local APIC Timer

This is a simple operating system that demonstrates time measurement using the Local APIC Timer and ACPI PM Timer. <br>
The Local APIC Timer used here conforms to the use of xAPIC.

## Demo

https://user-images.githubusercontent.com/61627945/227731633-158b05aa-2f53-49e6-b201-9b4a0c4fb231.mov

## Features

The operating system generates an interrupt every second by measuring the frequency of the Local APIC Timer using ACPI PM Timer.

## Requirements

* qemu-system-x86_64 7.2.0
* C++17
* Clang 15.0.7
* LLD 15.0.7
* EDKⅡ CLANGPDB
* GDB 12.1

## Note

This repository is created for internal study sessions, and it took 40 hours to prepare.
