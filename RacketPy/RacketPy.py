# -*- coding: utf-8 -*-

"""
Copyright 2017
RacketPy
Alexander Kozhinov <AlexanderKozhinov@yandex.com>
"""

import numpy as np
import matplotlib.pyplot as plt


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
    >>> t, acc, gyro, ang = readDataLog(fileName)
    >>> np.shape(t)
    (1240, 1)
    >>> np.shape(acc)
    (1240, 3)
    >>> np.shape(gyro)
    (1240, 3)
    >>> np.shape(ang)
    (1240, 3)
    """
    file = open(fileName, "r")
    dataSet = file.readlines()
    file.close()

    N = np.shape(dataSet)[0]
    M = 3
    t = np.zeros((N, 1))
    acc = np.zeros((N, M))
    gyro = np.zeros((N, M))
    ang = np.zeros((N, M))
    for i, d in enumerate(dataSet):
        t[i], acc[i], gyro[i], ang[i] = encodeRecord(d.strip())

    # Delete minimum time value such that time begins from zero
    t = t - np.min(t)
    ang = np.rad2deg(ang)

    return t, acc, gyro, ang


# -----------------------------------------------------------------------------
def plotData(t, acc, gyro, ang, fig, s=0.75, scatter=True, fontsize=10):
    """
    Plots logged data.

    >>> fileName = './DataSets/DataLog1.txt'
    >>> t, acc, gyro, ang = readDataLog(fileName)
    >>> fig1 = plt.figure()
    >>> plotData(t, acc, gyro, ang, fig=fig1, s=2, scatter=True)
    >>> acc = acc - 1
    >>> plotData(t, acc, gyro, ang, fig=fig1, s=2, scatter=True)
    # >>> plt.show()
    """

    fontSz = fontsize
    plt.figure(fig.number)
    axs = []

    # Plot acceleartions:
    axs.append(plt.subplot(331))
    plt.title('Acceleration', fontsize=fontSz)
    if scatter is True:
        plt.scatter(t, acc[:, 0], s=s)
        plt.ylabel('a_x, m/s^2', fontsize=fontSz)
        axs.append(plt.subplot(334))
        plt.scatter(t, acc[:, 1], s=s)
        plt.ylabel('a_y, m/s^2', fontsize=fontSz)
        axs.append(plt.subplot(337))
        plt.scatter(t, acc[:, 2], s=s)
        plt.ylabel('a_z, m/s^2', fontsize=fontSz)
    else:
        plt.plot(t, acc[:, 0])
        plt.ylabel('a_x, m/s^2', fontsize=fontSz)
        axs.append(plt.subplot(334))
        plt.plot(t, acc[:, 1])
        plt.ylabel('a_y, m/s^2', fontsize=fontSz)
        axs.append(plt.subplot(337))
        plt.plot(t, acc[:, 2])
        plt.ylabel('a_z, m/s^2', fontsize=fontSz)

    # Plot angular speeds:
    axs.append(plt.subplot(332))
    plt.title('Angular speed', fontsize=fontSz)
    if scatter is True:
        plt.scatter(t, gyro[:, 0], s=s)
        plt.ylabel('w_x, °/s', fontsize=fontSz)
        axs.append(plt.subplot(335))
        plt.scatter(t, gyro[:, 1], s=s)
        plt.ylabel('w_y, °/s', fontsize=fontSz)
        axs.append(plt.subplot(338))
        plt.scatter(t, gyro[:, 2], s=s)
        plt.ylabel('w_z, °/s', fontsize=fontSz)
    else:
        plt.plot(t, gyro[:, 0])
        plt.ylabel('w_x, °/s', fontsize=fontSz)
        axs.append(plt.subplot(335))
        plt.plot(t, gyro[:, 1])
        plt.ylabel('w_y, °/s', fontsize=fontSz)
        axs.append(plt.subplot(338))
        plt.plot(t, gyro[:, 2])
        plt.ylabel('w_z, °/s', fontsize=fontSz)

    # Plot orientations:
    axs.append(plt.subplot(333))
    plt.title('Angular speed', fontsize=fontSz)
    if scatter is True:
        plt.scatter(t, ang[:, 0], s=s)
        plt.ylabel('w_x, °/s', fontsize=fontSz)
        axs.append(plt.subplot(336))
        plt.scatter(t, ang[:, 1], s=s)
        plt.ylabel('w_y, °/s', fontsize=fontSz)
        axs.append(plt.subplot(339))
        plt.scatter(t, ang[:, 2], s=s)
        plt.ylabel('w_z, °/s', fontsize=fontSz)
    else:
        plt.plot(t, ang[:, 0])
        plt.ylabel('w_x, °/s', fontsize=fontSz)
        axs.append(plt.subplot(336))
        plt.plot(t, ang[:, 1])
        plt.ylabel('w_y, °/s', fontsize=fontSz)
        axs.append(plt.subplot(339))
        plt.plot(t, ang[:, 2])
        plt.ylabel('w_z, °/s', fontsize=fontSz)

    # Update tick font size of axises:
    for a in axs:
        plt.setp(a.get_xticklabels(), fontsize=fontSz)
        plt.setp(a.get_yticklabels(), fontsize=fontSz)


# -----------------------------------------------------------------------------
def main():
    """
    Main function.
    """
    pass


# -----------------------------------------------------------------------------
if __name__ == "__main__":
    main()
