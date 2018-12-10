
# mergesort recieves the start array address and the length from &a0 and &a1
# for testing porposes, we will use the "word" command to add the array to the memory
# starting from memory 1024

# for testing purposes we will sort the array 5 10 2 -1 8 1
# &sp will point at the start of the program to memory index 65535 and go down
# we will use the v0 register as another a<x> register

limm $sp, $zero, $zero 2048
.word 1024 5
.word 1025 4
.word 1026 9
.word 1027 3
.word 1028 8
.word 1029 7
.word 1030 25
.word 1031 2
.word 1032 1
limm $a0, $zero, $zero, 1024
limm $a1, $zero, $zero, 9  #length
jal $zero, $zero, $zero, mergesort    #store in a0 a1 array address and length and jump(ra = here)
halt

mergesort:                       # $a0 is array address, $a1 is array length
	add $t0, $a1, $zero, 0          # temp store length
	limm $t1, $zero, $zero, 1
	sub $t1, $t0, $t1, 0 
	add $a1, $a0, $t1, 0            # a1 store adress to end of array
	beq $zero, $zero, $zero, mergesorthelper

mergesorthelper:                 # $a0 is array address, $a1 is array end address  

	limm $t0, $zero, $zero, 4
	sub $sp, $sp, $t0, 0            # make space for 4 local variables on stack
	sw  $ra, $sp, $zero, 0          # return address for recursive calls

	limm $t0, $zero, $zero, 1
	add $t0, $sp, $t0, 0            
	sw  $a0, $t0, $zero, 0          # store start of array on stack

	limm $t0, $zero, $zero, 2
	add $t0, $sp, $t0, 0            
	sw  $a1, $t0, $zero, 0         # store end of array address on stack

	sub $t0, $a1, $a0, 0
	limm $t1, $zero, $zero, 1
	add $t0, $t0, $t1, 0            # num of elements in the array

	ble $zero, $t0, $t1, arrayissorted  # if 1 or 0 elements, the array is sorted
	

	sra $t0, $t0, $t1            # devide num of elements in 2(t1 == 1)
	sub $t1, $t0, $t1, 0
	add $a1, $a0, $t1, 0            # address of the middle of the array minus 1

	limm $t0, $zero, $zero, 3
	add $t0, $sp, $t0, 0            
	sw  $a1, $t0, $zero,  0          # store middle of array minus 1 on stack

	jal $zero, $zero, $zero, mergesorthelper    #call this function on the left sub array

	limm $t0, $zero, $zero, 3
	add $t0, $sp, $t0, 0            
	lw $a0, $t0, $zero, 0          # load middle of array - 1 to a0
	limm $t1, $zero, $zero, 1
	add $a0, $a0, $t1, 0  
	limm $t0, $zero, $zero, 2
	add $t0, $sp, $t0, 0            
	lw $a1, $t0, $zero, 0          # load end of array to a1

	jal $zero, $zero, $zero, mergesorthelper    #call this function on the right sub array

	
	limm $t0, $zero, $zero, 1
	add $t0, $sp, $t0, 0            
	lw $a0, $t0, $zero, 0          # load start of array to a0

	limm $t0, $zero, $zero, 2
	add $t0, $sp, $t0, 0            
	lw $a1, $t0, $zero, 0          # load end of array to a1

	limm $t0, $zero, $zero, 3
	add $t0, $sp, $t0, 0            
	lw $v0, $t0, $zero, 0          # load midlle - 1 of array to v0

	jal $zero, $zero, $zero, mergesortedsubarrays  #merge the sorted sub arrays
	beq $zero, $zero, $zero, arrayissorted



mergesortedsubarrays:           # a0 is start of array, a1 is end of array and v0 is middle-1

	limm $t0, $zero, $zero, 4
	sub $sp, $sp, $t0, 0            # make space for 4 local variables on stack
	sw  $ra, $sp, $zero, 0          # return address for recursive calls

	limm $t0, $zero, $zero, 1
	add $t0, $sp, $t0, 0            
	sw  $a0, $t0, $zero, 0          # store start of array on stack

	limm $t0, $zero, $zero, 2
	add $t0, $sp, $t0, 0            
	sw  $a1, $t0, $zero, 0          # store end of array address on stack

	limm $t0, $zero, $zero, 3
	add $t0, $sp, $t0, 0            
	sw  $v0, $t0, $zero, 0          # store middle-1 of array address on stack

	add $s0, $a0, $zero, 0  
	limm $t0, $zero, $zero, 1
	add $s1, $v0, $t0, 0			 # store start and middle array address
	add $fp, $a1, $t0, 0			 # fp holds pointer to out of array

	# here we simulate as if mem 500 was allocated in heap for us for temp array
	limm $at, $zero, $zero, 500

loopsortedarrays:

											#s0 is the temp_p to first half array and s1 in the temp_p to second half array
	limm $t0, $zero, $zero, 1
	add $t0, $v0, $t0, 0
	beq $zero, $t0, $s0, leftisfinished    #if left pointer is same as middle array

	beq $zero, $s1, $fp, rightisfinished    #if right pointer is out of array


	lw $t3, $s0, $zero, 0         # load start of array value to t3
	lw $s2, $s1, $zero, 0         # load middle of array value to s2

	bgt $zero, $t3, $s2,  rightissmaller
	ble $zero, $t3, $s2,  leftissmaller

rightissmaller:
	sw $t3, $at, $zero, 0
	limm $t0, $zero, $zero, 1
	add $at, $at, $t0, 0  
	add $s0, $s0, $t0, 0  
	beq $zero, $zero, $zero, loopsortedarrays

leftissmaller:
	sw $s2, $at, $zero, 0
	limm $t0, $zero, $zero, 1
	add $at, $at, $t0, 0  
	add $s1, $s1, $t0, 0  
	beq $zero, $zero, $zero, loopsortedarrays


leftisfinished:
	lw $s2, $s1, $zero, 0    #load the value of current pointer
	limm $t0, $zero, $zero, 1
	add $t0, $a1, $t0, 0
	beq $zero, $s1, $fp, botharefinished    #if right pointer is out of array

	sw $s2, $at, $zero, 0
	limm $t0, $zero, $zero, 1
	add $at, $at, $t0, 0  
	add $s1, $s1, $t0, 0  
	beq $zero, $zero, $zero, leftisfinished


rightisfinished:
	lw $t3, $s0, $zero, 0    #load the value of current pointer
	limm $t0, $zero, $zero, 1
	add $t0, $v0, $t0, 0
	beq $zero, $t0, $s0, botharefinished    #if left pointer is same is middle array

	sw $t3, $at, $zero, 0
	limm $t0, $zero, $zero, 1
	add $at, $at, $t0, 0  
	add $s0, $s0, $t0, 0  
	beq $zero, $zero, $zero, rightisfinished

botharefinished:
	limm $at, $zero, $zero, 500

botharefinishedloop:
	beq $zero, $a0, $fp, arrayissorted			#transfer all objects from temp array to old array, if start is out of array we finished

	lw $t0, $at, $zero, 0
	sw $t0, $a0, $zero, 0
	
	limm $t0, $zero, $zero, 1
	add $a0, $a0, $t0, 0
	add $at, $at, $t0, 0
	beq $zero, $zero, $zero, botharefinishedloop


arrayissorted:
	lw $ra, $sp, $zero, 0          # load return address from stack pointer
	limm $t0, $zero, $zero, 4        # fold the stack back up
	add $sp, $sp, $t0, 0
	jr $ra, $zero, $zero, 0
	

	