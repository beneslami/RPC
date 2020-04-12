#include <stdio.h>
#include <netdb.h>
#include <memory.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "rpc_common.h"
#include "serialize/serialize.h"

void
rpc_send_recv(ser_buff_t *client_send_ser_buffer, ser_buff_t *client_recv_ser_buffer){
  struct sockaddr_in dest;
  int sockfd = 0, rc = 0, recv_size = 0;
  unsigned int addr_len;
  dest.sin_family = AF_INET;
  dest.sin_port = htons(SERVER_PORT);
  struct hostent *host = (struct hostent *)gethostbyname(SERVER_IP);
  sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
  if(sockfd == -1){
    printf("socket creation failed\n");
    exit(-1);
  }
  rc = sendto(sockfd, client_send_ser_buffer->b, get_serialize_buffer_length(client_send_ser_buffer), 0, (struct sockaddr *)&dest, sizeof(struct sockaddr));
  printf("%s() : %d bytes sent\n", __FUNCTION__, rc);

  recv_size = recvfrom(sockfd, client_recv_ser_buffer->b, get_serialize_buffer_length(client_recv_ser_buffer), 0, (struct sockaddr*)&dest, &addr_len);
  printf("%s() : %d bytes received\n", __FUNCTION__, recv_size);
}

ser_buff_t
*multiply_client_stub_marshal(int a, int b){
  ser_buff_t *client_send_ser_buffer = NULL;
  init_serialized_buffer_of_defined_size(&client_send_ser_buffer, MAX_RECV_SEND_BUFF_SIZE);
  serialize_data(client_send_ser_buffer, (char*)&a, sizeof(int));
  serialize_data(client_send_ser_buffer, (char*)&b, sizeof(int));
  return client_send_ser_buffer;
}

int
multiply_client_stub_unmarshal(ser_buff_t *client_recv_ser_buffer){
  int res;
  de_serialize_data((char*)&res, client_recv_ser_buffer, sizeof(int));
  return res;
}

int
multiply_rpc(int a, int b){
  /* step 2: serializing/marshaling the arguments */
  ser_buff_t *client_send_ser_buffer = multiply_client_stub_marshal(a, b);
  ser_buff_t *client_recv_ser_buffer = NULL;
  /* prepare the buffer for receiving the serialized data */
  init_serialized_buffer_of_defined_size(&client_recv_ser_buffer, MAX_RECV_SEND_BUFF_SIZE);

  /* step 3: send the serialized data to the server and wait for the reply*/
  rpc_send_recv(client_send_ser_buffer, client_recv_ser_buffer);
  free_serialize_buffer(client_send_ser_buffer);
  client_send_ser_buffer = NULL;

  /* step 9: un-marshal the received serialized buffer. And reconstruct the RPC return type*/
  int res = multiply_client_stub_unmarshal(client_recv_ser_buffer);

  free_serialize_buffer(client_recv_ser_buffer);
  return res;
}

int
main(int argc, char **argv){
  int a, b;
  printf("Enter the first number:\n");
  scanf("%d", &a);
  printf("Enter the second number:\n");
  scanf("%d", &b);

  /* step 1: Invoke the rpc */
  int result = multiply_rpc(a, b);

  printf("Result = %d\n", result);
  return 0;
}
