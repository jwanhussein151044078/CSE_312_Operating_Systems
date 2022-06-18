	.data
		table    	: .space 3200            # allocate the space for process table which can hold 20 processes maximum
		regs     	: .space 128             # temporary space to hold the regesters
		curprocess 	: .word 0
		nextprocess	: .word 0
		process1 	: .asciiz "Collatz.s"
		process2 	: .asciiz "BinarySearch.s"
		process3 	: .asciiz "Palindrome.s"
		process4 	: .asciiz "LinearSearch.s"		
		init     	: .asciiz "init"
		end      	: .asciiz "no process left to be execute  !!!!\n"		

	.align 2
	.text
	.globl main
	.globl inturrupt_handle
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
	######################################## load a program into the kernal ##############################################
	addi $sp , $sp , -4                    # allocate space in the stack for the return addres 
	sw $ra , 0($sp)

	
	la $a0 , process4                      # arg a0 holds the name of the program to be loaded to the kernal  
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

inturrupt_handle:
    ################# move the regesters to temporary memory space ##############
	sw $2  , regs+ 0
	sw $3  , regs+ 4 
	sw $4  , regs+ 8
	sw $5  , regs+ 12
	sw $6  , regs+ 16
	sw $7  , regs+ 20
	sw $8  , regs+ 24
	sw $9  , regs+ 28
	sw $10 , regs+ 32
	sw $11 , regs+ 36
	sw $12 , regs+ 40
	sw $13 , regs+ 44
	sw $14 , regs+ 48
	sw $15 , regs+ 52
	sw $16 , regs+ 56
	sw $17 , regs+ 60
	sw $18 , regs+ 64
	sw $19 , regs+ 68
	sw $20 , regs+ 72
	sw $21 , regs+ 76
	sw $22 , regs+ 80
	sw $23 , regs+ 84
	sw $24 , regs+ 88
	sw $25 , regs+ 92
	sw $26 , regs+ 96
	sw $27 , regs+ 100
	sw $28 , regs+ 104
	sw $29 , regs+ 108
	sw $30 , regs+ 112
	sw $31 , regs+ 116
	 
	mfhi $a0
	sw $a0 , regs+ 120
	mflo $a0
	sw $a0 , regs+ 124
	
	#########################find the next process to run #######################
	li $v0 , 22            # syscall to find the id of the next process
	syscall
	sw $v0 , curprocess    # v0 contains the id of the running process   
	sw $v1 , nextprocess   # v1 contains the id of the next process to be run 
	####################  find the addres of the process in the table ###########
	la  $a0 , table
	li  $t0 , 160
	mul $a1 , $v0 , $t0
	add $a0 , $a0 , $a1    # a0 contains the addr of the running process in the table
	######################### set process state ##################################
	li $v0 , 23 
	syscall
	######################### store pc ###########################################
	li $v0 , 24 
	syscall
	##########################	 
	lw $a3 , regs+ 124
	sw $a3 , 24($a0)        # store the high regester to the table
	lw $a3 , regs+ 120
	sw $a3 , 28($a0)        # store the low regester to the table
	########################## store the regester to the table ####################
	lw $a3 , regs+ 0
	sw $a3 , 40($a0)
	lw $a3 , regs+ 4
	sw $a3 , 44($a0)
	lw $a3 , regs+ 8
	sw $a3 , 48($a0)
	lw $a3 , regs+ 12
	sw $a3 , 52($a0)
	lw $a3 , regs+ 16
	sw $a3 , 56($a0)
	lw $a3 , regs+ 20
	sw $a3 , 60($a0)
	lw $a3 , regs+ 24
	sw $a3 , 64($a0)
	lw $a3 , regs+ 28
	sw $a3 , 68($a0)
	lw $a3 , regs+ 32
	sw $a3 , 72($a0)
	lw $a3 , regs+ 36
	sw $a3 , 76($a0)
	lw $a3 , regs+ 40
	sw $a3 , 80($a0)
	lw $a3 , regs+ 44
	sw $a3 , 84($a0)
	lw $a3 , regs+ 48
	sw $a3 , 88($a0)
	lw $a3 , regs+ 52
	sw $a3 , 92($a0)
	lw $a3 , regs+ 56
	sw $a3 , 96($a0)
	lw $a3 , regs+ 60
	sw $a3 , 100($a0)
	lw $a3 , regs+ 64
	sw $a3 , 104($a0)
	lw $a3 , regs+ 68
	sw $a3 , 108($a0)
	lw $a3 , regs+ 72
	sw $a3 , 112($a0)
	lw $a3 , regs+ 76
	sw $a3 , 116($a0)
	lw $a3 , regs+ 80
	sw $a3 , 120($a0)
	lw $a3 , regs+ 84
	sw $a3 , 124($a0)
	lw $a3 , regs+ 88
	sw $a3 , 128($a0)
	lw $a3 , regs+ 92
	sw $a3 , 132($a0)
	lw $a3 , regs+ 96
	sw $a3 , 136($a0)
	lw $a3 , regs+ 100
	sw $a3 , 140($a0)
	lw $a3 , regs+ 104
	sw $a3 , 144($a0)
	lw $a3 , regs+ 108
	sw $a3 , 148($a0)
	lw $a3 , regs+ 112
	sw $a3 , 152($a0)
	lw $a3 , regs+ 116
	sw $a3 , 156($a0)
	####################  find the addres of the process in the table ###########
	la  $a0 , table
	li  $t0 , 160
	mul $a1 , $v1 , $t0
	add $a0 , $a0 , $a1    # a0 contains the addr of the next process in the table
	#################### print  info about next process #######################
	li $v0 , 26
	syscall
	#################### set the next process state ############################
	li $v0 , 23 
	syscall
	#################### load the regesters form the table #####################
	lw $2  , 40($a0)
	lw $3  , 44($a0)
#	lw $4  , 48($a0)
	lw $5  , 52($a0)
	lw $6  , 56($a0)
	lw $7  , 60($a0)
	lw $8  , 64($a0)
	lw $9  , 68($a0)
	lw $10 , 72($a0)
	lw $11 , 76($a0)
	lw $12 , 80($a0)
	lw $13 , 84($a0)
	lw $14 , 88($a0)
	lw $15 , 92($a0)
	lw $16 , 96($a0)
	lw $17 , 100($a0)
	lw $18 , 104($a0)
	lw $19 , 108($a0)
	lw $20 , 112($a0)
	lw $21 , 116($a0)
	lw $22 , 120($a0)
	lw $23 , 124($a0)
	lw $24 , 128($a0)
	lw $25 , 132($a0)
	lw $26 , 136($a0)
	lw $27 , 140($a0)
	lw $28 , 144($a0)
	lw $29 , 148($a0)
	lw $30 , 152($a0)
	lw $31 , 156($a0)

	################### set the pc the the pc addr of the next process ########
	li $v0 , 25
	syscall
	