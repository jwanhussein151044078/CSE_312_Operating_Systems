	.data
		BSarray      : .word 3 ,4 ,6 ,8 ,14 ,65 ,78 , 100
		array_size   : .word 8
		target       : .word 65
		notfound_msg : .asciiz "not found\n"
		found_msg    : .asciiz "found\n"
		output_msg   : .asciiz "in index : "

		
	.text
	.globl main

main :  li $t3 , 0             # 
		la $t0 , array_size    # t0 = addres of array size
		lw $t1 , 0($t0)		   # t1 = mem[array size]
		la $t0 , target  	   # t0 = addres of the target
		lw $t2 , 0($t0)		   # t2 = mem[target]
		li $t5 , 4 
		
		loop:
			li $t6 , 2
			sub  $t7 , $t1 , $t3    			  # t7 = the end point - the start point
			bltz $t7 , endloop					  # if t7 < 0 the target was not found
			add  $t7 , $t3 , $t1				  # t7 = the end point + the start point		
			div  $t7 , $t6						  # lo = t7 / t6 ; t6 = 2
			mflo $t7                			  # t7 = mid
			move $t6 , $t7						  # t6 = mid
			mul  $t7 , $t5 , $t7				  # t7 = t7 * t5 ; t5 = 4 ; to get the actual addres of the mid index of the array 
			la   $a0 , BSarray  				  # 
			add  $a0 , $a0 , $t7    			  # a0 = mem[a0+t7]
			lw   $a0 , 0($a0)       			  #
			beq  $a0 , $t2 , found  			  # if a0 == t2 the target was found
			sub  $t7 , $a0 , $t2                  # t7 = a0 - t2 ; t2 = target value
			bgtz $t7 , search_the_left_half       # if t7 > 0 then the target must be in the left half of the array 
			j search_the_right_half               # else search the right half

		endloop:
			la $a0 , notfound_msg
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
	div  $t7 , $t5						  
	mflo $t7                			  		
	move $a0 , $t7
	li $v0 , 1
	syscall
	
	li $v0 , 10
	syscall

search_the_left_half:				
			move $t1 , $t6			# t1 = mid
			addi $t1 , $t1 , -1     # t1 = mid - 1  ; t1 = the end point 
			j loop 

search_the_right_half:
			move $t3 , $t6          # t3 = mid
			addi $t3 , $t3 , 1 		# t3 = mid + 1  ; t3 = the start point 
			j loop			








