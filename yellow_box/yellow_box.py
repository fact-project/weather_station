import socket
import time
import struct
from collections import namedtuple
import logging
import random

log = logging.getLogger(__name__)

Report = namedtuple(
    'Report',
    [
        'drop_counter_number_of_pulses',
        'drop_counter_pulse_length',
        'condensation_detector_number_of_pulses',
        'condensation_detector_pulse_length',
        'time_since_boot_in_ms',
        'time_between_message_updates_in_ms',
        'chksum'
    ]
)


class YellowBox:
    def __init__(self,
                 ip='161.72.130.79',
                 time_between_updates_in_ms=1000):
        self.ip = ip
        self.time_between_updates_in_ms = time_between_updates_in_ms
        self.last_time = None

    def read(self):
        while True:
            try:
                msg = struct.unpack("IIIIIIH", request(self.ip))
            except socket.timeout:
                log.exception('timeout when reading from yellow box')
                continue
            if msg[5] != self.time_between_updates_in_ms:
                log.warn(
                    'yellow box time between updates is %f, expected was %f',
                    msg[5],
                    self.time_between_updates_in_ms)
                log.warn('setting internal time between updates to %f', msg[5])
                self.time_between_updates_in_ms = msg[5]

            report = Report(*msg)

            if self.last_time != report.time_since_boot_in_ms:
                self.last_time = report.time_since_boot_in_ms
                break
            time.sleep(self.time_between_updates_in_ms*1e-3 / 3.)
        return report


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


class YellowBoxMockup:
    def __init__(self,
                 time_between_updates_in_ms=1000):
        self.time_between_updates_in_ms = time_between_updates_in_ms
        self.last_time = 0
        self.start_time_in_ms = int(time.time() * 1e3)

    def read(self):
        while True:
            msg = [random.randint(0, 100) for i in range(7)]
            msg[4] = int(time.time() * 1e3) - self.start_time_in_ms
            msg = tuple(msg)
            report = Report(*msg)

            if report.time_since_boot_in_ms - self.last_time > self.time_between_updates_in_ms:
                self.last_time = report.time_since_boot_in_ms
                break
            time.sleep(self.time_between_updates_in_ms*1e-3 / 3.)
        return report


