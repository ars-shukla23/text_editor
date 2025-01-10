#include<termios.h>
#include<ctype.h>
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
  raw_struct.c_iflag &=~(BRKINT|ICRNL|IXON|INPCK|ISTRIP);
  raw_struct.c_oflag &=~(OPOST);
  raw_struct.c_cflag |=(CS8);
  raw_struct.c_lflag &=~(ECHO | ICANON | IEXTEN | ISIG);
  raw_struct.c_cc[VMIN]=0;
  raw_struct.c_cc[VTIME]=1;

  tcsetattr(STDIN_FILENO,TCSAFLUSH,&raw_struct);
}


int main(){
  enableRawMode();
  while(1){
   char c='\0';
    read(STDIN_FILENO,&c,1);
    if(iscntrl(c)){
      printf("%d\r\n",c);
    } 
    else{
     printf("%d ('%c')\r\n",c,c);
    }
    if(c=='q')break;
  }
  return 0;

}
