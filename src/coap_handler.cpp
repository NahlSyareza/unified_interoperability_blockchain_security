#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <coap3/coap.h>
#include "coap_handler.hpp"
#include <spdlog/spdlog.h>
#include "data_route_handler.hpp"

int coap_handler(DataStructure::Instance *ds) {
  // coap_context_t  *ds->coap_ctx = nullptr;
  coap_resource_t *resource = nullptr;
  int result = EXIT_FAILURE;;
  uint32_t scheme_hint_bits;
  coap_addr_info_t *info = nullptr;
  coap_addr_info_t *info_list = nullptr;
  coap_str_const_t *my_address = coap_make_str_const(COAP_LISTEN_UCAST_IP);
  bool have_ep = false;

  /* Initialize libcoap library */
  coap_startup();

  /* Set logging level */
  coap_set_log_level(COAP_LOG_WARN);

  /* Create CoAP context */
  ds->coap_ctx = coap_new_context(nullptr);
  if (!ds->coap_ctx) {
    coap_log_emerg("cannot initialize context\n");
    goto finish;
  }

  /* Let libcoap do the multi-block payload handling (if any) */
  coap_context_set_block_mode(ds->coap_ctx, COAP_BLOCK_USE_LIBCOAP|COAP_BLOCK_SINGLE_BODY);

  scheme_hint_bits = coap_get_available_scheme_hint_bits(0, 0, COAP_PROTO_NONE);
  info_list = coap_resolve_address_info(my_address, 0, 0, 0, 0,
      0,
      scheme_hint_bits, COAP_RESOLVE_TYPE_LOCAL);
  /* Create CoAP listening endpoint(s) */
  for (info = info_list; info != NULL; info = info->next) {
    coap_endpoint_t *ep;

    ep = coap_new_endpoint(ds->coap_ctx, &info->addr, info->proto);
    if (!ep) {
      coap_log_warn("cannot create endpoint for CoAP proto %u\n",
          info->proto);
    } else {
      have_ep = true;
    }
  }
  coap_free_address_info(info_list);
  if (have_ep == false) {
    coap_log_err("No context available for interface '%s'\n",
        (const char *)my_address->s);
    goto finish;
  }

  /* Add in Multicast listening as appropriate */
#ifdef COAP_LISTEN_MCAST_IPV4
  coap_join_mcast_group_intf(ds->coap_ctx, COAP_LISTEN_MCAST_IPV4, NULL);
#endif /* COAP_LISTEN_MCAST_IPV4 */
#ifdef COAP_LISTEN_MCAST_IPV6
  coap_join_mcast_group_intf(ds->coap_ctx, COAP_LISTEN_MCAST_IPV6, NULL);
#endif /* COAP_LISTEN_MCAST_IPV6 */

  /* Create a resource that the server can respond to with information */
  // GET endpoint
  resource = coap_resource_init(coap_make_str_const("coapSen"), 0);

  coap_resource_set_userdata(resource, (void*)ds);

  coap_register_handler(resource, COAP_REQUEST_POST, coap_post_callback);

  coap_add_resource(ds->coap_ctx, resource);


  /* Handle any libcoap I/O requirements */
  while (true) {
    coap_io_process(ds->coap_ctx, COAP_IO_WAIT);
  }

  result = EXIT_SUCCESS;
finish:

  coap_free_context(ds->coap_ctx);
  coap_cleanup();

  return result;
}

void coap_post_callback(coap_resource_t *resource, coap_session_t *session [[maybe_unused]], const coap_pdu_t *request, const coap_string_t *query [[maybe_unused]], coap_pdu_t* response) {
  DataStructure::Instance *ds = (DataStructure::Instance*) coap_resource_get_userdata(resource);

  if(ds->pr_time) { 
    ds->start_time = std::chrono::high_resolution_clock::now();;
  }

  coap_str_const_t *uri = coap_resource_get_uri_path(resource);
  size_t len;
  const uint8_t *databuf;
  size_t offset;
  size_t total;

  if(uri) {
    std::string path((char*)uri->s, uri->length);
    // fprintf(stdout, "Path: %s\n", pth.c_str());

    if (coap_get_data_large(request, &len, &databuf, &offset, &total)) {
      std::string payload((char*)databuf, len);

      ds->universal_map["coap/" + path] = payload;

      data_route_handler(ds, path);
    }
  }

  const char* msg = "Ready to fire sir";

  // coap_show_pdu(COAP_LOG_WARN, request);
  coap_pdu_set_code(response, COAP_RESPONSE_CODE_CONTENT);
  coap_add_data(response, strlen(msg), (const uint8_t*)msg);
  // coap_show_pdu(COAP_LOG_WARN, response);
}


