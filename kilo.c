/*** includes ***/
#include<termios.h>
#include<ctype.h>
#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<errno.h>
#include<sys/ioctl.h>

/*** defines ***/

#define CTRL_KEY(k) ((k) & 0x1f)



/*** data ***/
struct editorConfig {
 int screenrows;
 int screencols;
 struct termios orig_termios;
};

struct editorConfig E;

/*** terminal ***/
void die(const char *s){
   write(STDOUT_FILENO,"\x1b[2J",4);
   write(STDOUT_FILENO,"\x1b[H",3);
   perror(s);
   exit(1);
}

void disableRawMode(){
 if(tcsetattr(STDIN_FILENO,TCSAFLUSH,&E.orig_termios)==-1)die("tcsetattr");
}

void enableRawMode(){
  if(tcgetattr(STDIN_FILENO,&E.orig_termios)==-1)die("tcgetattr");
  atexit(disableRawMode);

struct termios raw_struct= E.orig_termios;
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

int getCursorPosition(int *rows,int *cols){
 char buf[32];
 unsigned int i=0; 

 if(write(STDOUT_FILENO,"\x1b[6n",4)!=4)return -1;

 while(i<sizeof(buf)-1){
  if(read(STDIN_FILENO,&buf[i],1)!=1)break;
  if(buf[i]=='R')break;
  i++;
 }
 
 buf[i]='\0';
 
 if(buf[0]!='\x1b' || buf[1]!='[')return -1;
 if(sscanf(&buf[2],"%d;%d",rows,cols)!=2)return -1;
 
 return 0;
}

int getWindowSize(int *rows,int *cols){
  struct winsize ws;
  
  if(ioctl(STDOUT_FILENO,TIOCGWINSZ,&ws)== -1 || ws.ws_col == 0){
    if(write(STDOUT_FILENO,"\x1b[999C\x1b[999B",12)!=12) return -1;
    return getCursorPosition(rows,cols);
  }
  else{
    *cols = ws.ws_col;
    *rows = ws.ws_row;
    return 0;
   }
}

/*** append buffer ***/
// Its not a good idea to make a whole bunch of small 'write()'s everytime we refresh the screen. It would be better do one big write() to make sure the
// screen updates at once. We want to replace all our write() calls with code that appends the string to the buffer, and then write() this buffer at the
// end. C doesn't have dynamic strings so we'll create our own dynamic string type that supports one operation: appending. We define a struct called abuf

struct abuf{
  char *b;
  int len;
};

#define ABUF_INIT {NULL,0}

/*** output ***/

void editorDrawRows(){
  int y;
  for(y=0;y< E.screenrows;y++){
    write(STDOUT_FILENO,"~",1);
    
    if(y< E.screenrows-1)write(STDOUT_FILENO,"\r\n",2);
    }
}

//"\x1b" is the escape character

void editorRefreshScreen(){
   write(STDOUT_FILENO,"\x1b[2J",4);
   write(STDOUT_FILENO,"\x1b[H",3);

   editorDrawRows();

   write(STDOUT_FILENO,"\x1b[H",3);
}



/*** input ***/
//editorProcessKeypress() waits for a keypress and then handles it accordingly
void editorProcessKeypress(){
  char c=editorReadKey();
  switch(c){
   case CTRL_KEY('q'):
    write(STDOUT_FILENO,"\x1b[2J",4);
    write(STDOUT_FILENO,"\x1b[H",3);
    exit(1);
    break;
   }
}


/*** init ***/

void initEditor(){
  if(getWindowSize(&E.screenrows, &E.screencols)== -1) die("getWindowSize");
}

int main(){
  enableRawMode();
  initEditor();

  while(1){
   editorRefreshScreen();
   editorProcessKeypress();
  }

  return 0;
}
