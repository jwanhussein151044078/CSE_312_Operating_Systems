.data
		table    : .space 3200            # allocate the space for process table which can hold 20 processes maximum
		process1 : .asciiz "Collatz.asm"
		process2 : .asciiz "BinarySearch.asm"
		process3 : .asciiz "LinearSearch.asm"
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
	move $t0 , $v0                        # t0 = random  
	######################################################################################################################
	li $t1 , 0
	li $t2 , 10
	li $t3 , 1
	li $t4 , 2
	li $t5 , 3
	loop:
		beq $t1 , $t2 , endloop 
		beq $t0 , $t3 , load_processA
		beq $t0 , $t4 , load_processB
		beq $t0 , $t5 , load_processC
		
	endloop:

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

load_processA:
	
	li $v0 , 18                                  # fork # no argument
	syscall

	la $a0 , process1
	li $v0 , 19									 # exeve syscall
	syscall

	addi $t1 , $t1 , 1
	j loop

load_processB:
	
	li $v0 , 18                                  # fork # no argument
	syscall

	la $a0 , process2
	li $v0 , 19									 # exeve syscall
	syscall

	addi $t1 , $t1 , 1
	j loop

load_processC:
	
	li $v0 , 18                                  # fork # no argument
	syscall

	la $a0 , process3
	li $v0 , 19									 # exeve syscall
	syscall

	addi $t1 , $t1 , 1
	j loop