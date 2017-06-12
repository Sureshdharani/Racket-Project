# -*- coding: utf-8 -*-

"""
Copyright 2017
Fake UDP data sender
Alexander Kozhinov <AlexanderKozhinov@yandex.com>
"""

# import timeit
# import numpy as np
# import scipy.stats as scs
# import matplotlib.pyplot as plt
import socket
# import sys
# import os


# -----------------------------------------------------------------------------
def sendFakeSensorData(ip, port, message):
    """
    Sends an udp packet as string

    >>> sendToUDP("127.0.0.1", 5555, "hey hey")
    """
    # initialize socket
    s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    
    # send the command
    s.sendto(message.encode(), (ip, port))
 
    # close the socket
    s.close()


# -----------------------------------------------------------------------------
def main():
    sendToUDP("127.0.0.1", 5555, "hey hey")


# -----------------------------------------------------------------------------
if __name__ == "__main__":
    main()
