	.data
		##########  every word in the dictionary must end with (:) , a word length can be at maximum 256 bytes ############
		dictionary   	: .asciiz "ada:what:aba:how:what:that:go:CSE:315:book:do:pen:hi:they:dont:know:will:should:went:gone:random:CD:DVD:gebze:istanbul:izmit:ankara:I:me:you:wait:fork:exe:print:hello:world:dont:shop:work:HW:homework:home:drone:lion:tiger:call:STM32:ESP8266:keybord:mouth:digit:alpha:beta:segma:Xray:car:vicle:like:ice:fire:wrong:class:C:C++:java:python:human:animal:pet:boy:girl:friend:Gmail:hotmail:yahoo:WWW:HTML:1234321:spider:super:am:is:are:was:were:have:had:has:ASCII:code:to:from:form:far:away:abcddcba:close:open:bob:charecter:"
		temp 			: .space 257   # temporary buffer to hold the words including null character
		SIZE         	: .word 100	   # number of word in the dictionary 
		start 			: .word 0 # will be treated like a pointer which will point to the first character of all words in the dictionary one by one 
		newline_msg  	: .asciiz "\n"
		_semicolon      : .asciiz ":"
		_msg	    	: .asciiz "do you want to continue(y/n)\n"
		_Palindrome	 	: .asciiz "Palindrome\n"
		_Not_Palindrome : .asciiz "Not Palindrome\n"
		_EXIT			: .asciiz "Goodbye...\n"
	
	.text
	.globl main

main:
	li $s0 , 0      			# s0 = 0 
	lw $s1 , SIZE   			# s1 = size of the dictionary
	la $s2 , dictionary 		# s2 = addr of the dictionary
	la $s4 , temp
	
	loop:
		beq $s1 , $s0 , continue			# go to continue lable if s1 == s0
		addi $a0 , $s0 , 1
		li $v0 , 1
		syscall
		la $a0 , _semicolon
		li $v0 , 4
		syscall
		##################################################
		addi $sp , $sp , -4                    # allocate space in the stack for the return addres 
		sw $ra , 0($sp)    
		lw $s3 , start
		la $s4 , temp
		 
		li $t0 , 0
		jal findlen 						   # find the length of the word and copy the word to the temporary buffer

		lw $ra , 0($sp) 					   # pop the return addres from the stack  
		addi $sp , $sp , 4
		##################################################
		la $a0 , temp 
		li $v0 , 4
		syscall
		la $a0 , _semicolon 
		li $v0 , 4
		syscall
		##################################################
		addi $sp , $sp , -4                    # allocate space in the stack for the return addres 
		sw $ra , 0($sp)    
		
		jal isPalindrome					   # check if the word in the temporary buffer is palindrome or not 

		lw $ra , 0($sp) 					   # pop the return addres from the stack  
		addi $sp , $sp , 4
		##################################################				
	#	la $a0 , newline_msg
	#	li $v0 , 4
	#	syscall

		addi $s0 , $s0 , 1           		# increaseing the counter 		
		j loop

findlen:                                    # find the length of the world and 
	lb $a0 , 0($s2)							# read a byte from the dictionary
	addi $a0 , $a0 , -58					#
	bnez $a0 , fillstr						# check if the read byte is (:)
	addi $s2 , $s2 , 1
	addi $s3 , $s3 , 1
	sw $s3 , start
	sb $zero , 0($s4)
	jr $ra

fillstr: 									# copy the word to the buffer 
	addi $a0 , $a0 , 58
	sb $a0 , 0($s4)
	addi $s4 , $s4 , 1	
	addi $s2 , $s2 , 1
	addi $s3 , $s3 , 1
	addi $t0 , $t0 , 1	
	j findlen

isPalindrome:								# chech if the word inside the buffer is palindrome or not
	li $t1 , 0 	
	addi $t0 , $t0 , -1 					# t0 = length of the word 	

	loop2:

		bge $t1 , $t0 , endloop2			# while (t0 > t1)
		la $s4 , temp 						# 
		add $s4 , $s4 , $t0 				#
		lb  $t5 , 0($s4) 					# t5 = buffer[t0]
		la $s4 , temp 						#
		add $s4 , $s4 , $t1                 #
		lb  $t6 , 0($s4)                    # t6 = buffer[t1]

		bne $t5 , $t6 , not_palindrome      # if(t5 != t6) the word is not palindrome 

		addi $t1 , $t1 , 1
		addi $t0 , $t0 , -1
		j loop2
	endloop2:
		la $a0 , _Palindrome
		li $v0 , 4
		syscall
	jr $ra

not_palindrome:
	la $a0 , _Not_Palindrome
	li $v0 , 4
	syscall
	jr $ra


continue:                                  # after processing the words in the dictionary 
	la $a0 , _msg
	li $v0 , 4 
	syscall
	
	li $v0 , 27                            # read a char  
	syscall
	move $a0 , $v0	
#	lb $a0 , 0($a0)
	addi $a0 , $a0 , -121
	beqz $a0 , continue2                   # if the char is n go to exit
	j exit 

continue2:
	#li $v0 , 12                     # 
	#syscall                         # 
	
	la $a0 , temp                          # read the string 
	li $a1 , 256						   
	li $v0 , 8
	syscall
	la $s4 , temp
	li $t0 , 0
	li $t7 , 10

	addi $a0 , $s0 , 1
	li $v0 , 1
	syscall
	la $a0 , _semicolon 
	li $v0 , 4
	syscall
	
	loop3 :                              # find the word length 
		lb $t2 , 0($s4)
		beq $t2 , $t7 , del_nl
		addi $s4 , $s4 , 1
		addi $t0 , $t0 , 1
		
		j loop3
	
del_nl:                                  # delete the new line character from the input string
	
	sb $zero , 0($s4)
	la $a0 , temp 
	li $v0 , 4
	syscall
	la $a0 , _semicolon 
	li $v0 , 4
	syscall
	addi $sp , $sp , -4                    # allocate space in the stack for the return addres 
	sw $ra , 0($sp)    
	jal isPalindrome
	lw $ra , 0($sp) 					   # pop the return addres from the stack  
	addi $sp , $sp , 4
	j exit
	
exit :
	la $a0 , _EXIT
	li $v0 , 4
	syscall
	li $v0 , 10
	syscall