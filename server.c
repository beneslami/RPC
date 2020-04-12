#include <stdio.h>
#include <memory.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include "rpc_common.h"
#include "serialize/serialize.h"

int multiply(int a, int b){
  return a * b;
}

int
multiply_server_stub_unmarshal(ser_buff_t *server_recv_ser_buffer){
    int a, b;
    de_serialize_data((char*)&a, server_recv_ser_buffer, sizeof(int));
    de_serialize_data((char*)&b, server_recv_ser_buffer, sizeof(int));
    /* step 6: actual RPC */
    return multiply(a, b);
}

void
multiply_server_stub_marshal(int res, ser_buff_t *server_send_ser_buffer){
  serialize_data(server_send_ser_buffer, (char*)&res, sizeof(int));
}

void
rpc_server_process_msg(ser_buff_t *server_recv_ser_buffer, ser_buff_t *server_send_ser_buffer){
  /* step 5: */
  int res = multiply_server_stub_unmarshal(server_recv_ser_buffer);
  /* step 7: */
  multiply_server_stub_marshal(res, server_send_ser_buffer);
}

int
main(int argc, char **argv){
  int sockfd = 0, len, opt = 1, reply_msg_size = 0, data_socket;
  struct sockaddr_in server_addr, client;
  int clientlen;

  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if(sockfd == -1){
    printf("socket creation failed\n");
    exit(1);
  }
  memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  server_addr.sin_port = htons(SERVER_PORT);

  if(bind(sockfd, (struct sockaddr*)&server_addr, sizeof(struct sockaddr)) < 0){
    perror("bind");
    exit(EXIT_FAILURE);
  }

  if (listen(sockfd, 20) < 0) {
      perror("listen");
      exit(1);
  }
  ser_buff_t *server_recv_ser_buffer = NULL,
             *server_send_ser_buffer = NULL;

  init_serialized_buffer_of_defined_size(&server_recv_ser_buffer, MAX_RECV_SEND_BUFF_SIZE);
  init_serialized_buffer_of_defined_size(&server_send_ser_buffer, MAX_RECV_SEND_BUFF_SIZE);

  printf("server ready to receive data\n");
  while(1){
    clientlen = sizeof(struct sockaddr_in);
    data_socket = accept(sockfd, (struct sockaddr *)&client, (socklen_t*)&clientlen);
    reset_serialize_buffer(server_recv_ser_buffer);
    /* step 4: receive the data from client in local buffer */
    len = recv(data_socket, (char*)server_recv_ser_buffer->b, 32, 0);
    if(len > 0){
        printf("number of bytes received from the client = %d\n", len);
        reset_serialize_buffer(server_send_ser_buffer);
        rpc_server_process_msg(server_recv_ser_buffer, server_send_ser_buffer);
        
        /* step 8: send the serialized result back to the client */
        len = send(data_socket, (char*)server_send_ser_buffer->b, get_serialize_buffer_length(server_send_ser_buffer), 0);
        if(len > 0){
          printf("rpc server replied with %d bytes msg\n", len);
          close(data_socket);
        }
        else{
          printf("server could not send the result back to the client\n");
          close(data_socket);
        }
    }
    else{
        printf("could not receive data from client %d\n", len);
        perror("recv");
        close(data_socket);
    }
  }
  close(sockfd);
  return 0;
}
