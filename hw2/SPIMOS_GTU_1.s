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

main :
	######################################## launching the init process ##################################################
	addi $sp , $sp , -4                    # allocate space in the stack for the return addres 
	sw $ra , 0($sp)
										   
	la $a0 , table						   # arg a0 holds the addr of the process table
	la $a1 , init 						   # arg a1 holds the name of init process
	jal launch_init

	lw $ra , 0($sp) 					   # pop the return addres from the stack  
	addi $sp , $sp , 4
	######################################################################################################################
	######################################## load a program into the kernal ##############################################
	addi $sp , $sp , -4                    # allocate space in the stack for the return addres 
	sw $ra , 0($sp)
	
	la $a0 , process1                      # arg a0 holds the name of the program to be loaded to the kernal  
	jal load_program 

	lw $ra , 0($sp) 					   # pop the return addres from the stack  
	addi $sp , $sp , 4
	######################################################################################################################
	######################################## load a program into the kernal ##############################################
	addi $sp , $sp , -4                    # allocate space in the stack for the return addres 
	sw $ra , 0($sp)
	
	la $a0 , process2                      # arg a0 holds the name of the program to be loaded to the kernal  
	jal load_program 

	lw $ra , 0($sp) 					   # pop the return addres from the stack  
	addi $sp , $sp , 4
	######################################################################################################################
	######################################## load a program into the kernal ##############################################
	addi $sp , $sp , -4                    # allocate space in the stack for the return addres 
	sw $ra , 0($sp)
	
	la $a0 , process3                      # arg a0 holds the name of the program to be loaded to the kernal  
	jal load_program 

	lw $ra , 0($sp) 					   # pop the return addres from the stack  
	addi $sp , $sp , 4
	######################################################################################################################
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

load_program:
	
	li $v0 , 18                                  # fork # no argument
	syscall

	li $v0 , 19									 # exeve syscall
	syscall

	jr $ra