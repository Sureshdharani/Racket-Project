# -*- coding: utf-8 -*-

"""
Copyright 2017
Fake UDP data sender
Alexander Kozhinov <AlexanderKozhinov@yandex.com>
"""

import numpy as np
import socket
import timeit
import time


# -----------------------------------------------------------------------------
def sendPacket(ip, port, packet):
    """
    Sends UDP packet to ip on port
    # >>> sendPacket("127.0.0.1", 5555, "Hey")
    """
    # initialize socket
    s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

    # send packet
    s.sendto(packet.encode('ascii', 'replace'), (ip, port))

    # close the socket
    s.close()


# -----------------------------------------------------------------------------
def createFakeAppEdissonPacket(t, acc, gyro, q):
    """
    Creates fake edisson packet:
        t - time stamp as 32 bit int
        acc - three accelerations data (float): [x, y, z]
        gyro - three gyroscope data (float): [x, y, z]
        q - four quaternions data (float): [w, x, y, z]

    Note:
        Packet format as string:
            "s@TimeStamp@accX@accY@accZ@gyroX@gyroY@gyroZ@w@x@y@z@;"

    >>> t = 99999
    >>> acc = np.array([1.1, 2.2, 3.3])
    >>> gyro = np.array([4.4, 5.5, 6.6])
    >>> q = np.array([7.7, 8.8, 9.9, 10.10])
    >>> createFakeAppEdissonPacket(t, acc, gyro, q)
    's@99999@1.1@2.2@3.3@4.4@5.5@6.6@7.7@8.8@9.9@10.1@;'
    """
    packet = "s@" + str(t) +\
             "@" + str(acc[0]) + "@" + str(acc[1]) + "@" + str(acc[2]) +\
             "@" + str(gyro[0]) + "@" + str(gyro[1]) + "@" + str(gyro[2]) +\
             "@" + str(q[0]) + "@" + str(q[1]) + "@" + str(q[2]) +\
             "@" + str(q[3]) + "@;"
    return packet


# -----------------------------------------------------------------------------
def generateRandSensData(acc_mu=0.0, acc_sigma=10,
                         gyro_mu=0.0, gyro_sigma=200,
                         q_mu=0.0, q_sigma=90):
    """
    Generates random sensor data

    # >>> acc, gyro, q = generateRandSensData()
    """

    acc = np.random.normal(acc_mu, acc_sigma, size=3)
    gyro = np.random.normal(gyro_mu, gyro_sigma, size=3)
    q = np.random.normal(q_mu, q_sigma, size=4)

    return acc, gyro, q


# -----------------------------------------------------------------------------
def main():
    """
    Main function.

    # >>> main()
    """

    ip = "127.0.0.1"
    port = 5555

    acc = []
    gyro = []
    q = []
    packets = []
    t_samp = 10 / 1000.0  # 10 ms sampling time

    N = 50  # number of packets to send in one buffer

    print("Server is running...")

    k = 0
    timeStamp = 0
    t = timeit.Timer('char in text',
                     setup='text = "sample string"; char = "g"')
    while (True):
        k = k + 1

        # sample packets to the buffer:
        for i in range(N):
            acc, gyro, q = generateRandSensData()
            p = createFakeAppEdissonPacket(timeStamp, acc, gyro, q)
            packets.append(p)
            time.sleep(t_samp)  # sleep for sampling time
            timeStamp += t.timeit()

        # send buffer
        for p in packets:
            sendPacket(ip, port, p)

        # pause between packets
        # time.sleep(np.random.normal(100, 25, size=1)[0] / 1000.0)
        # print("Sending buffer #%s" % (k))


# -----------------------------------------------------------------------------
if __name__ == "__main__":
    main()
