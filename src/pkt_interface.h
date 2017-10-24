#ifndef	__PKT_INTERFACE_H__
#define	__PKT_INTERFACE_H__

/*
 * An interface is what it sounds like - an interface.
 *
 * It'll handle a variety of packets that are layer 2 of some
 * sort (typically AX25 but hey, who knows what this will grow.)
 */

struct pkt_interface {
	TAILQ_ENTRY(, pkt_interface) n;

	char *name;

	/*
	 * This is the interface owner.
	 *
	 * I think it'll be some kind of routing stack that owns
	 * a list of interfaces which it'll be sending data in and out
	 * of.
	 */
	struct {

	} owner;

	/*
	 * This is the underlying protocol handler.
	 *
	 * This is the L2 protocol itself - APRS-IS, KISS, etc.
	 * It'll take packets from the interface and do encap/decap
	 * as appropriate, or complain that it indeed can't handle
	 * the given packet type.
	 */
	struct {

	} proto;

	/*
	 * This is the list of incoming frames from the protocol handler.
	 */

	/*
	 * This is the list of outgoing frames from the protocol handler.
	 */
};

#endif	/* __PKT_INTERFACE_H__ */
