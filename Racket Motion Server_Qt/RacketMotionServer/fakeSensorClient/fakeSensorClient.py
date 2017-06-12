# -*- coding: utf-8 -*-

"""
Copyright 2017
Fake UDP data sender
Alexander Kozhinov <AlexanderKozhinov@yandex.com>
"""

# import timeit
import numpy as np
# import scipy.stats as scs
# import matplotlib.pyplot as plt
import socket
# import sys
# import os


# -----------------------------------------------------------------------------
def sendFakeSensorData(ip, port, N=100):
    """
    Sends an udp packet as string

    >>> sendFakeSensorData("127.0.0.1", 5555, 3)
    """
    # initialize socket
    s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

    # send the command
    del3 = ", 3,  ";
    del4 = ", 4,  ";
    del5 = ", 5,  ";
    delim = ", ";
    dat = ", 3,  -0.059,  0.098,  9.826, 4,  -0.000,  0.001,  0.001, 5,  16.191, 12.642,-34.497"
    for k in range(0, N):
        data = str(k)
        data = data.join(dat)

        print("data: {", data, "}")
        s.sendto(data.encode('ascii', 'replace'), (ip, port))

    # close the socket
    s.close()


# -----------------------------------------------------------------------------
def main():
    sendFakeSensorData("127.0.0.1", 5555, N=3)


# -----------------------------------------------------------------------------
if __name__ == "__main__":
    main()
