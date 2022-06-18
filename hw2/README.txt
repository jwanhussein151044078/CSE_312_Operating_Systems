running instruction :
	*.s and *.asm files should be in the spim dir
	*.cpp and *.h files should be in the CPU dir

	" ./spim -file *.s " command to run the kernel 
	the exceptions.s file is loaded by default

report :
	the LAUNCH_INIT syscall is loading the table with the init process with process id = 0
	the LAUNCH_INIT syscall takes two argument #1: the addr of the table , #2: the addr of the init process name: 
	the table is in the data segment of the memory 
	if any other process made this syscall the program will terminate and print an erorr msg

	the table can hold at maximum 20 processes , the size of the table is 3200 bytes , 160 bytes fo each process
	table field[0] holds the process id 
	table field[1] holds the a pointer to the process name addr , which is data segment also
	table field[2] holds the process state {empty = 0 , created = 1 , ready = 2 , running = 3 , blocked = 4 , terminated = 5 }
	table field[3] holds the process PC
	table field[4] holds the process base addr
	table field[5] holds the process sp base addr
	table field[6] holds the process LO reg
	table field[7] holds the process HI reg
	table field[8:40] holds the process regesters

	the FORK creacting an img of the calling process in the table ,
		initialize the base addr of the img process
		initialize the sp addr of the img process
		initialize the gp reg of the img process 
		sets all other regs to zero for this img process

		only fail if there is no space in the table 

	the EXECVE syscall searchs the table for the first empty slot of the memory then write the process into the memory
					   also changes the globl label name of the process
	
	the WAITPID syscall : takes one argument if the arg == -1 then searchs the table for unterminated process in case of success it 
							decreases the PC by 4 so it keep in its place until the next context sceduling

	when context sceduling :
		the program search the table for an ready process useing Round Robin algorithm 
		save all informations about the running process to the table 
		change the state of the running process to be ready

		then load the regesters and PC belonge to the next process to the actual regesters and PC
		and change the state of the next process to be running  