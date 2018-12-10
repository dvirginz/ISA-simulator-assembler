limm $sp, $zero, $zero, 2048 # initialize the stack
limm $t0, $zero, $zero, 2 # temp value to open slots in the stack
sub $sp, $sp, $t0, 0 #add two items to the stack
sw $s0, $sp, $zero, 1 #store $s0
sw $s1, $sp, $zero, 0 #store $s1
limm $t1, $zero, $zero, 1 # j=1
limm $s0, $zero, $zero, 1024 # base address (for the column)
limm $s1, $zero, $zero, 1024 # current address

OUTERLOOP:
	limm $t0, $zero, $zero, 10 # max value of the table
	bgt $zero, $t1, $t0, END # if j> 10 we are done
	add $t3, $zero, $zero, 0 # current "difference"
	add $s1, $s0, $zero, 0 # the current  address for the column
	limm $t2, $zero, $zero, 1 # i=1
	
INNERLOOP:
		limm $t0, $zero, $zero, 10 # max value of the table
		bgt $zero, $t2, $t0, FINISHINNERLOOP # if i> 10 we are done
		add $t3, $t3, $t1, 0 # curr_val += j
		sw $t3, $s1, $zero, 0 # column[j] = curr_val
		limm $t0, $zero, $zero, 10 
		add $s1, $s1, $t0, 0 # increment the current address by 10
		limm $t0, $zero, $zero, 1 # temporary 1
		add $t2, $t2, $t0, 0 # i++
		beq $zero, $zero, $zero INNERLOOP # back to check the loop condition
		
FINISHINNERLOOP:
	limm $t0, $zero, $zero, 1 
	add $t1, $t1, $t0, 0 # j++
	add $s0, $s0, $t0, 0 # increment the base address for the column
	beq $zero, $zero, $zero, OUTERLOOP # go back to loop to check condition
	
END:
	lw $s0, $sp, $zero, 1 # restore the old value
	lw $s1, $sp, $zero, 0 # restore the old value
	limm $t2, $zero, $zero, 0
	add $sp, $sp, $t0, 0 # restore sp to original value
	halt $zero, $zero, $zero, 0 # we are done
	