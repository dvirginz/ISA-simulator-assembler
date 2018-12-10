limm $sp, $zero, $zero, 2048 # initialize the stack
limm $t0, $zero, $zero, 2 # temp value to open slots in the stack
sub $sp, $sp, $t0, 0 #add two items to the stack
sw $s0, $sp, $zero, 1 #store $s0
sw $s1, $sp, $zero, 0 #store $s1
limm $t1, $zero, $zero, 1024 # i = 1024

OUTERLOOP:
	limm $t0, $zero, $zero, 1038 # last cell to compare with it's neighbor
	bgt $zero, $t1, $t0, END # if i> 1038 we are done
	limm $t2, $zero, $zero, 1024 # j = 1024
	
INNERLOOP:
		limm $t0, $zero, $zero, 1038 # max value of the table
		bgt $zero, $t2, $t0, FINISHINNERLOOP # if j> 1038 we are done
		lw $s0, $t2, $zero, 0 # arr[j]
		lw $s1, $t2, $zero, 1 # arr[j+1]
		bgt $zero, $s0, $s1, CONTINUE # if arr[j] > arr[j+1] continue with no swap
		sw $s0, $t2, $zero, 1 # store arr[j] in &arr[j+1]
		sw $s1, $t2, $zero, 0 # store arr[j+1] in &arr[j]
		
CONTINUE:
	limm $t0, $zero, $zero, 1 # temporary 1
		add $t2, $t2, $t0, 0 # j++
		beq $zero, $zero, $zero INNERLOOP # back to check the loop condition
		
FINISHINNERLOOP:
	limm $t0, $zero, $zero, 1 
	add $t1, $t1, $t0, 0 # i++
	beq $zero, $zero, $zero, OUTERLOOP # go back to loop to check condition
	
END:
	lw $s0, $sp, $zero, 1 # restore the old value
	lw $s1, $sp, $zero, 0 # restore the old value
	limm $t2, $zero, $zero, 0
	add $sp, $sp, $t0, 0 # restore sp to original value
	halt $zero, $zero, $zero, 0 # we are done

.word  1024   1438
.word  1025   2460
.word  1026   1540
.word  1027   -2182
.word  1028   406
.word  1029   31
.word  1030   1667
.word  1031   -1542
.word  1032   958
.word  1033   -1109
.word  1034   2250
.word  1035   1585
.word  1036   -1721
.word  1037   1025
.word  1038   2397
.word  1039   -1075