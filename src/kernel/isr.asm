[bits 32]
section .text
    extern exceptionHandler
    global isrStubTable

%macro isrErrorStub 1
isrStub%+%1:
    push %1
    call exceptionHandler
    iret 
%endmacro
%macro isrNoErrorStub 1
isrStub%+%1:
    push 0xffffffff
    push %1
    call exceptionHandler
    iret
%endmacro

isrNoErrorStub 0
isrNoErrorStub 1
isrNoErrorStub 2
isrNoErrorStub 3
isrNoErrorStub 4
isrNoErrorStub 5
isrNoErrorStub 6
isrNoErrorStub 7
isrErrorStub   8
isrNoErrorStub 9
isrErrorStub   10
isrErrorStub   11
isrErrorStub   12
isrErrorStub   13
isrErrorStub   14
isrNoErrorStub 15
isrNoErrorStub 16
isrErrorStub   17
isrNoErrorStub 18
isrNoErrorStub 19
isrNoErrorStub 20
isrNoErrorStub 21
isrNoErrorStub 22
isrNoErrorStub 23
isrNoErrorStub 24
isrNoErrorStub 25
isrNoErrorStub 26
isrNoErrorStub 27
isrNoErrorStub 28
isrNoErrorStub 29
isrErrorStub   30
isrNoErrorStub 31

isrStubTable:
%assign i 0 
%rep    32 
    dd isrStub%+i
%assign i i+1 
%endrep