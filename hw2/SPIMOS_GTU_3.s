.data
		table    : .space 3200            # allocate the space for process table which can hold 20 processes maximum
		process1 : .asciiz "LinearSearch.asm"
		process2 : .asciiz "BinarySearch.asm"
		process3 : .asciiz "Collatz.asm"
		init     : .asciiz "init"
		end      : .asciiz "no process left to be execute  !!!!\n"	

	.align 2
	.text
	.globl main

main:
	######################################## launching the init process ##################################################
	addi $sp , $sp , -4                    # allocate space in the stack for the return addres 
	sw $ra , 0($sp)
										   
	la $a0 , table						   # arg a0 holds the addr of the process table
	la $a1 , init 						   # arg a1 holds the name of init process
	jal launch_init

	lw $ra , 0($sp) 					   # pop the return addres from the stack  
	addi $sp , $sp , 4
	######################################################################################################################
	####################################### get random number between [1,3] ##############################################
	li $v0 , 21                           # GETRAND syscall
	syscall
	move $t0 , $v0                        # t0 = random1
	move $t1 , $v1                        # t1 = random2
	
		li $t2 , 0
		li $t3 , 3
		li $t4 , 1
		li $t5 , 2
		li $t6 , 3
		li $t7 , 0	
		loop1:
			beq $t2 , $t3 , endloop1
			beq $t0 , $t4 , load_processesA
			beq $t0 , $t5 , load_processesB
			beq $t0 , $t6 , load_processesC

		endloop1:
			li $t7 , 1
			li $t2 , 0
		loop2:
			
			beq $t2 , $t3 , endloop2
			beq $t1 , $t4 , load_processesA
			beq $t1 , $t5 , load_processesB
			beq $t1 , $t6 , load_processesC
			
		endloop2:
	############################### wait all the programs loaded to the kernal to be terminated ##########################
	li $a0 , -1   #wait
	li $v0 , 20
	syscall
	######################################################################################################################
	la $a0 , end
	li $v0 , 4
	syscall
	jr $ra 


launch_init:
	li $v0 , 0
	syscall
	jr $ra

load_processesA:
	li $v0 , 18                                  # fork # no argument
	syscall
	
	la $a0 , process1
	li $v0 , 19									 # exeve syscall
	syscall
	j increament_counter

load_processesB:
	li $v0 , 18                                  # fork # no argument
	syscall
	la $a0 , process2
	li $v0 , 19									 # exeve syscall
	syscall
	j increament_counter

load_processesC:
	li $v0 , 18                                  # fork # no argument
	syscall
	la $a0 , process3
	li $v0 , 19									 # exeve syscall
	syscall	
	j increament_counter

increament_counter:
	addi $t2 , $t2 , 1
	beq  $t7 , $0 , loop1
	j loop2