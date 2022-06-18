	.data
		array      : .word 85 ,23 ,654 ,97 ,4 ,58 ,456 ,147 ,12 , 85 , 456 , 24 , 852 , 963 , 158 
		array_size : .word 15
		msg1       : .asciiz "the original array  >>  "
		msg2       : .asciiz "the sorted array    >>  "
		msg3       : .asciiz "  "
		msg4       : .asciiz "\n" 
	.text
	.globl main

main :		###############   print unsorted array ########
		addi $sp , $sp , -4                    # allocate space in the stack for the return addres 
		sw $ra , 0($sp)         			   # push the return addres to the stack
		jal print_the_orijinal_array 		
		lw $ra , 0($sp) 					   # pop the return addres from the stack  
		addi $sp , $sp , 4 					   # free the allocated speace
			###############################################
			###############  sorting the array ############ 
		addi $sp , $sp , -4
		sw $ra , 0($sp)         
		jal sort 		
		lw $ra , 0($sp)
		addi $sp , $sp , 4
			###############################################
			###############   print the sorted array ######
		addi $sp , $sp , -4
		sw $ra , 0($sp)         
		jal print_the_sorted_array 		
		lw $ra , 0($sp)
		addi $sp , $sp , 4
			###############################################


		jr $ra 


print_the_orijinal_array:
	la    $a0 , msg1
	li    $v0 , 4
	syscall
	addi  $sp , $sp , -4
	sw    $ra , 0($sp)         
	jal print_array
	lw    $ra , ($sp)
	addi  $sp , $sp , 4			
	jr    $ra

print_the_sorted_array:
	la    $a0 , msg2
	li    $v0 , 4
	syscall
	addi  $sp , $sp , -4
	sw    $ra , 0($sp)         
	jal print_array
	lw    $ra , ($sp)
	addi  $sp , $sp , 4			
	jr    $ra	

print_array:
	lw $t0 , array_size     # init the counter
	li $t1 , 0 
	loop:
		beq  $t0 , $t1 , endloop
		la   $a0 , array
		mul  $t1 , $t1 , 4 
		add  $a0 , $a0 , $t1
		lw   $a0 , 0($a0)
		li   $v0 , 1
		syscall
		div  $t1 , $t1 , 4 
		addi $t1 , $t1 , 1
		la   $a0 , msg3
		li   $v0 , 4
		syscall
		j loop

	endloop:
		la   $a0 , msg4
		li   $v0 , 4
		syscall		
		jr   $ra


sort:
	li   $t0 , 0                           # t0 = counter for first loop
	lw   $t1 , array_size                  # t1 = array size
	la   $s0 , array                       # s0 = the addres of the array (const)
	move $s4 , $s0                         # s4 = the addres of the array
	loop1:
		beq  $t0 , $t1 , endloop1
		addi $t2 , $t0 , 1                 # t2 = counter for the second loop
		move $t3 , $t0                     # t3 = the index of the min element
		lw   $s3 , 0($s4)                  # s3 = min value in the unsorted part of the array 
		loop2:
			beq  $t2 , $t1 , endloop2
			mul  $t2 , $t2 , 4             # t2 = t2 * 4
			add  $t4 , $t2 , $s0           # t4 = (addres) t2 + s0 
			lw   $t4 , 0($t4)              # t4 = mem[t4]
			bgt  $s3 , $t4 , update_min_index
			div  $t2 , $t2 , 4             # t2 = t2 / 4
			addi $t2 , $t2 , 1             # t2++
			j loop2

		endloop2:
			j swap
	

	endloop1:	
		jr $ra 

update_min_index:
	move $s3 , $t4
	div  $t2 , $t2 , 4
	move $t3 , $t2
	addi $t2 , $t2 , 1
	j loop2

swap:           ## t0 and t3 indexes are to be swapped
	mul  $t8 , $t0 , 4
	add  $s1 , $s0 , $t8     ## addres of the first element 
	lw   $t8 , 0($s1)        ## the value of the first element 
	mul  $t9 , $t3 , 4
	add  $s2 , $s0 , $t9     ## addres of the second element 
	lw   $t9 , 0($s2)        ## the value of the second element
	sw   $t8 , 0($s2)        ## mem[s2] = t8
	sw   $t9 , 0($s1)        ## mem[s1] = t9
	
	addi $s4 , $s4 , 4 	           # s1 += 4
	addi $t0 , $t0 , 1 			   # t0 += 1
	j loop1 