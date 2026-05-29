/* minimal CoAP server
 *
 * Copyright (C) 2018-2026 Olaf Bergmann <bergmann@tzi.org>
 */

#include "data_structure.hpp"

/*
 * This server listens to Unicast CoAP traffic coming in on port 5683 and handles it
 * as appropriate.
 *
 * If support for multicast traffic is not required, comment out the COAP_LISTEN_MCAST_IPV*
 * definitions.
 */

#define COAP_LISTEN_UCAST_IP "::"

#define COAP_LISTEN_MCAST_IPV4 "224.0.1.187"
// #define COAP_LISTEN_MCAST_IPV4 "127.0.0.1"
#define COAP_LISTEN_MCAST_IPV6 "ff02::fd"

int coap_handler(DataStructure::Instance *ds);
