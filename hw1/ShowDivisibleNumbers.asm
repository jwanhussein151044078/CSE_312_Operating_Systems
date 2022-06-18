	.data
		num1 : .word 0
		num2 : .word 25
		num3 : .word 2 
		msg1 : .asciiz ">> "
		msg2 : .asciiz "\n"

	.text
	.globl main

main :
		la   $t0 , num1		# load the address of the first number into $to
		lw   $t1 , 0($t0)   # t1 = the value of num1
		la   $t0 , num2		# load the address of the first number into $to
		lw   $t2 , 0($t0) 	# t2 = the value of num2
		addi $t2 , $t2 , 1 	# t2 += 1  
		la   $t0 , num3		# load the address of the first number into $to
		lw   $t3 , 0($t0)	# t3 = the value of num3	
		loop :
			beq  $t1 , $t2 , endloop        # if t1 == t2 the end of the loop 
			div  $t1 , $t3			        # lo = t1 / t3 ; hi = t1 % t3 
			mfhi $t4 				        # load hi into $t4
			beq  $t4 , $zero , print_num 	# if $t4 == 0 go and print the value of $t1
			addi $t1 , $t1 , 1 				# t1 += 1
			j loop 						    

		endloop:	
			jr $ra 
print_num :
	la   $a0 , msg1
	li   $v0 , 4
	syscall
	move $a0 , $t1
	li   $v0 , 1
	syscall
	la   $a0 , msg2
	li   $v0 , 4
	syscall
	addi $t1 , $t1 ,1
	j loop