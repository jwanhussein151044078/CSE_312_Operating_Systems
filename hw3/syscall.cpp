/* SPIM S20 MIPS simulator.
   Execute SPIM syscalls, both in simulator and bare mode.
   Execute MIPS syscalls in bare mode, when running on MIPS systems.
   Copyright (c) 1990-2010, James R. Larus.
   All rights reserved.

   Redistribution and use in source and binary forms, with or without modification,
   are permitted provided that the following conditions are met:

   Redistributions of source code must retain the above copyright notice,
   this list of conditions and the following disclaimer.

   Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation and/or
   other materials provided with the distribution.

   Neither the name of the James R. Larus nor the names of its contributors may be
   used to endorse or promote products derived from this software without specific
   prior written permission.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
   AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
   IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
   ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
   LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
   CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
   GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
   HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
   LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
   OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#ifndef _WIN32
#include <unistd.h>
#endif
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/types.h>

#ifdef _WIN32
#include <io.h>
#endif

#include "spim.h"
#include "string-stream.h"
#include "inst.h"
#include "reg.h"
#include "mem.h"
#include "sym-tbl.h"
#include "syscall.h"

#include "scanner.h"
#include "spim-utils.h"
#include "data.h"
#include <time.h>   

#include <iostream>
using namespace std;

#ifdef _WIN32
/* Windows has an handler that is invoked when an invalid argument is passed to a system
   call. https://msdn.microsoft.com/en-us/library/a9yf33zb(v=vs.110).aspx

   All good, except that the handler tries to invoke Watson and then kill spim with an exception.

   Override the handler to just report an error.
*/

#include <stdio.h>
#include <stdlib.h>
#include <crtdbg.h>


  
void myInvalidParameterHandler(const wchar_t* expression,
   const wchar_t* function, 
   const wchar_t* file, 
   unsigned int line, 
   uintptr_t pReserved)
{
  if (function != NULL)
    {
      run_error ("Bad parameter to system call: %s\n", function);
    }
  else
    {
      run_error ("Bad parameter to system call\n");
    }
}

static _invalid_parameter_handler oldHandler;

void windowsParameterHandlingControl(int flag )
{
  static _invalid_parameter_handler oldHandler;
  static _invalid_parameter_handler newHandler = myInvalidParameterHandler;

  if (flag == 0)
    {
      oldHandler = _set_invalid_parameter_handler(newHandler);
      _CrtSetReportMode(_CRT_ASSERT, 0); // Disable the message box for assertions.
    }
  else
    {
      newHandler = _set_invalid_parameter_handler(oldHandler);
      _CrtSetReportMode(_CRT_ASSERT, 1);  // Enable the message box for assertions.
    }
}
#endif

////////////////////////////////////////////////////////////////////////////////////////
enum state{EMPTY,CREATED,READY,RUNNING,BLOCKED,TERMINATED};
mem_addr table_addr = 0x0;
mem_addr it_addr = 0x0 ;
mem_addr temp_PC ;
int table_size = 0 ;
int running_process = 0 ;
int lock; 
////////////////////////////////////////////////////////////////////////////////////////
/*You implement your handler here*/
void SPIM_timerHandler()
{
   // Implement your handler..
    try{
      if(lock != 0 )return;
      lock = 1 ;
      //throw logic_error( "NotImplementedException\n" );
      //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
      printf("\n/////////////////////////////////////////  context scheduling  /////////////////////////////////////////////////\n");
      
      lab *it_lable=  lookup_label ("inturrupt_handle");
      if(it_lable->addr != 0x0){
        it_addr = it_lable->addr ;
      //  printf("0x%x\n",it_lable->addr);        
      }else{
        printf("no interrupt handler was defined !! \n");
        printf("terminateing the program !!\n");
        exit(-1);
      }
      temp_PC = PC ;                                                                  // save pc value 
      PC = it_addr ;                                                                  // goto interrupt handler
      /////////////////////////////////  save the regesters of the running process to the memory /////////////////////////////////
      //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
      
    }catch ( exception &e ){
      cerr <<  endl << "Caught: " << e.what( ) << endl;
    };
   
}
/* Decides which syscall to execute or simulate.  Returns zero upon
   exit syscall and non-zero to continue execution. */
int
do_syscall ()
{
#ifdef _WIN32
    windowsParameterHandlingControl(0);
#endif

  /* Syscalls for the source-language version of SPIM.  These are easier to
     use than the real syscall and are portable to non-MIPS operating
     systems. */
     //print_table();
  switch (R[REG_V0]){
    
    case PRINT_INT_SYSCALL:
      write_output (console_out, "%d", R[REG_A0]);
      break;

    case PRINT_FLOAT_SYSCALL:{
      float val = FPR_S (REG_FA0);
      write_output (console_out, "%.8f", val);
      break;
    }

    case PRINT_DOUBLE_SYSCALL:{
      write_output (console_out, "%.18g", FPR[REG_FA0 / 2]);
      break;
    }

    case PRINT_STRING_SYSCALL:{
      write_output (console_out, "%s", mem_reference (R[REG_A0]));
      break;
    }

    case READ_INT_SYSCALL:{
      static char str [256];
      read_input (str, 256);
      R[REG_RES] = atol (str);
      break;
    }

    case READ_FLOAT_SYSCALL:{
      static char str [256];
      read_input (str, 256);
      FPR_S (REG_FRES) = (float) atof (str);
      break;
    }

    case READ_DOUBLE_SYSCALL:{
      static char str [256];
      read_input (str, 256);
      FPR [REG_FRES] = atof (str);
      break;
    }

    case READ_STRING_SYSCALL:{
      read_input ( (char *) mem_reference (R[REG_A0]), R[REG_A1]);
      data_modified = true;
      break;
    }

    case SBRK_SYSCALL:{
      mem_addr x = data_top;
      expand_data (R[REG_A0]);
      R[REG_RES] = x;
      data_modified = true;
      break;
    }

    case PRINT_CHARACTER_SYSCALL:{
      write_output (console_out, "%c", R[REG_A0]);
      break;
    }

    case READ_CHARACTER_SYSCALL:{
      static char str [2];
      read_input (str, 2);
      if (*str == '\0') *str = '\n';      /* makes xspim = spim */
      R[REG_RES] = (long) str[0];
      break;
    }

    case EXIT_SYSCALL:{
      //print_table();
      if(running_process == 0){
        spim_return_value = 0;
        return (0);
      }
      else{        
        set_mem_word((running_process*160)+table_addr+8,0x5);
        PC = PC - 0x8 ;
      }      
      break;
    }
    case EXIT2_SYSCALL:{
      spim_return_value = R[REG_A0];	/* value passed to spim's exit() call */
      return (0);
    }

    case OPEN_SYSCALL:{
#ifdef _WIN32
        R[REG_RES] = _open((char*)mem_reference (R[REG_A0]), R[REG_A1], R[REG_A2]);
#else
        R[REG_RES] = open((char*)mem_reference (R[REG_A0]), R[REG_A1], R[REG_A2]);
#endif
      break;
    }

    case READ_SYSCALL:{
	/* Test if address is valid */
      (void)mem_reference (R[REG_A1] + R[REG_A2] - 1);
#ifdef _WIN32
      R[REG_RES] = _read(R[REG_A0], mem_reference (R[REG_A1]), R[REG_A2]);
#else
      R[REG_RES] = read(R[REG_A0], mem_reference (R[REG_A1]), R[REG_A2]);
#endif
      data_modified = true;
      break;
    }

    case WRITE_SYSCALL:{
	/* Test if address is valid */
      (void)mem_reference (R[REG_A1] + R[REG_A2] - 1);
#ifdef _WIN32
      R[REG_RES] = _write(R[REG_A0], mem_reference (R[REG_A1]), R[REG_A2]);
#else
      R[REG_RES] = write(R[REG_A0], mem_reference (R[REG_A1]), R[REG_A2]);
#endif
      break;
    }

    case CLOSE_SYSCALL:{
#ifdef _WIN32
      R[REG_RES] = _close(R[REG_A0]);
#else
      R[REG_RES] = close(R[REG_A0]);
#endif
      break;
    }
    /////////////////////////////////////////////////////
    case FORK:{
      int res = _FORK();
      if(res != 0 ){
        spim_return_value = 0;
        return (0);
      }
      break ;
    }
    case EXECVE:{
      _EXECVE();
      break;
    }

    case WAITPID:{
      
      int arg = R[REG_A0];
      int res = _WAITPID(arg);
      if (res == 1 ){
        PC = PC - 0x4 ;  
      }
      
      break;
    }

    case LAUNCH_INIT:{
      int res = init_table();
      if (res == -1 ){
        spim_return_value = 0;
        return (0);
      }
      
      break;
    }
    case GETRAND:{
      srand(time(0));
      int arr[] = {1,2,3,4};
      int i = (rand()%4);
      R[REG_A0] = arr[i];
      R[REG_A0+1] = arr[(i+1)%4];
      R[REG_A0+2] = arr[(i+2)%4];
      
      break;
    }
    case nextproc :{
      R[REG_V0] = running_process;
      R[REG_V0+1] = doroundrobin();
      running_process = R[REG_V0+1] ;
      break;
    }
    case setprocstate :{
      int state = read_mem_word (R[REG_A0]+8);
      if(state == READY){
        set_mem_word(R[REG_A0]+8,RUNNING);
      }
      else if(state != TERMINATED ){                                                           // check if the process was terminated 
        set_mem_word(R[REG_A0]+8,READY);                                             // set the state to ready 
      }
      break;
    }
    case storepc :{
      set_mem_word(R[REG_A0]+12,temp_PC);
      break;
    }
    case gotonext :{
      R[REG_V0] = read_mem_word(R[REG_A0]+40);
      PC = read_mem_word(R[REG_A0]+12) - 4;
      R[REG_A0] = read_mem_word(R[REG_A0]+48); 
      lock = 0 ;
      break;
    }
    case printinfo :{
      printf("the process '%s' will be running\tprocess_id = %d\n", (char*)mem_reference(read_mem_word(R[REG_A0] + 4)),
                                                                     read_mem_word(R[REG_A0]));
      printf("\tcurrent pc    = 0x%08X",read_mem_word(R[REG_A0]+12) );
      printf("\tstack pointer = 0x%08X",read_mem_word(R[REG_A0]+20));
      printf("\tbase_addr     = 0x%08X\n",read_mem_word(R[REG_A0]+16));
      printf("----------------------------------------------------------------------------------------------------------------\n");
      break;      
    }
    case readchar :{
      char ch;
      char dummy ;
      int i ;
      i = scanf("%c",&ch);
      i = scanf("%c",&dummy);
      R[REG_V0] = (long)ch ;
      break;
    }
    /////////////////////////////////////////////////////

    default:
      run_error ("Unknown system call: %d\n", R[REG_V0]);
      break;
    }

#ifdef _WIN32
    windowsParameterHandlingControl(1);
#endif
  return (1);
}


void
handle_exception ()
{
  if (!quiet && CP0_ExCode != ExcCode_Int)
    error ("Exception occurred at PC=0x%08x\n", CP0_EPC);

  exception_occurred = 0;
  PC = EXCEPTION_ADDR;

  switch (CP0_ExCode)
    {
    case ExcCode_Int:
      break;

    case ExcCode_AdEL:
      if (!quiet)
	error ("  Unaligned address in inst/data fetch: 0x%08x\n", CP0_BadVAddr);
      break;

    case ExcCode_AdES:
      if (!quiet)
	error ("  Unaligned address in store: 0x%08x\n", CP0_BadVAddr);
      break;

    case ExcCode_IBE:
      if (!quiet)
	error ("  Bad address in text read: 0x%08x\n", CP0_BadVAddr);
      break;

    case ExcCode_DBE:
      if (!quiet)
	error ("  Bad address in data/stack read: 0x%08x\n", CP0_BadVAddr);
      break;

    case ExcCode_Sys:
      if (!quiet)
	error ("  Error in syscall\n");
      break;

    case ExcCode_Bp:
      exception_occurred = 0;
      return;

    case ExcCode_RI:
      if (!quiet)
	error ("  Reserved instruction execution\n");
      break;

    case ExcCode_CpU:
      if (!quiet)
	error ("  Coprocessor unuable\n");
      break;

    case ExcCode_Ov:
      if (!quiet)
	error ("  Arithmetic overflow\n");
      break;

    case ExcCode_Tr:
      if (!quiet)
	error ("  Trap\n");
      break;

    case ExcCode_FPE:
      if (!quiet)
	error ("  Floating point\n");
      break;

    default:
      if (!quiet)
	error ("Unknown exception: %d\n", CP0_ExCode);
      break;
    }
}


int init_table(){
  //////////////////////////////////////////// change leble name ////////////////////////////////////////
  lab *main_lable =  lookup_label("main");
  main_lable->name = "kernal";
  ///////////////////////////////////////////////////////////////////////////////////////////////////////
  if (table_addr != 0x0 ){
    printf("ERORR : init has been initialized for the second time \n");
    printf("exiting the kernal !!!!!!!\n");
    return -1 ;
  }
  table_addr = R[REG_A0] ;
  table_size++;
  running_process = 0 ;
  set_mem_word(R[REG_A0],0x0) ;           // set the process id to 0
  set_mem_word(R[REG_A0]+4,R[REG_A1]) ;   // set the process name as pointer to the addr in reg a1 
  set_mem_word(R[REG_A0]+8,0x3) ;         // set the process to running
  set_mem_word(R[REG_A0]+12,PC) ;         // set the process pc to PC  
  set_mem_word(R[REG_A0]+16,0x400000) ;   // set the process base addr
  set_mem_word(R[REG_A0]+20,0x7fffefd4);  // set the process stack pointer base
  set_mem_word(R[REG_A0]+24,LO);          // set the lo reg
  set_mem_word(R[REG_A0]+28,HI);          // set the hi reg
  for(int i = 0 ; i < 128 ; i+=4){        // set the regesters
    set_mem_word(R[REG_A0]+32+i,R[i/4]);
  }
  return 1;
}

int _FORK(){
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  if (table_size == 20){
    printf("there is no space to fork a process !!!!\n");
    return -1 ;
  }
  mem_addr fork_addr = (table_size*160)+table_addr;
  set_mem_word(fork_addr   ,table_size) ;                                   // set the process id to 0
  set_mem_word(fork_addr+4 ,read_mem_word( table_addr+4 ));                 // set the name of process to init 
  set_mem_word(fork_addr+8 ,0x1 );                                          // set the state to created
  set_mem_word(fork_addr+12 ,read_mem_word(fork_addr-144)+ 0x800 );         // set the pc to where the process will be writen 
  set_mem_word(fork_addr+16 ,read_mem_word(fork_addr-144)+ 0x800 );         // set the base addr of the process
  set_mem_word(fork_addr+20 ,read_mem_word(fork_addr-12) - 0x200 );         // set the process sp base 
  set_mem_word(fork_addr+24 ,0x0 );                                         // set lo reg to zero
  set_mem_word(fork_addr+28 ,0x0 );                                         // set hi reg to zero
  for(int i = 0 ; i < 128 ; i+=4){                                          // set the regesters to zero
    set_mem_word(fork_addr+32+i,0x0);
  }
  set_mem_word(fork_addr + 148 ,read_mem_word(fork_addr+20));               // set sp reg  
  set_mem_word(fork_addr + 144 ,read_mem_word(fork_addr-16)+0x400);         // set gp reg         
  table_size++ ;
  
  return 0 ;
}

void _EXECVE(){
  ///////////////////////////////    change the leble name /////////////////////////////
  lab *main_lable=  lookup_label("main");
  if(main_lable){
    char process_lable[50] ;
    char id[20] ;
    strcat(process_lable , "p_");
    sprintf(id ,"%d" , table_size-1);
    strcat(process_lable , id );
    main_lable->name = process_lable ;
  }
  ////////////////////////////////////////////////////////////////////////////////////////

  char *p_path = (char*)mem_reference(R[REG_A0]) ;                // process path

  int where_to_write = 1 ;
  while(read_mem_word((table_addr+(160*where_to_write))+8) != 1 ){      // search the table for the first img of init
    where_to_write++;
  }

  mem_addr exe = table_addr+(160*where_to_write) ;

  increment_text_pc( read_mem_word(exe+16)  - current_text_pc());
  increment_data_pc( read_mem_word(exe+144) - current_data_pc());
  read_assembly_file(p_path);
  pop_scanner();
  set_mem_word(exe+8,0x2);
  set_mem_word(exe+4,R[REG_A0]);  
}

int _WAITPID(int arg){
  //////////////////////////////////////////////////////////////////////////////////////////////////////
  if (arg == -1){
    for(int i = 1 ; i < table_size ; i++){
      if(read_mem_word((i*160)+table_addr+8)!=5 ){              // if any child process is not terminated yet , then keep waitting 
        return 1 ;
      } 
    }
    return 0 ;
  }
  else{
    printf("unimplemented  !!!!!!\n");
    return 0 ;
  }  
  //////////////////////////////////////////////////////////////////////////////////////////////////////
  
}

int doroundrobin(){
  int next = (running_process+1)%table_size; 
  mem_addr next_p_addr = (next*160)+table_addr ;
  mem_addr running_p_addr = (running_process*160) + table_addr ;
  printf("running process was '%s'\tprocess_id = %d\tPC = 0x%08X\n",(char*)mem_reference(read_mem_word(running_p_addr + 4)),
                                                                        read_mem_word(running_p_addr),temp_PC);
  printf("=============================================\n");
  while(read_mem_word(next_p_addr+8) == 0x5){                                       // search for the next unterminated process
    next++;
    next = (next)%table_size ;                                                      // 
    next_p_addr = (next*160)+table_addr;                                            //
  } 
  running_process = next ;
  return next;
}

void print_table(){
  int table_index = 0 , state = -1 ;
  printf("////////////////////////////// process table //////////////////////////////////\n");
    printf("table size       %d\n",table_size);
    printf("running process  %d\n",running_process );
    printf("table_info :: \n");
    for(int i = 0 ; i < table_size ; i++){
      table_index = i*160  ;
      printf("____________________________\n");
      printf("\tprocess id             %d\n", read_mem_word(table_addr + table_index ) );  
      printf("\tprocess name          '%s'\n" , (char*)mem_reference(read_mem_word(  table_addr + table_index + 4  ) ));
      printf("\tprocess pc             0x%08X\n",read_mem_word(table_addr + table_index + 12 ) );
      state = read_mem_word(table_addr + (i*160) + 8 ) ;
      if(state == 0){
        printf("\tprocess state          EMPTY \n ");
      }else if (state == 1 ){
        printf("\tprocess state          CREATED\n ");
      }else if (state == 2){
        printf("\tprocess state          READY\n ");
      }else if (state == 3){
        printf("\tprocess state          RUNNING\n ");
      }else if (state == 4){
        printf("\tprocess state          BLOCKED\n ");
      }else if (state == 5){
        printf("\tprocess state          TERMINATED\n ");
      }else{
        printf("\tprocess state          unknown\n ");
      }
      printf("\tprocess base addr      0x%08X\n", read_mem_word(table_addr + table_index + 16 ));
      printf("\tprocess sp base addr   0x%08X\n", read_mem_word(table_addr + table_index + 20 ));
      printf("\tprocess sp reg         0x%08X\n", read_mem_word(table_addr + table_index + 148 ));
      printf("\tprocess gp reg         0x%08X\n", read_mem_word(table_addr + table_index + 144 ));
      printf("\tprocess LO reg         0x%08X\n", read_mem_word(table_addr + table_index + 24 ));
      printf("\tprocess HI reg         0x%08X\n", read_mem_word(table_addr + table_index + 28 ));

    }
  printf("-------------------------------------------------------------------------------\n");

}