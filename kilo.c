/*** includes ***/
#include<termios.h>
#include<ctype.h>
#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<errno.h>

/*** defines ***/

#define CTRL_KEY(k) ((k) & 0x1f)



/*** data ***/
struct termios orig_termios;

void die(const char *s){
   perror(s);
   exit(1);
}

/*** terminal ***/
void disableRawMode(){
 if(tcsetattr(STDIN_FILENO,TCSAFLUSH,&orig_termios)==-1)die("tcsetattr");
}

void enableRawMode(){
  if(tcgetattr(STDIN_FILENO,&orig_termios)==-1)die("tcgetattr");
  atexit(disableRawMode);

  struct termios raw_struct= orig_termios;
  raw_struct.c_iflag &=~(BRKINT|ICRNL|IXON|INPCK|ISTRIP);
  raw_struct.c_oflag &=~(OPOST);
  raw_struct.c_cflag |=(CS8);
  raw_struct.c_lflag &=~(ECHO | ICANON | IEXTEN | ISIG);
  raw_struct.c_cc[VMIN]=0;
  raw_struct.c_cc[VTIME]=1;

 if(tcsetattr(STDIN_FILENO,TCSAFLUSH,&raw_struct)==-1)die("tcsetattr");
}

//editorReadKey()'s job is to wait for one keypress and then return it
char editorReadKey(){
  int nread;
  char c;
  while((nread=read(STDIN_FILENO,&c,1))!=1){
    if(nread == -1 && errno != EAGAIN)die("read");
    }
   return c;
}
/*** output ***/
//"/x1b" is the escape character
void editorRefreshScreen(){
   write(STDOUT_FILENO,"\x1b[2J",4);
}



/*** input ***/
//editorProcessKeypress() waits for a keypress and then handles it accordingly
void editorProcessKeypress(){
  char c=editorReadKey();
  switch(c){
   case CTRL_KEY('q'):
    exit(1);
    break;
   }
}


/*** init ***/
int main(){
  enableRawMode();

  while(1){
   editorRefreshScreen();
   editorProcessKeypress();
  }

  return 0;
}
