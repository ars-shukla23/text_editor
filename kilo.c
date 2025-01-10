#include<termios.h>
#include<stdio.h>
#include<unistd.h>

void enableRawMode(){
  struct termios raw_struct;
  tcgetattr(STDIN_FILENO,&raw_struct);
  raw_struct.c_lflag &=~(ECHO);
  tcsetattr(STDIN_FILENO,TCAFLUSH,&raw_struct);
}


int main(){
  enableRawMode();
  char c;
  while(read(STDIN_FILENO,&c,1)==1 && c!='q');
  return 0;

}

