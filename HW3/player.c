#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include "abstract.h"
#include "potato.h"

int main(int argc, char* argv[]) {
    if (argc != 3) {
        printf("Two command line arguements are required\n");
        return EXIT_FAILURE;
    }
    int status;
    int socket_fd;
    struct addrinfo hint;
    struct addrinfo *host_info_list;
    struct addrinfo *p; // temporary variable for getting socket descriptop
    int yes = 1;
    const char *hostname = argv[1];
    const char *port = argv[2];
    int player_id;
    int num_players;

    /* connect to ringmaster's server */
    socket_fd = connect_to(hostname, port);
    /* successfully connect */

    /* receive player id & total player number */
    recv(socket_fd, &player_id, sizeof(player_id), MSG_WAITALL);
    recv(socket_fd, &num_players, sizeof(num_players), MSG_WAITALL);
    printf("Connected as player %d out of %d total players\n", player_id, num_players);
    /* finish receiving */

    /* become a server */
    char my_host[1024];
    char my_service[20];
    int myService_fd = create_service("0.0.0.0","", my_host, 1024, my_service, 20);
    /* already a server */


    /* send ip/port to master */
    // send host name first
    send(socket_fd, my_host, 1024, 0);
    // then send port
    send(socket_fd, my_service, 20, 0);
    /* finishing sending */


    /* receive left and right neighbor host and port */
    char left_host[1024];
    char left_service[20];
    //char right_host[1024];
    //char right_service[20];

    // recv left
    recv(socket_fd, left_host, 1024, MSG_WAITALL);
    recv(socket_fd, left_service, 20, MSG_WAITALL);
    // recv right
    //recv(socket_fd, right_host, 1024, MSG_WAITALL);
    //recv(socket_fd, right_service, 20, MSG_WAITALL);
    

    /* connect to neighbors */
    //printf("player %d begin to left neighbor\n", player_id);
    int left_socket_fd = connect_to(left_host, left_service);
    //printf("player %d successfully connect to left neighbor\n", player_id);
    //int right_socket_fd = connect_to(right_host, right_service);
    //printf("player %d successfully connect to right neighbor\n", player_id);
    /* already connecting */

    /* accept connections from neighbor */
    int right_socket_fd = accept_connect(myService_fd);
    //printf("player %d successfully accept connection from right neighbor\n", player_id);
    //accept_connect(myService_fd);
    /* finshing accept */

    //set rand
    //srand((unsigned int)time(NULL)+player_id);
    /* begin playing the game */
    fd_set player_set;
    fd_set read_fds;
    FD_ZERO(&player_set); 
    FD_ZERO(&read_fds);
    int neighbor_fd[3];
    neighbor_fd[0] = left_socket_fd;
    neighbor_fd[1] = right_socket_fd;
    neighbor_fd[2] = socket_fd;
    int fdmax = 0;
    for (int i = 0; i < 3; i++) {
        fdmax = neighbor_fd[i] > fdmax ? neighbor_fd[i] : fdmax;
    }
    FD_SET(left_socket_fd, &player_set);
    FD_SET(right_socket_fd, &player_set);
    FD_SET(socket_fd, &player_set);
    for (int i = 0; i < 10000; i++) {
        read_fds = player_set;
        if (select(fdmax + 1, &read_fds, NULL, NULL, NULL) == -1) {
            fprintf(stderr, "Error: player %d select\n", player_id);
            exit(EXIT_FAILURE);
        }
        int shut_dowm;
        // get potato
        potato hot_potato;
        for (int i = 0; i < 3; i++) {
            if (FD_ISSET(neighbor_fd[i], &read_fds)) {
                shut_dowm = recv(neighbor_fd[i], &hot_potato, sizeof(hot_potato), MSG_WAITALL);
                break;
            }
        }
        // if there is a potato with -1 hop
        // close everything
        if (hot_potato.hop == -1 || shut_dowm == 0) {
            printf("playerid:%d, ENDing\n", player_id);
            break;
        }
        // if the size is less than the hop num
        if (hot_potato.size < hot_potato.hop) {
            // send it to left of right
            hot_potato.routh[hot_potato.size] = player_id;
            hot_potato.size += 1;
            int rand_num = rand() % 2;
            if (rand_num == 0) { // send it to left
                send(left_socket_fd, &hot_potato, sizeof(hot_potato), 0);
                printf("Cur_size:%d, Player_id %d Sending potato to %d\n", hot_potato.size - 1, player_id, (player_id + num_players - 1) % num_players);
            }
            else { // send it to right 
                send(right_socket_fd, &hot_potato, sizeof(hot_potato), 0);
                printf("Cur_size:%d, Player_id %d Sending potato to %d\n", hot_potato.size - 1, player_id,(player_id + num_players + 1) % num_players);
            }
        }
        else {
            // send it to master
            hot_potato.routh[hot_potato.size] = player_id;
            hot_potato.size += 1;
            printf("Cur_size:%d, player_id:%d, Hot:%d, I'm it\n", hot_potato.size - 1, player_id, hot_potato.hop);
            send(socket_fd, &hot_potato, sizeof(hot_potato), 0);
        }
    }
    /* ending playing */

    /* */
    /**/
    close(left_socket_fd);
    close(right_socket_fd);
    close(myService_fd);
    close(socket_fd);
    return EXIT_SUCCESS;
}