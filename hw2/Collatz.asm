	.data
		MAX          : .word 25
		CUR          : .word 1
		newline_msg  : .asciiz "\n"
		_msg         : .asciiz " : "
		space_msg    : .asciiz " "
		
	.text
	.globl main

main :
	li $s0 , 26
	li $s1 , 1

	li    $t9 , 1
	li    $t8 , 2
	li    $t6 , 3

	loop : 
		beq $s1 , $s0 , exit
		####################################### print the curent number ###########################
		addi $sp , $sp , -4                    # allocate space in the stack for the return addres 
		sw $ra , 0($sp)    

		jal print_cur

		lw $ra , 0($sp) 					   # pop the return addres from the stack  
		addi $sp , $sp , 4
		###########################################################################################
		addi $sp , $sp , -4                    # allocate space in the stack for the return addres 
		sw $ra , 0($sp)

		move $t0 , $s1 
		jal Collatz

		lw $ra , 0($sp) 					   # pop the return addres from the stack  
		addi $sp , $sp , 4

		######################################
		la $a0 , newline_msg
		li $v0 , 4 
		syscall
		######################################
		addi $s1 , $s1 , 1
		j loop
	
	exit :
		li $v0 , 10
		syscall



print_cur :
	move $a0 , $s1
	li $v0 , 1  # print int syscall
	syscall
	la $a0 ,  _msg
	li $v0 , 4  # print string syscall
	syscall

	jr $ra


Collatz :
	beq   $t0 , $t9 , print_number 
	div   $t0 , $t8
	mfhi  $t3 
	slti  $t7 , $t3 , 1
	
	beq   $t7 , $t9 , even
	
	mult  $t0 , $t6 
	mflo  $t0
	addi  $t0 , $t0 , 1

	move  $a0 , $t0 
	li    $v0 , 1
	syscall
	la    $a0 , space_msg 
	li    $v0 , 4
	syscall
	
	j Collatz


even : 
	div   $t0 , $t8
	mflo  $t0

	beq   $t0 , $t9 , print_number 
	
	move  $a0 , $t0 
	li    $v0 , 1
	syscall
	la    $a0 , space_msg 
	li    $v0 , 4
	syscall
	
	j Collatz



print_number : 
	move $a0 , $t0 
	li $v0 , 1 
	syscall
	la $a0 , space_msg 
	li $v0 , 4
	syscall
	
	jr $ra 