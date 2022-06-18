#ifndef utilities_H_
#define utilities_H_

int parse_command_line(int argc , char* argv[] ,int *framesize ,int *numPhysical ,int *numVirtual ,int *pageTablePrintInt ,char **pageReplacement , char **allocPolicy , char **diskFileName );
int power2(int b);

#endif