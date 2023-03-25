# Time measurement using Local_APIC_Timer

Measure 1 second using Local_APIC_Timer and ACPI_PM_Timer.
※The Local APIC Timer used here conforms to the use of xAPIC 😇

# DEMO



# Features

Implemented to generate an interrupt every second by measuring the frequency of the Local Apic Timer using ACPI PM Timer.

# Requirement

* qemu-system-x86_64 7.2.0
* C++ c++17
* Clang 15.0.7
* LLD 15.0.7
* EDKⅡ CLANGPDB
* gdb 12.1

# Note

Repository for internal study sessions.
It took 40 hours to prepare this repository for the study sessions💰