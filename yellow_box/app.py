import os
import requests
import msgpack
import json
from datetime import datetime
from yellow_box import YellowBox, YellowBoxMockup

import click
import socket
import logging

log = logging.getLogger(__name__)

@click.command()
@click.argument(
    'outpath',
    type=click.Path(exists=True, file_okay=False)
    )
@click.option('--debug', is_flag=True)
def main(outpath, debug):
    if debug:
        b = YellowBoxMockup()
    else:
        b = YellowBox()

    while True:
        report = b.read()  # read blocks until a fresh report comes in
        send_to_dominik(report)
        with open(filename(outpath), 'a') as file:
            save_to_file(report, file)


def filename(outpath):
    return os.path.join(
        outpath,
        datetime.utcnow().date().isoformat() + ".jsonl"
        )


def send_to_dominik(report):
    try:
        requests.post(
            'http://ihp-pc41.ethz.ch:5001/yellowbox',
            data={report.__class__.__name__: msgpack.packb(report)}
        )
    except (requests.RequestException, socket.timeout):
        log.exception('could not write to Dominik')


def save_to_file(report, file):
    d = dict(report._asdict())
    d['timestamp_utc'] = datetime.utcnow().isoformat()
    json.dump(d, file)
    file.write('\n')

if __name__ == '__main__':
    main()
