# Part 3 - LED blinking program on a Rasberry Pi

## Program pseudocode

1. initialise
	- initialise registers

2. set LED
	- start of the loop
	- set FSEL 21
    - set address 0x3f20 001c bit 21

3. stall1
	- loop 1000000 times to delay

4. clear
	- set FSEL 21
	- set address 0x3f20 0028 bit 21

5. stall2
	- loop 1000000 times to delay

6. loop
	- branch unconditionally to start of loop
  
7. return 0
