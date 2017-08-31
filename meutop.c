#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <pwd.h>
#include <dirent.h>
#include <ctype.h>

struct Process{
  unsigned int pid;
  char state[10];
  char p_name[100];
  unsigned char * u_name;
};

struct Process get_process_info(int pid){
  struct Process p;
  struct passwd *pwd;
  char buffer[100];
  struct stat stat_buffer;
  // Retrieve process information fromm /proc/*/stat
  sprintf(buffer,"%s%d%s","/proc/",pid,"/stat");
  FILE * fp = fopen(buffer,"r");
  fscanf(fp,"%d %s %s", &p.pid, p.p_name, p.state);
  // Retreive process owner's name
  stat(buffer, &stat_buffer);
  pwd = getpwuid(stat_buffer.st_uid);
  p.u_name = pwd->pw_name;
  return p;
}

void print_top_table(struct Process *process_set){
  printf("%-7s|%-10s|%-20s|%-8s|\n", "PID", "User", "PROCNAME", "Estado");
  printf("-------|----------|--------------------|--------|\n");
  for (int i = 0; i < 20; ++i){
    printf("%-7d|%-10s|%-20s|%-8s|\n", process_set[i].pid, process_set[i].u_name, process_set[i].p_name, process_set[i].state);
  }
  printf("-------|----------|--------------------|--------|\n");
}

int is_number(char* string){
  int isDigit = 1;
  int j=0;
  while(j<strlen(string) && isDigit == 1){
    if(!isdigit(string[j])){
      isDigit = 0;
      break;
    }
    j++;
  }
  return isDigit;
}

void getProcesses(int* pidsList){
  unsigned char isFolder =0x4;
  DIR *dir;
  struct dirent *ent;
  int i = 0;
  if ((dir = opendir ("/proc")) != NULL) {
    /* print all the files and directories within directory */
    while ((ent = readdir (dir)) != NULL && i < 20) {
      if(ent->d_type == isFolder && is_number(ent->d_name)){
        pidsList[i] = atoi(ent->d_name);
        i++;
      }
    }
    closedir (dir);
  } else {
    /* could not open directory */
    printf("Erro ao acessar /proc");
  }
}

int main(){
  FILE * pp; 
  int pidsList[20];
  getProcesses(pidsList);  
  struct Process process_set[20];
  for (int i = 0; i < 20; ++i)
    process_set[i] = get_process_info(pidsList[i]);  
  print_top_table(process_set);   
  return 0;
}
