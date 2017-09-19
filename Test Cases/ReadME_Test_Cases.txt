*******************************************************************************************************

sum.txt ---> Calcualtes sum from 1 to n

mov r0,0										0x0 0x4C000000
mov r2,n=7										0x4 0x4C800008
mov r1,1										0x8 0x4C400001
.loop:
	add r0,r0,r1								0xC 0x00004000
	add r1,r1,1									0x10 0x04440001
	cmp r2,r1									0x14 0x28084000
	bgt .loop									0x18 0x8FFFFFFD

												0x1C 0x00000000

*******************************************************************************************************

fibonacci.txt ----> Calculates n th fibonacci number

mov r0,n=8										0x0 0x4C000008
mov r1,1										0x4 0x4C400001
mov r2,1										0x8 0x4C800001
mov r3,2										0xC 0x4CC00002
.loop:
	mov r4,r2									0x10 0x49008000
	add r2,r2,r1								0x14 0x00884000
	mov r1,r4									0x18 0x48410000
	add r3,r3,1									0x1C 0x04CC0001
	cmp r0,r3									0x20 0x2800C000
	bgt .loop									0x24 0x8FFFFFFB

												0x28 0x00000000

*******************************************************************************************************

power.txt -----> Calculates a ^ b 

mov r0,a=2										0x0 0x4C000002
mov r1,b=5										0x4 0x4C040005
mov r2,1										0x8 0x4C080001
mov r3,1										0xC 0x4CC00001
.loop:
	mul r2,r2,r0								0x10 0x10880000
	add r3,r3,1									0x14 0x04CC0001
	cmp r1,r3									0x18 0x2804C000
	bgt .loop									0x1C 0x8FFFFFFD

												0x20 0x00000000

*******************************************************************************************************

parity.txt ------> Check for odd even using function

.isEven:
	mod r1,r0,2									0xC 0x24400002
	ret											0x10 0xA0000000

.main:
	mov r0,n=5									0x0 0x4C000005
	call isEven									0x4 0x98000002

												0x8 0x00000000

*******************************************************************************************************

foo.txt -------> function inside another function
Assuming sp to be r14

.foo:
	sub sp,sp,4									0xC 0x0FB80004
	st ra,[sp]									0x10 0x7FF80000
	call .goo									0x14 0x98000004
	ld ra,[sp]									0x18 0x77F80000
	add r1,r1,4									0x1C 0x04440004
	ret											0x20 0xA0000000

.goo:
	add r1,r0,2									0x24 0x04400002
	ret											0x28 0xA0000000

.main:
	mov r0,n=3									0x0 0x4C000003
	call .foo									0x4 0x98000002

												0x8 0x00000000

*******************************************************************************************************

Calculate sum of all the elements of array
Assuming address of array to be stored in r13
First storing, A[i] = i, i=0,1,2,....,n
n = 5
Then adding from i = 0 to n (A[i])

mov r0,5										0x0 0x4C000005
mov r1,0										0x4 0x4C400000
.loop:
	mul r2,r1,4									0x8 0x14840004
	add r2,r2,r13								0xC 0x008B4000
	st r1,[r2]									0x10 0x7C480000
	add r1,r1,1									0x14 0x04440001
	cmp r0,r1 									0x18 0x28004000
	bgt .loop									0x1C 0x8FFFFFFB

mov r1,0										0x20 0x4C400000
mov r3,0										0x24 0x4D400000
.loop:		
	mul r2,r1,4									0x28 0x14840004
	add r2,r2,r13								0x2C 0x008B4000
	ld r4,[r2]									0x30 0x75080000
	add r1,r1,1									0x34 0x04440001
	add r3,r3,r4 								0x38 0x00CD0000
	cmp r0,r1 									0x3C 0x28004000
	bgt .loop									0x40 0x8FFFFFFA

												0x44 0x00000000

