import socket
import time
import struct
from collections import namedtuple
Report = namedtuple('Report',
    ['drop_counter_number_of_pulses',
    'drop_counter_pulse_length',
    'condensation_detector_number_of_pulses',
    'condensation_detector_pulse_length',
    'time_since_boot_in_ms',
    'time_between_message_updates_in_ms',
    ]
)

def request(host, port=80):
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.settimeout(5.)
    s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    s.connect((host, port))
    s.send(b"GET / HTTP/1.0\r\n\r\n")
    data = s.recv(1000000)
    s.shutdown(1)
    s.close()
    return data

if __name__ == "__main__":
    while True:
        try:
            msg = struct.unpack("IIIIIIH", request('10.0.100.155'))
            report = Report(
                msg[0],
                msg[1] / (msg[0]*1000) if msg[0] else '-',
                msg[2],
                msg[3] / (msg[2]*1000) if msg[2] else '-',
                msg[4],
                msg[5],
            )

            print(time.asctime(), report)
        except Exception as e:
            print(time.asctime(), e)

        time.sleep(1)
