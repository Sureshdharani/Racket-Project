# -*- coding: utf-8 -*-

"""
Copyright 2017
RacketPy
Alexander Kozhinov <AlexanderKozhinov@yandex.com>
"""

import numpy as np
# import argparse


# -----------------------------------------------------------------------------
def encodeRecord(record):
    """
    Encodes sensor packet
    >>> record = "s@123@0.01@0.06@1.11@-22.00@8.00@24.00@0.02@-0.03@-0.40@;"
    >>> t, acc, gyro, ang = encodeRecord(record)
    >>> t
    123.0
    >>> acc
    array([ 0.01,  0.06,  1.11])
    >>> gyro
    array([-22.,   8.,  24.])
    >>> ang
    array([ 0.02, -0.03, -0.4 ])
    """
    # Return parameters

    d = record[1]  # delimiter
    record = record[2:-2]
    record = record.split(d)
    record = [float(r) for r in record]

    t = record[0]
    acc = np.array(record[1:4])
    gyro = np.array(record[4:7])
    ang = np.array(record[7:10])
    return t, acc, gyro, ang


# -----------------------------------------------------------------------------
def readDataLog(fileName):
    """
    Reads data log file.

    >>> fileName = './DataSets/DataLog1.txt'
    >>> readDataLog(fileName)
    ''
    """
    pass


# -----------------------------------------------------------------------------
def main():
    """
    Main function.

    # >>> main()
    """
    pass


# -----------------------------------------------------------------------------
if __name__ == "__main__":
    main()
