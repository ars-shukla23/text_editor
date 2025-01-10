#include<termios.h>
#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>

struct termios orig_termios;

void disableRawMode(){
  tcsetattr(STDIN_FILENO,TCSAFLUSH,&orig_termios);
}

void enableRawMode(){
  tcgetattr(STDIN_FILENO,&orig_termios);
  atexit(disableRawMode);

  struct termios raw_struct= orig_termios;
  raw_struct.c_lflag &=~(ECHO);
  tcsetattr(STDIN_FILENO,TCSAFLUSH,&raw_struct);
}


int main(){
  enableRawMode();
  char c;
  while(read(STDIN_FILENO,&c,1)==1 && c!='q');
  return 0;

}

