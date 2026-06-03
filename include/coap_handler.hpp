#include "data_structure.hpp"

#define COAP_LISTEN_UCAST_IP "::"

#define COAP_LISTEN_MCAST_IPV4 "224.0.1.187"
#define COAP_LISTEN_MCAST_IPV6 "ff02::fd"

int coap_handler(DataStructure::Instance *ds);
void coap_post_callback(coap_resource_t *resource, coap_session_t *session [[maybe_unused]], const coap_pdu_t *request, const coap_string_t *query, coap_pdu_t* response);
