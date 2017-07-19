# -*- coding: utf-8 -*-

"""
Copyright 2017
Fake UDP data sender
Alexander Kozhinov <AlexanderKozhinov@yandex.com>
"""

import numpy as np
import argparse
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
def createFakePacketQuaternion(t, acc, gyro, q):
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
    >>> createFakePacketQuaternion(t, acc, gyro, q)
    's@99999@1.1@2.2@3.3@4.4@5.5@6.6@7.7@8.8@9.9@10.1@;'
    """
    packet = "s@" + str(t) +\
             "@" + str(acc[0]) + "@" + str(acc[1]) + "@" + str(acc[2]) +\
             "@" + str(gyro[0]) + "@" + str(gyro[1]) + "@" + str(gyro[2]) +\
             "@" + str(q[0]) + "@" + str(q[1]) + "@" + str(q[2]) +\
             "@" + str(q[3]) + "@;"
    return packet


# -----------------------------------------------------------------------------
def createFakePacketEuler(t, acc, gyro, ang):
    """
    Creates fake edisson packet:
        t - time stamp as 32 bit int
        acc - three accelerations data (float): [x, y, z]
        gyro - three gyroscope data (float): [x, y, z]
        ang - Euler angle data (float): [x, y, z]

    Note:
        Packet format as string:
            "s@TimeStamp@accX@accY@accZ@gyroX@gyroY@gyroZ@angX@angY@angZ@;"

    >>> t = 99999
    >>> acc = np.array([1.1, 2.2, 3.3])
    >>> gyro = np.array([4.4, 5.5, 6.6])
    >>> ang = np.array([7.7, 8.8, 9.9])
    >>> createFakePacketEuler(t, acc, gyro, ang)
    's@99999@1.1@2.2@3.3@4.4@5.5@6.6@7.7@8.8@9.9@;'
    """
    packet = "s@" + str(t) +\
             "@" + str(acc[0]) + "@" + str(acc[1]) + "@" + str(acc[2]) +\
             "@" + str(gyro[0]) + "@" + str(gyro[1]) + "@" + str(gyro[2]) +\
             "@" + str(ang[0]) + "@" + str(ang[1]) + "@" + str(ang[2]) +\
             "@;"
    return packet


# -----------------------------------------------------------------------------
def generateRandSensData(acc_mu=0.0, acc_sigma=10,
                         gyro_mu=0.0, gyro_sigma=200,
                         ang_mu=0.0, ang_sigma=360,
                         q_mu=0.0, q_sigma=90, isRandom=True):
    """
    Generates random sensor data.

    >>> acc, gyro, ang, q = generateRandSensData(isRandom=False)
    >>> acc
    array([1, 2, 3])
    >>> gyro
    array([4, 5, 6])
    >>> ang
    array([7, 8, 9])
    >>> q
    array([10, 11, 12, 13])
    """

    acc = []
    gyro = []
    ang = []
    q = []
    if isRandom:
        acc = np.random.normal(acc_mu, acc_sigma, size=3)
        gyro = np.random.normal(gyro_mu, gyro_sigma, size=3)
        ang = np.random.normal(ang_mu, ang_sigma, size=3)
        q = np.random.normal(q_mu, q_sigma, size=4)
    else:
        acc = np.array([1, 2, 3])
        gyro = np.array([4, 5, 6])
        ang = np.array([7, 8, 9])
        q = np.array([10, 11, 12, 13])

    return acc, gyro, ang, q


# -----------------------------------------------------------------------------
def str2bool(v):
    if v.lower() in ('yes', 'true', 't', 'y', '1'):
        return True
    elif v.lower() in ('no', 'false', 'f', 'n', '0'):
        return False
    else:
        raise argparse.ArgumentTypeError('Boolean value expected.')


# -----------------------------------------------------------------------------
def main():
    """
    Main function.

    # >>> main()
    """

    parser = argparse.ArgumentParser()
    parser.add_argument('-ip', required=True, help='destination ip adress')
    parser.add_argument('-port', required=True, type=int,
                        help='destination port')
    parser.add_argument('-random', required=True,
                        help='set to true to activate random values')
    parser.add_argument('-n', required=False, type=int, default=50,
                        help='buffer size to be sent')
    parser.add_argument('-eulerquat', required=False, default="true",
                        help='Quaternions or Euler angles to be sent '
                             '(default: true - for Euler angles)')
    args = parser.parse_args()

    ip = args.ip
    port = args.port
    random = str2bool(args.random)
    N = args.n  # number of packets to send in one buffer
    isEuler = str2bool(args.random)

    acc = []
    gyro = []
    q = []
    packets = []
    t_samp = 20 / 10000.0  # 20 ms sampling time
    t_transfer = 1000 / 1000.0  # transfer time

    print("Start server...")
    print("Send packets to  %s:%s with buffer size %s." % (ip, port, N))
    print("Sending random packets: %s" % random)
    print("Server is running...")

    k = 0
    timeStamp = 0
    t = timeit.Timer('char in text',
                     setup='text = "sample string"; char = "g"')
    while (True):
        k = k + 1

        # sample packets to the buffer:
        for i in range(N):
            # Create random packet:
            acc, gyro, ang, q = generateRandSensData(isRandom=random)
            pQ = createFakePacketQuaternion(timeStamp, acc, gyro, q)
            pE = createFakePacketEuler(timeStamp, acc, gyro, ang)

            print(pE)

            # Append new packet to the buffer:
            if isEuler is True:
                packets.append(pE)
            else:
                packets.append(pQ)

            # Sleep - sampling time
            time.sleep(t_samp)
            timeStamp += t.timeit()

        # Send buffer
        for p in packets:
            sendPacket(ip, port, p)

        # Pause between packets
        time.sleep(t_transfer)
        # print("Buffer #%s were sent." % (k))


# -----------------------------------------------------------------------------
if __name__ == "__main__":
    main()
