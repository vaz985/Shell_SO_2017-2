#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/stat.h>
#include<pwd.h>

struct process{
  unsigned int pid;
  unsigned char state;
  unsigned char * p_name;
  unsigned char * u_name;
};

struct process * get_process_info(int pid){
  char buffer[100];
  struct process * p = malloc(sizeof(struct process));
  struct passwd *pwd;
  struct stat stat_buffer;
  sprintf(buffer,"%s%d%s","/proc/",pid,"/stat");
  FILE * fp = fopen(buffer,"r"); 
  fscanf(fp,"%d %s %c",&p->pid,p->p_name,&p->state);
  fclose(fp);
  printf("bp 1\n");
  stat(buffer, &stat_buffer);
  printf("bp 2\n");
  pwd = getpwuid(stat_buffer.st_uid);
  printf("bp 3\n");
  printf("%s\n", pwd->pw_name);
  return p;
};

int main(){
  FILE * pp; 
  get_process_info(1207); 
  
  return 0;
}
