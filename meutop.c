#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <pwd.h>
#include <dirent.h>
#include <ctype.h>
#include <unistd.h>
#include <time.h>

struct Process{
  unsigned int pid;
  char state[10];
  char p_name[100];
  char u_name[100];
};

struct Process get_process_info(int pid){
  struct Process p;
  //struct passwd *pwd;
  char buffer[100];
  // Retrieve process information fromm /proc/*/stat
  sprintf(buffer,"%s%d%s","/proc/",pid,"/stat");
  FILE * fp = fopen(buffer,"r");
  fscanf(fp,"%d %s %s", &p.pid, p.p_name, p.state);
  // Retreive process owner's name
  struct stat info;
  stat(buffer, &info); 
  struct passwd *pw = getpwuid(info.st_uid);
  strcpy(p.u_name, pw->pw_name);
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
  int process_count = 0;
  
  // Read /poc once to count the number of processes
  if ((dir = opendir ("/proc")) != NULL) {
    while ((ent = readdir (dir)) != NULL) 
      if(ent->d_type == isFolder && is_number(ent->d_name))
        process_count++;
    closedir (dir);
  }else{
    printf("Erro ao acessar /proc");
  }
  // Alocate a vector the size of the number of process found
  int *processes = malloc(process_count*sizeof(int));
  // Read the /proc directory again, filling the vector with the PID's found
  if((dir = opendir ("/proc")) != NULL) {
    while ((ent = readdir (dir)) != NULL) {
      if(ent->d_type == isFolder && is_number(ent->d_name)){
        processes[i] = atoi(ent->d_name);
        i++;
      }
    }
    closedir (dir);
  }else{
    /* could not open directory */
    printf("Erro ao acessar /proc");
  }

  // Get 20 random processes
  i = 0;
  srand(time(NULL)); 
  int rand_pid = 0;
  int invalid_pid;
  int repeated = 0;
  int j;
  while(i<20){
    invalid_pid = 1;
    while(invalid_pid){
      repeated = 0;
      rand_pid = rand() % process_count;
      // Avoid repeated PID number
      for(j=0; j<i; j++)
          if(pidsList[j] == processes[rand_pid])
            repeated = 1;
      if(rand_pid != 0 && !repeated)
        invalid_pid = 0;
    }
    pidsList[i] = processes[rand_pid];
    i++;
  }
}

int main(){
  int pidsList[20];
  struct Process process_set[20];
  getProcesses(pidsList);    
  for (int i = 0; i < 20; i++)
    process_set[i] = get_process_info(pidsList[i]);  
  print_top_table(process_set);   
  /*int a, b;
  printf(">");
  scanf("%i %i", &a, &b);
  printf("%d %d\n", a, b);*/
  return 0;
}
