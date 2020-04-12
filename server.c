

int main(int argc, char **argv){
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

  
  return 0;
}
