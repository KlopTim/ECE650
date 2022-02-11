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

int main(int argc, char * argv[]) {
    if (argc != 4) {// ringmaster port num_players num_hops
        printf("Three command line arguments are required\n");
        return EXIT_FAILURE;
    }
    int status;
    int socket_fd;
    int yes = 1;
    struct addrinfo hints;
    struct addrinfo *p; // temporary variable for getting socket descriptop
    struct addrinfo *host_info_list;
    const char *hostname = NULL;
    struct sockaddr_storage player_socket_addr;
    socklen_t player_socket_addr_len = sizeof(player_socket_addr);
    
    // input arguement
    const char * port = argv[1];
    int num_players = atoi(argv[2]);
    int num_hops = atoi(argv[3]);
    if (num_players < 1) {
        fprintf(stderr, "player number should be greater than 0\n");
        return EXIT_FAILURE;
    }
    if (num_hops <= 0) {
        fprintf(stderr, "hop number should be greater than or equal to 0\n");
        return EXIT_FAILURE;
    }
    // player_fd & player_addr(sockaddr_storage type)
    int player_fd[num_players];
    char player_host[num_players][1024];
    char player_serv[num_players][20];
    // output information
    printf("Potato Ringmaster\n");
    printf("Player = %d\n", num_players);
    printf("Hops = %d\n", num_hops);

    /* become a server */
    socket_fd = create_service(hostname, port, NULL, 0, NULL, 0);
    /* already become a server */



    /* accept connections from players */
    for (int i = 0; i < num_players; i++) {
        // accept players' connection
        player_fd[i] = accept_connect(socket_fd);
        // send each player an id
        send(player_fd[i], &i, sizeof(int), 0);
        // send each player the total player number
        send(player_fd[i], &num_players, sizeof(int), 0);
        printf("Player %d is ready to play\n", i);
    }
    /* finish accepting all the connections */

    /* receive each player's host and port */
    for (int i = 0; i < num_players; i++) {
        recv(player_fd[i], &player_host[i], 1024, MSG_WAITALL);
        recv(player_fd[i], &player_serv[i], 20, MSG_WAITALL);
    }
    /* receiving ending */

    /*
    for (int i = 0; i < num_players; i++) {
        printf("player id: %d\n", i);
        printf("host: %s\n", player_host[i]); 
        printf("service: %s\n", player_serv[i]); 
    }
    */

    /* send each player its neighbors' port*/
    
    for (int i = 0; i < num_players; i++) {
        // calculate left and right neighbor index
        int left = (i - 1 + num_players) % num_players;
        //int right = (i + 1 + num_players) % num_players;
        // send left neighbor host and port
        send(player_fd[i], &player_host[left], 1024, 0);
        send(player_fd[i], &player_serv[left], 20, 0);
        // send right neighbor host and port
        //send(player_fd[i], player_host[right], 1024, 0);
        //send(player_fd[i], &player_serv[right], 20, 0);
    }
    
    /* finishing sending */
    //set rand
    /* begin playing the game */
    // create a potato
    potato hot_potato;
    hot_potato.hop = num_hops;
    hot_potato.size = 0;
    // send a potato to a random player
    int player_id = rand() % num_players;
    send(player_fd[player_id], &hot_potato, sizeof(hot_potato), 0);
    printf("Ready to start the game, sending potato to player %d\n", player_id);
    // wait for a 0 hop potato
    fd_set master_set;
    FD_ZERO(&master_set);
    int fdmax = 0;
    // add all file descriptor to master_set
    for (int i = 0; i < num_players; i++) {
        FD_SET(player_fd[i], &master_set);
        fdmax = player_fd[i] > fdmax ? player_fd[i] : fdmax;
    }
    if (select(fdmax + 1, &master_set, NULL, NULL, NULL) == -1) {
        fprintf(stderr, "Error: master select\n");
        exit(EXIT_FAILURE);
    }
    // find the player has the final potato
    for (int i = 0; i < player_fd[i]; i++) {
        if (FD_ISSET(player_fd[i], &master_set)) {
            recv(player_fd[i], &hot_potato, sizeof(potato), MSG_WAITALL);
            break;
        }
    }
    // trace routh
    printf("Trace of potato:\n");
    for (int i = 0; i <= hot_potato.hop; i++) {
        if (i != 0) printf(",");
        printf("%d", hot_potato.routh[i]);
    }
    printf("\n");
    /* finsh playing */

    /* send a special potato to end the game */
    for (int i = 0; i < num_players; i++) {
        potato new_hot_potato;
        new_hot_potato.hop = -1;
        new_hot_potato.size = -1;
        send(player_fd[i], &new_hot_potato, sizeof(new_hot_potato), 0);
    }
    /* ending */

    // close all the connection
    for (int i = 0; i < num_players; i++) close(player_fd[i]);
    close(socket_fd);
    return EXIT_SUCCESS;
}