#ifndef RG11__H
#define RG11__H

struct RG11_pulses_t
{
	uint32_t number_of_pulses;
	uint32_t pulse_length;
};

struct message_payload_t {
	RG11_pulses_t drop_counter;
	RG11_pulses_t condensation_detector;

    uint32_t time_since_boot_in_ms;
    uint32_t time_between_message_updates_in_ms;
};

struct message_t {
    message_payload_t payload;
    uint16_t checksum;
};

#endif // RG11__H