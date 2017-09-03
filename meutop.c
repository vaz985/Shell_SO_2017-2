#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <pwd.h>
#include <dirent.h>
#include <ctype.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <signal.h>
#include <pthread.h>

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

char *create_top_table(struct Process *process_set, char* buffer){
  buffer[0] = '\0'; // "Esvazia a string"
  char line_buffer[1000];
  sprintf(line_buffer, "%-7s|%-10s|%-20s|%-8s|\n", "PID", "User", "PROCNAME", "Estado");
  strcat(buffer, line_buffer);
  sprintf(line_buffer, "-------|----------|--------------------|--------|\n");
  strcat(buffer, line_buffer);
  for (int i = 0; i < 20; ++i){
    sprintf(line_buffer, "%-7d|%-10s|%-20s|%-8s|\n", process_set[i].pid, process_set[i].u_name, process_set[i].p_name, process_set[i].state);
    strcat(buffer, line_buffer);
  }
  sprintf(line_buffer, "-------|----------|--------------------|--------|\n");
  strcat(buffer, line_buffer);
  return buffer;
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


typedef struct bashString{
  char table_string[1000];
  char signal_string[100];
} bashString;

void *thread_print_table(void *arg){
  bashString *bash_text = (bashString*) arg;
  int pidsList[20];
  struct Process process_set[20];
  while(1){
    getProcesses(pidsList);    
    for (int i = 0; i < 20; i++)
      process_set[i] = get_process_info(pidsList[i]);  
    char table[10000];
    create_top_table(process_set, (*bash_text).table_string);
    printf("%s\n", (*bash_text).table_string);
    system("clear");
    sleep(1);  
  }
  
}

void *thread_print_command_line(void *arg){
  bashString *bash_text = (bashString*) arg;
  while(1){
    strcat((*bash_text).signal_string, "abc");
    printf("%s\n", (*bash_text).signal_string);
    sleep(2);
    strcat((*bash_text).signal_string, "def");
    sleep(2);
    printf("%s\n", (*bash_text).signal_string);
    strcat((*bash_text).signal_string, "ghi");
    sleep(2);
  }
}



int main(){
  pthread_t threads[2];
  char bash_text[10000];
  int status;
  struct bashString top_bash_text; 

  status = pthread_create(&threads[0], NULL, thread_print_table, &top_bash_text);
  if(status){
      printf("Erro ao criar thread. pthread_create() retornou: %d\n", status);
      exit(-1);
  }
  status = pthread_create(&threads[1], NULL, thread_print_command_line, &top_bash_text);
  if(status){
      printf("Erro ao criar thread. pthread_create() retornou: %d\n", status);
      exit(-1);
  }
  pthread_join(threads[0], NULL);
  pthread_join(threads[1], NULL);
  printf("%s\n", bash_text);
  return 0;
}
