#include <util/atomic.h>
#include <avr/wdt.h>
#include <SPI.h>
#include <Ethernet.h>
#include "Arduino.h"

#include "checksum_fletcher16.h"
#include "RG11.h"

byte mac[] = {0x02, 0xFA, 0xC7, 0x00, 0x00, 0x10};
IPAddress ip(161, 72, 130, 79);


const int condensation_detector_pin = 2;
volatile unsigned long condensation_detector_number_of_pulses;
volatile unsigned long condensation_detector_pulse_length;

const int drop_counter_pin = 3;
volatile unsigned long drop_counter_number_of_pulses;
volatile unsigned long drop_counter_pulse_length;

const unsigned long time_between_message_updates_in_ms = 1UL*1000UL;

/* The relais pin needs to be 'debounced', i.e. for a single pulse
 * one sees several tiny pulses in quick succession on the relais pin within
 * few microseconds. In this circuit, I only see this on the rising edge of
 * the actual pulse.
 * A typical "rain drop pulse" from the RG11 is hundreds of milliseconds long
 * and the only variation I see is in multiples of 100ms.
 *
 * In order to get rid of these tiny bouncing pulses, I debounce like this:
 *   - the first rising edge counts, i.e. starts the stopwatch
 *   - a falling edge only counts, if the rising edge is more than 5ms ago.
 *
 * the 5ms is defined by the constant named: debounce_time below.
*/
const unsigned long debounce_time = 5000UL;


void drop_counter_pulse_ISR ()
{
    static unsigned long time_of_last_valid_transition = 0;
    static bool inside_a_pulse = false;

    unsigned long duration_of_current_phase = micros() - time_of_last_valid_transition;
    // ignore everything, which happens too fast after a transition
    if (duration_of_current_phase < debounce_time)
        return;

    byte pin_state = digitalRead(drop_counter_pin);

    // ignore a falling edge, if we are already in a pulse
    // ignore a rising edge, if we are outside of a pulse
    // the binary x-or thing below is a shorter form of
    // these two if clauses.
    /*
    if (pin_state == LOW && inside_a_pulse)
        return;

    if (pin_state == HIGH && !inside_a_pulse)
        return;
    */
    if (pin_state ^ inside_a_pulse)
        return;

    // We arrived here, so this is a valid transition.
    time_of_last_valid_transition = micros();
    inside_a_pulse = !inside_a_pulse;

    if (pin_state == HIGH)
    {
        drop_counter_number_of_pulses++;
        drop_counter_pulse_length += duration_of_current_phase;
    }
}


void condensation_detector_pulse_ISR ()
{
    static unsigned long time_of_last_valid_transition = 0;
    static bool inside_a_pulse = false;

    unsigned long duration_of_current_phase = micros() - time_of_last_valid_transition;
    // ignore everything, which happens too fast after a transition
    if (duration_of_current_phase < debounce_time)
        return;

    byte pin_state = digitalRead(condensation_detector_pin);

    // ignore a falling edge, if we are already in a pulse
    // ignore a rising edge, if we are outside of a pulse
    // the binary x-or thing below is a shorter form of
    // these two if clauses.
    /*
    if (pin_state == LOW && inside_a_pulse)
        return;

    if (pin_state == HIGH && !inside_a_pulse)
        return;
    */
    if (pin_state ^ inside_a_pulse)
        return;

    // We arrived here, so this is a valid transition.
    time_of_last_valid_transition = micros();
    inside_a_pulse = !inside_a_pulse;

    if (pin_state == HIGH)
    {
        condensation_detector_number_of_pulses++;
        condensation_detector_pulse_length += duration_of_current_phase;
    }
}

EthernetServer server(80);
message_t msg;

void setup() {
    Ethernet.begin(mac, ip);
    server.begin();

    pinMode(drop_counter_pin, INPUT);
    attachInterrupt(
        digitalPinToInterrupt(drop_counter_pin),
        drop_counter_pulse_ISR,
        CHANGE
    );

    attachInterrupt(
        digitalPinToInterrupt(condensation_detector_pin),
        condensation_detector_pulse_ISR,
        CHANGE
    );

    wdt_enable(WDTO_8S);
}

/* Check if a client connected via ethernet and requested a status message
 *  if so:
 *    - send out one status message
 *    - reset the watchdog timer
 *    - close connection to signal end of message
 *
 *  Note: If watchdog timer is not reset within 8sec
 *        The watchdog will reset the MCU.
 *        So in case of network issues, which make it impossible to
 *        have a client connecting to us, there is a chance for this MCU reset
 *        to nicely reset the wiznet chip and allow a client to connect again.
 *
 *  Note: in order to request a status message, any valid HTTP request is okay
 *        any valid HTTP request ends in a "double enter" == "\n\n"
 *        so we just look for a "\n\n" in order to send a status message.
*/
void ethernet_communication() {

    EthernetClient client = server.available();
    if (client) {
        boolean currentLineIsBlank = true;
        while (client.connected()) {
            if (client.available()) {
                char c = client.read();

                if (c == '\n' && currentLineIsBlank) {
                    wdt_reset();
                    client.write((byte*)&msg, sizeof(message_t));
                    break;
                }
                if (c == '\n') {
                    currentLineIsBlank = true;
                }
                else if (c != '\r') {
                    currentLineIsBlank = false;
                }
            }
        }
        delay(1);
        client.stop();
    }
}

void loop() {
    unsigned long current_millis = millis();
    static unsigned long time_of_last_message_update = 0;

    if (current_millis - time_of_last_message_update > time_between_message_updates_in_ms)
    {
        msg.payload.time_since_boot_in_ms = current_millis;

        ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
        {
            msg.payload.drop_counter.number_of_pulses = drop_counter_number_of_pulses;
            msg.payload.drop_counter.pulse_length = drop_counter_pulse_length;
            msg.payload.condensation_detector.number_of_pulses = condensation_detector_number_of_pulses;
            msg.payload.condensation_detector.pulse_length = condensation_detector_pulse_length;

            msg.payload.time_between_message_updates_in_ms = time_between_message_updates_in_ms;

            drop_counter_number_of_pulses = 0;
            drop_counter_pulse_length = 0;
            condensation_detector_number_of_pulses = 0;
            condensation_detector_pulse_length = 0;
        }

        msg.checksum = checksum_fletcher16(
            (byte *)&(msg.payload),
            sizeof(message_payload_t)
        );
        time_of_last_message_update = current_millis;
    }

    ethernet_communication();
}
