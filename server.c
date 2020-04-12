#include <stdio.h>
#include <memory.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include "rpc_common.h"

int multiply(int a, int b){
  return a + b;
}

void
multiply_server_stub_marshal(int res, ser_buff_t *server_recv_ser_buffer){
  serialize_data(server_send_ser_buffer, (char*)&res, sizeof(int));
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
rpc_server_process_msg(ser_buff_t *server_recv_ser_buffer, ser_buff_t *server_send_ser_buffer){
  /* step 5: */
  int res = multiply_server_stub_unmarshal(server_recv_ser_buffer);

  /* step 7: */
  multiply_server_stub_marshal(res, server_recv_ser_buffer);
}
int
main(int argc, char **argv){
  int sockfd = 0, len, addr_len, opt = 1, reply_msg_size = 0;
  struct sockaddr_in server_addr, client_addr;
  sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
  if(sockfd == -1){
    printf("socket creation failed\n");
    exit(1);
  }
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(SERVER_PORT);
  server_addr.sin_addr.s_addr = INADDR_ANY;
  addr_len = sizeof(struct sockaddr);

  if(setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (char*)&opt, sizeof(opt)) < 0){
    perror("setsockopt");
    exit(EXIT_FAILURE);
  }
  if(setsockopt(sockfd, SOL_SOCKET, SO_REUSEPORT, (char*)&opt, sizeof(opt)) < 0){
    perror("setsockopt");
    exit(EXIT_FAILURE);
  }
  if(bind(sockfd, (struct sockaddr*)&server_addr, sizeof(struct sockaddr)) < 0){
    perror("bind");
    exit(EXIT_FAILURE);
  }

  ser_buff_t *server_recv_ser_buffer = NULL,
             *server_send_ser_buffer = NULL;

  init_serialized_buffer_of_defined_size(&server_recv_ser_buffer, MAX_RECV_SEND_BUFF_SIZE);
  init_serialized_buffer_of_defined_size(&server_send_ser_buffer, MAX_RECV_SEND_BUFF_SIZE);

  printf("server ready to receive data\n");
  while(1){
    reset_serialize_buffer(server_recv_ser_buffer);
    /* step 4: receive the data from client in local buffer */
    len = recvfrom(sockfd, server_recv_ser_buffer->b, get_serialize_buffer_data_size(server_recv_ser_buffer), 0, (struct sockaddr*)&client_addr, &addr_len);
    printf("number of bytes received from the client = %d\n", len);

    reset_serialize_buffer(server_send_ser_buffer);

    rpc_server_process_msg(server_recv_ser_buffer, server_send_ser_buffer);

    /* step 8: send the serialized result back to the client */
    len = sendto(sockfd, server_send_ser_buffer->b, get_serialize_buffer_data_size(server_send_ser_buffer), 0, (struct sockaddr*)&client_addr, sizeof(sockaddr));
  }
  return 0;
}
