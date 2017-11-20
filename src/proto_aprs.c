#include <stdio.h>
#include <stdlib.h>

/*
 * This implements the APRS message encode/decode components.
 *
 * It doesn't specifically know about the underlying transport layer
 * (such as AX25, IGATE, etc) - it just takes care of creating APRS
 * messages and dequeuing APRS messages to a handler.
 *
 * A higher level will register one or more interfaces,
 * one of these instances to handle the APRS protocol itself
 * and then will run the APRS timers for beaconing, telemetry,
 * handling relaying, etc as appropriate.
 */

/* Chapter 4 - assigning APRS data in source/dest fields */

/*
 * Destination field - for now, is APZxxx for broadcasts, as
 *   that's experimental.
 */

/*
 * Set the destination field.  TEST or BEACON.  Will look into
 * the others later.
 */

/*
 * Set the digipeater configuration list.  Yes, go figure this out.
 */

/*
 * Set the source address.  APRS can use the SSID in the source
 * address to display a symbol.  Need to see how this works versus
 * allowing multiple sources with the same callsign.
 */

/* Chapter 6 - Time, Position formats */

/* Chapter 7 - APRS data extensions */

/* Chapter 8 - Position reports */

/* Chapter 9 - Compressed location reports */

/* Chapter 10 - MIC-E Data */

/* Chapter 11 - object, item reports */

/* Chapter 12 - weather reports */

/* Chapter 13 - telemetry */


/*
 * Create an APRS message.
 * (Chapter 14.)
 *
 * The normal message format contains:
 * - ':'
 * - 9 character callsign
 * - ':'
 * - 67 character message, ASCII except |, ~ or {
 * - An optional { followed by a 1-5 alphanumeric message identifier
 *   (with no spaces.)
 */

/*
 * Respond to an APRS message.
 * (Chapter 14.)
 *
 * This contains:
 * - ':'
 * - 9 character callsign
 * - ':'
 * - the phrase 'ack', lowercase
 * - the 1-5 alphanumeric message identfier, with no spaces.
 */

/*
 * Reject an APRS message.
 * (Chapter 14.)
 *
 * This contains:
 * - ':'
 * - 9 character callsign
 * - ':'
 * - the phrase 'rej', lowercase
 * - the 1-5 alphanumeric message identfier, with no spaces.
 */

/* XXX TODO: bulletins, etc */


/*
 * Create a test message.
 *
 * The test message begins with ',', and then contains ASCII data.
 */


/*
 * Create a status report.
 * (Chapter 16.)
 *
 * A status report contains the following:
 *
 * '>'
 * 7 byte zulu time (DHMz)
 * 55 character status text
 *
 * A secondary option is available with 62 characters status text and no
 * timestamp.
 *
 * An optional beam heading / radiated power - consumes the last three
 * characters in the status report.
 *
 * An optional grid locator may immediately follow the > and be 4 or 6
 * characters long, followed by a space.  This takes up some space from
 * the beginning of the message text.
 */

/*
 * XXX TODO: look at appendix 1, make sure encoders are written for all of
 * the fields.  Then, yes, make sure decoders are also written.
 */
