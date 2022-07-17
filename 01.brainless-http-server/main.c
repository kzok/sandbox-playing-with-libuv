#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

#define LOG(...) do{printf(__VA_ARGS__);printf("\n");} while(0);

#define EXPOSED_PORT 3000

const char* response = \
  "HTTP/1.0 200 OK\r\n" \
  "content-type: text/plain\r\n" \
  "connection: close\r\n" \
  "\r\n" \
  "Hello world!\n";

/**
 * @brief handle client connection
 * 
 * @param client client socket fd
 */
void handle_connection(int client) {
  int ret = 0;

  char trash_bin_buffer[1024] = {0};
  // read received data (work for curl)
  ret = read(client, trash_bin_buffer, sizeof(trash_bin_buffer));
  if (ret == -1) {
    LOG("Failed to read(). %s", strerror(errno));
    return;
  }
  // wait 3 seconds
  for (int count = 3; count > 0; count--) {
    LOG("Sleep count %d", count);
    sleep(1);
  }
  // write HTTP response
  ret = write(client, response, strlen(response));
  if (ret == -1) {
    LOG("Failed to send(). %s", strerror(errno));
    return;
  }
}

int main() {
  int ret = 0;

  // socket()
  int server = socket(AF_INET, SOCK_STREAM, 0);
  if (server == -1) {
    LOG("Failed to socket(). %s", strerror(errno));
    return -1;
  }

  // Avoiding "bind: Address already in use"
  const int one = 1;
  setsockopt(server, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(int));

  // bind()
  struct sockaddr_in server_addr = { 0 };
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(EXPOSED_PORT);
  server_addr.sin_addr.s_addr = INADDR_ANY;
  ret = bind(server, (struct sockaddr *)&server_addr, sizeof(server_addr));
  if (ret == -1) {
    LOG("Failed to bind(). %s", strerror(errno));
    return -1;
  }

  // listen()
  ret = listen(server, 5);
  if (ret == -1) {
    LOG("Failed to listen(). %s", strerror(errno));
    return -1;
  }

  LOG("Server listening on http://localhost:%d ...", EXPOSED_PORT);

  while(1) {
    struct sockaddr_in client_addr;
    int client_addr_len = sizeof(client_addr);
    // accept()
    int client = accept(server, (struct sockaddr *)&client_addr, (socklen_t *)&client_addr_len);
    if (client == -1) {
      LOG("Failed to accept(). %s", strerror(errno));
      continue;
    }
    // Set 1 second timeout (Browsers sometimes hold TCP connection)
    struct timeval tv;
    tv.tv_sec = 1;
    tv.tv_usec = 0;
    setsockopt(client, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv);

    LOG("Connection accepted.");
    handle_connection(client);
    if (client != -1) {
      close(client);
    }
    LOG("Connection closed.");
  }

  // cleanup (but dead code)
  if (server != -1) {
    close(server);
  }
  return 0;
}
