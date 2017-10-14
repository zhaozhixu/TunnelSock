#include <signal.h>
#include <sys/wait.h>
#include "socklib.h"
#include "protocol.h"
#include "util.h"

#define DEFAULT_PORT 2222
#define MAXWORKER 4

static int sfd = -1, cfd = -1;
static int newsfd = -1, newcfd = -1;
static pid_t worker_ids[MAXWORKER];
static int ismaster;

extern char REQ_NEW_CLIENT[];
extern char REQ_CLOSE_FD[];

static void usage(void)
{
     fprintf(stderr,
             "usage: tunneld [-p port]\n");
     exit(0);
}

static int add_worker(pid_t id) {
     int i;
     for (i = 0;worker_ids[i] != 0 && i < MAXWORKER; i++)
          ;

     if (i >= MAXWORKER) {
          printf("MAXWORKER limit.\n");
          kill(id, SIGQUIT);
          return -1;
     }

     worker_ids[i] = id;
     return i;
}

static int delete_worker(pid_t id) {
     int i;
     for (i = 0; worker_ids[i] != id && i < MAXWORKER; i++)
          ;
     if (i < MAXWORKER)
          worker_ids[i] = 0;
}

static void quit_all_workers() {
     int i;
     for (i = 0; i < MAXWORKER; i++)
          if (worker_ids[i] != 0)
               kill(worker_ids[i], SIGQUIT); /* waitpid */
}

static void quit_handler(int s) {
     printf("Preparing to quit...");
     if (ismaster) {
          quit_all_workers();
          close(sfd);
          close(cfd);
     } else {
          close(newsfd);
          close(newcfd);
     }
     printf("done.\n");
     exit(0);
}

static void int_handler(int s) {
     printf("Preparing to stop...");
     printf("done.\n");
     exit(0);
}

static void chld_handler(int s) {
     printf("worker terminated: ");
     pid_t cid;
     while ((cid = waitpid(-1, NULL, WNOHANG)) > 0) {
          printf("%d ", cid);
          delete_worker(cid);
     }
     printf("\n");
     if (cid == -1)
          perror("waitpid");
}

static void set_signal(int signum) {
     struct sigaction newhandler;
     sigset_t blocked;

     newhandler.sa_flags = SA_RESETHAND | SA_RESTART;
     sigemptyset(&blocked);

     switch (signum) {
     case SIGQUIT:
          sigaddset(&blocked, SIGINT);
          newhandler.sa_handler = quit_handler;
          break;
     case SIGINT:
          sigaddset(&blocked,SIGQUIT);
          newhandler.sa_handler = int_handler;
          break;
     case SIGCHLD:
          newhandler.sa_handler = chld_handler;
     default:
          break;
     }

     newhandler.sa_mask = blocked;

     if (sigaction(signum, &newhandler, NULL) == -1)
          perror("sigaction");
}

static void init() {
     ismaster = 1;
     int i;
     for (i = 0; i < MAXWORKER; i++)
          worker_ids[i] = 0;
     set_signal(SIGINT);
     set_signal(SIGQUIT);
     set_signal(SIGCHLD);
}

static void process_newcfd(int cfd, int newcfd)
{
     char buf[BUFSIZ];
     size_t n;

     write(cfd, REQ_NEW_CLIENT, strlen(REQ_NEW_CLIENT));
     n = read(cfd, buf, sizeof(buf));

     if (is_server_ready(buf, strlen(buf))) {
          set_nonblock(cfd);
          set_nonblock(newcfd);

          protocol_talk(newcfd, cfd);
          fprintf(stderr, "done accept\n");
          set_block(cfd);
     }
     if (is_server_fail(buf, strlen(buf))) {
          fprintf(stderr, "remote server fail\n");
          exit(EXIT_FAILURE);
     }
}

static void process_rq(int cfd)
{
     pid_t worker_id;
     int n;
     char *host, *port;
     char buf[BUFSIZ];
     char new_req_flag[] = "new request";
     char not_bind[] = "not bind";
     char bind_success[] = "bind success";

     worker_id = fork();
     if (worker_id == -1) {
          perror("fork");
          return;
     }
     if (worker_id != 0) {
          add_worker(worker_id);
          return;
     }
     ismaster = 0;
     fprintf(stderr, "new worker: %d\n", (int)getpid());

     read(cfd, buf, sizeof(buf));           /* need exam */
     fprintf(stderr, "got a tunnel request: %s\n", buf); /* need exam */
     parse_hostport(buf, &host, &port);

     newsfd = make_server_socket(atoi(port));
     if (newsfd == -1) {
          fprintf(stderr, "process_rq socket failed\n");
          write(cfd, not_bind, strlen(not_bind)+1);
          exit(EXIT_FAILURE);
     }
     write(cfd, bind_success, strlen(bind_success)+1);

     while (1) {                /* should make parallel */
          newcfd = accept(newsfd, NULL, NULL);
          fprintf(stderr, "accept a request\n");
          process_newcfd(cfd, newcfd);
          /* write(cfd, new_req_flag, strlen(new_req_flag)+1); */
          /* write(cfd, REQ_NEW_CLIENT, strlen(REQ_NEW_CLIENT)); */
          /* n = read(cfd, buf, sizeof(buf)); */

          /* if (is_server_ready(buf, strlen(buf))) { */
          /*      set_nonblock(cfd); */
          /*      set_nonblock(newcfd); */

          /*      protocol_talk(newcfd, cfd); */
          /*      fprintf(stderr, "done accept\n"); */
          /*      set_block(cfd); */
          /* } */
          /* if (is_server_fail(buf, strlen(buf))) { */
          /*      fprintf(stderr, "remote server fail\n"); */
          /*      exit(EXIT_FAILURE); */
          /* } */
          /* close(newcfd);        /\* shutdown? *\/ */
     }
}

int main(int argc, char *argv[])
{
     int port = DEFAULT_PORT;

     if (argc == 2) {
          if (argv[1][0] != '-')
               usage();
          switch (argv[1][1]) {
          case 'p':
               port = atoi(argv[1] + 2);
               break;
          default:
               break;
          }
     }

     init();

     sfd = make_server_socket(port);
     if (sfd == -1)
          exit(EXIT_FAILURE);
     printf("tunneld listening port: %d\n", port);
     fflush(stdout);

     while(1) {
          cfd = accept(sfd, NULL, NULL);
          process_rq(cfd);
          close(cfd);
     }
}
