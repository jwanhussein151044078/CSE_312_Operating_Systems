	.data

		linear_search_array : .word 54 ,87 ,5 ,5445 ,12 ,34 ,85 ,156
		array_size : .word 8
		target : .word 5
		found_msg : .asciiz "found\n"
		not_found_msg : .asciiz "not found\n"
		output_msg   : .asciiz "in index : "

	.text
	.globl main

main :		# initialize the array size 
		la  $t0 , array_size  	 # load tha adress of the array size into t0
		lw  $t0 , 0($t0)		 # load the the valu of the array size into t0
		li  $t1 , 4			  	 # load 4 into the reg t1 
		mul $t0 , $t0 , $t1      # t0 = t0 * t1 
			###########################
		li  $t2 , 0          	 # initilize the conter
		la  $t3 , linear_search_array 	# initialize the array address
		la  $t4 , target 				# t4 = the adrress of the number to be searching for  
		lw  $t4 , 0($t4) 				# t4 = the number to be searching for
		loop :
			beq $t2 , $t0 , endloop  	# if t2 == t0 go to the end loop 
			lw $a0 , 0($t3)				# load the integer in the address t3 into a0
			beq $t4 , $a0 , found 		# if t4 == a0 the target was found go to endloop 
			addi $t3 , $t3 , 4 			# t3 += 4
			addi $t2 , $t2 , 4 			# t2 += 4
			j loop 						# jump to loop

		endloop :                   
			la $a0 , not_found_msg 
			li $v0 , 4
			syscall
			la $a0 , output_msg
			li $v0 , 4
			syscall
			li $a0 , -1 
			li $v0 , 1
			syscall
			li $v0 , 10
			syscall 

found : 
	la $a0 , found_msg 
	li $v0 , 4
	syscall
	la $a0 , output_msg
	li $v0 , 4
	syscall
	div  $t2 , $t1						  
	mflo $t2
	move $a0 , $t2
	li $v0 , 1
	syscall
	li $v0 , 10
	syscall