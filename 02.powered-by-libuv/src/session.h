#ifndef SESSION_H_INCLUDED
#define SESSION_H_INCLUDED

#include <uv.h>
#include <assert.h>

#include "./shared.h"

/** HTTP session */
typedef struct {
  /** Accepted TCP connection */
  uv_tcp_t client;
  /** Timer while request and response */
  uv_timer_t timer;
  /** HTTP Response write request */
  uv_write_t res_write;
  /** HTTP Response writing buffer */
  uv_buf_t sending_buf;
} session_t;

/**
 * @brief Initialize session_t
 * 
 * @param loop 
 * @return session_t* 
 */
static session_t* session_init(uv_loop_t* loop) {
  session_t* s = (session_t*)xmalloc(sizeof(session_t));
  *s = (session_t){0};

  uv_tcp_init(loop, &s->client);
  uv_timer_init(loop, &s->timer);

  return s;
}

/**
 * @brief set writing buffer
 * 
 * @param s
 * @param data char pointer of sending data
 * @param len length of data
 */
static void session_set_sending_buf(session_t* s, const char* data, size_t len) {
  s->sending_buf.base = data;
  s->sending_buf.len = len;
}

/**
 * @brief http://docs.libuv.org/en/v1.x/handle.html#c.uv_close_cb
 * 
 * @param client 
 */
static void session_on_close_client(uv_handle_t* client) {
  xfree(container_of(client, session_t, client));
}

/**
 * @brief Cleanup session_t
 * 
 * @param s
 */
static void session_cleanup(session_t* s) {
  assert(s != NULL);

  uv_timer_stop(&s->timer);
  uv_close(&s->client, session_on_close_client);
}

#endif /** include guard */
