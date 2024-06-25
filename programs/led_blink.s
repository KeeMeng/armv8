ldr w1, value8
ldr w2, addrFSel
str w1, [w2]

_blinkloop:
ldr w1, value0
ldr w2, value1
add w3,  w1, w2, lsl #21
ldr w4, addrGPSet
str w3, [w4]

ldr w1, value0
ldr w2, loopCount

_stall1: 
add w1, w1, #1
cmp w1, w2
b.lt, _stall1

ldr w1, value0
ldr w2, value1
add w3,  w1, w2, lsl #21
ldr w4, addrGPClear
str w3, [w4]

ldr w1, value0
ldr w2, loopCount

_stall2: 
add w1, w1, #1
cmp w1, w2
b.lt, _stall2

b _blinkloop
	
and x0, x0, x0

value0:
.int 0

value1:
.int 1

value8:
.int 8

loopCount:
.int 1000000

addrFSel:
.int 0x3F200008

addrGPSet:
.int 0x3F20001c

addrGPClear:
.int 0x3F200028
