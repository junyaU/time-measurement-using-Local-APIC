bits 64
section .text

global receiveFromIO ; uint32_t receiveFromIO(uint32_t addr);
receiveFromIO:
    mov dx, di ; di = addr
    in eax, dx
    ret