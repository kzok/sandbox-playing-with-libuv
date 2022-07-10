#include <uv.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>

#include "./shared.h"
#include "./session.h"

#define EXPOSED_PORT 3000
#define DEFAULT_BACKLOG 128

const char* g_http_response = \
  "HTTP/1.0 200 OK\r\n" \
  "content-type: text/plain\r\n" \
  "connection: close\r\n" \
  "\r\n" \
  "Hello world!\n";

/**
 * @brief http://docs.libuv.org/en/v1.x/handle.html#c.uv_alloc_cb
 * 
 * @param handle 
 * @param suggested_size 
 * @param buf 
 */
void alloc_cb(uv_handle_t *handle, size_t suggested_size, uv_buf_t *buf) {
  buf->base = (char*) xmalloc(suggested_size);
  buf->len = suggested_size;
}

/**
 * @brief step3: on write to client
 * 
 * @param timer
 */
void step3_on_write_client(uv_write_t *req, int status) {
  session_t* session = container_of(req, session_t, res_write);
  log("step3_on_write_client() was called.");
  if (status) {
    log("Write error %s\n", uv_strerror(status));
  }

  session_cleanup(session);
}

/**
 * @brief step2: on timeout
 * 
 * @param timer
 */
void step2_on_timeout(uv_timer_t* timer) {
  session_t* session = container_of(timer, session_t, timer);
  log("step2_on_timeout() was called.");
  session_set_sending_buf(session, g_http_response, strlen(g_http_response));
  uv_write((uv_write_t*) &session->res_write, &session->client, &session->sending_buf, 1, step3_on_write_client);
}

/**
 * @brief step1: on read from client
 * 
 * @param client 
 * @param nread 
 * @param buf 
 */
void step1_on_read_client(uv_stream_t* client, ssize_t nread, const uv_buf_t* buf) {
  session_t* session = container_of(client, session_t, client);
  log("step1_on_read_client() was called.");

  /** successfully received */
  if (nread > 0) {
    uv_read_stop(client);
    uv_timer_start(&session->timer, step2_on_timeout, 3000, 0);
    goto end;
  }

  /** error */
  if (nread < 0) {
    if (nread != UV_EOF) {
      log("Read error %s\n", uv_err_name(nread));
    }
    log("Connection close.");
    session_cleanup(session);
    goto end;
  }

end:
  xfree(buf->base);
}

/**
 * @brief callback on new connection.
 * @see http://docs.libuv.org/en/v1.x/stream.html#c.uv_connection_cb
 * 
 * @param server 
 * @param status 
 */
void on_new_connection(uv_stream_t *server, int status) {
  log("on_new_connection() was called.");
  if (status < 0) {
    log("New connection error %s\n", uv_strerror(status));
    return;
  }

  session_t* s = session_init(uv_default_loop());
  if (uv_accept(server, (uv_stream_t*) &s->client) == 0) {
    log("Connection accepted.");
    uv_read_start((uv_stream_t*) &s->client, alloc_cb, step1_on_read_client);
  } else {
    session_cleanup(s);
  }
}

/**
 * @brief The entry point
 * 
 * @return int 
 */
int main() {
  int ret = 0;

  uv_tcp_t server;
  uv_tcp_init(uv_default_loop(), &server);

  struct sockaddr_in addr;
  ret = uv_ip4_addr("0.0.0.0", EXPOSED_PORT, &addr);
  if (ret) {
    log("IPv4 address conversion error %s\n", uv_strerror(ret));
    return 1;
  }

  ret = uv_tcp_bind(&server, (const struct sockaddr*)&addr, 0);
  if (ret) {
    log("Socket bind error %s\n", uv_strerror(ret));
    return 1;
  }

  ret = uv_listen((uv_stream_t*) &server, DEFAULT_BACKLOG, on_new_connection);
  if (ret) {
    log("Listen error %s\n", uv_strerror(ret));
    return 1;
  }

  log("Server listening on http://localhost:%d ...", EXPOSED_PORT);
  uv_run(uv_default_loop(), UV_RUN_DEFAULT);

  return 0;
}
