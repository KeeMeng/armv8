# Part 1 - Emulator

## How the FDE cycle works:

### Fetch:
	We simply copy the 4 bytes in memory at the address given by the PC into the instruction buffer / register. (Not part of the actual processor)

### Decode-Execute:
	Use the op0 flag (bits 28-25 inclusive) to determine the group:
	- 100x = Data Processing Immediate (DPImm)
	- x101 = Data Processing Register (DPReg)
	- x1x0 = Loads and Stores (Single Data Transfer SDT)
	- 101x = Branches
	
	Once the group is determined, we execute the corresponding procedure for said group.

#### Data Processing Immediate:
Get OPI (bits 25 - 23).
Get OPC (bits 30 - 29).
Get sf (bit 31). This is the width of all registers.

If OPI = 010 then arithmetic:
	Imm12 = bits 21 - 10
	rn = bits 9 - 5
	sh = bit 22

	Check OPC:
		00 = Add
		01 = Adds (add and set PSTATE flags - see spec)
		10 = Sub
		11 = Subs (sub and set PSTATE flags - see spec)
	
	We shift Imm12 by 12 bits if sh = 1. We do not shift if sh = 0.

	Let Op2 be Imm12 shifted according to sh. In Rd, we store Rn +- Op2.

If OPI = 101 then wide move:
	Get hw (bits 22 - 21) and imm16 (bits 20 - 5).
	Let op be imm16 << hw

	Check OPC:
		00 = movn (move negated op into Rd)
		10 = movz (set Rd to op)
		11 = movk (move Imm16 into the bits hw * 16 + 16 to
		h2 * 16)

#### Data Processing Register:
Get opr (bits 24 - 21)
Get opc (bits 30 - 29)
Get sf (bit 31)
Get M (bit 28)

Check M and opr
	0 1xx0 = arithmetic
	0 0xxx = bit logic
	1 1000 = multiply

Arithmetic:
	Same as for DPImmi except we perform the operation between Rn and shifted Rm. The result is stored in Rd.

	Check opc:
		00 = add (Rn + Shifted Rm)
		01 = adds (Rn + Shifted Rm, set PSTATE)
		10 = sub (Rn - Shifted Rm)
		11 = subs (Rn - Shifted Rm, set PSTATE)

	This time we use rm and rn as source registers.
	We use rd as the destination register.
	
	We may need to apply a shift to rm.
	The shift type is determiend by bits 2 and 1 of opr.
	The shift amount is determined by operand (bits 15 - 10).

	Check shift type:
		00 = lsl
		01 = lsr
		10 = asr
		11 = ror (logical instructions only)

Bit Logic:
	The same shifting rules apply as previously described for
	Rm.
	
	Get N (bit 21 - bit 0 of opr). When N = 1, bitwise negate the shifted register Rm.

	Check OPC and N:
		00 0 = and = [Rd = Rn & Op]
		00 1 = bic = [Rd = Rn & ~Op]
		01 0 = orr = [Rd = Rn | Op]
		01 1 = orn = [Rd = Rn | ~Op]
		10 0 = eor = [Rd = Rn ^ Op]
		10 1 = eon = [Rd = Rn ^ ~Op]
		11 0 = ands = [Rd = Rn & Op, update PSTATE]
		11 1 = bics = [Rd = Rn & ~Op, update pstate]

Multiply:
	Get x (bit 15 = bit 5 of operand).
	Get ra (bits 14-10 = bits 4-0 of operand).
	Get s (bit 31).
	
	Check s
		0 = madd (multiply add) = [Rd = Ra + (Rn * Rm)]
		1 = msub (multiply sub) = [Rd = Ra - (Rn * Rm)]

	Set Ra = 11111 to use the zero register.

#### Loads and Stores:
In all cases we need to check the size of the value to be loaded by checking sf (bit 30).

Check bit 31:
	0 - single data transfer instruction.
	1 - load literal instruction.

Single data transfer:
	These are used to load and store to/from memory.
	Get L (bit 22) (1 = load, 0 = store)
	Get U (bit 24) (unsigned offset flag)

	Check U
		0 - Check bit 21
			0 - Pre / post index
				Check I (bit 11)
					0 - Post Index
					1 - Pre Index
			1 - Register offset

		1 - Unsigned offset

Load literal instruction:
	Load the literal value into the register Rt.		

#### Branches:
Check bits 31 and 30
	00 - unconditional
	11 - register
	01 - conditional

Unconditional:
	Get bits 25-0 = simm26.
	Offset = simm26 * 4 (sign extended).

Register:
	Xn (bits 9-5) is the address to jump to.

Conditional:
	Get simm19 (bits 23 - 5).
	simm19 * 4 is the offset to be conditionally added.
	Get cond (bits 3-0).
	Check cond:
		0000 = EQ = equal = Z == 1
		0001 = NE = not equal = Z == 0
		1010 = GE = signed greater or equal = N == V
		1011 = LT = signed less than = N != V
		1100 = GT = signed greater than = Z == 0 && N == V
		1101 = LE = signed less that or equal = !(Z == 0 && N == V)
		1110 = AL = always = any
