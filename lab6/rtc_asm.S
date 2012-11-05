.global _counter
.global        _periodicHandler
.data
_counter:
    .long 0
    
_periodicHandler:
    movb    $0xC,     %al
    outb    $0x70
    inb     $0x71
    incl    _counter
    movl    _counter,       %eax
    ret