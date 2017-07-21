# -*- coding: utf-8 -*-

"""
Copyright 2017
RacketPy
Alexander Kozhinov <AlexanderKozhinov@yandex.com>
"""

import copy as cp
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
def plotData(t, acc, gyro, ang, fig, s=0.75, lw=1, scatter=True, fontsize=10):
    """
    Plots logged data.

    # >>> fileName = './DataSets/DataLog1.txt'
    # >>> t, acc, gyro, ang = readDataLog(fileName)
    # >>> fig1 = plt.figure()
    # >>> plotData(t, acc, gyro, ang, fig=fig1, s=2, scatter=True)
    # >>> acc = acc - 1
    # >>> plotData(t, acc, gyro, ang, fig=fig1, s=2, scatter=True)
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
        plt.plot(t, acc[:, 0], linewidth=lw)
        plt.ylabel('a_x, m/s^2', fontsize=fontSz)
        axs.append(plt.subplot(334))
        plt.plot(t, acc[:, 1], linewidth=lw)
        plt.ylabel('a_y, m/s^2', fontsize=fontSz)
        axs.append(plt.subplot(337))
        plt.plot(t, acc[:, 2], linewidth=lw)
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
        plt.plot(t, gyro[:, 0], linewidth=lw)
        plt.ylabel('w_x, °/s', fontsize=fontSz)
        axs.append(plt.subplot(335))
        plt.plot(t, gyro[:, 1], linewidth=lw)
        plt.ylabel('w_y, °/s', fontsize=fontSz)
        axs.append(plt.subplot(338))
        plt.plot(t, gyro[:, 2], linewidth=lw)
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
        plt.plot(t, ang[:, 0], linewidth=lw)
        plt.ylabel('w_x, °/s', fontsize=fontSz)
        axs.append(plt.subplot(336))
        plt.plot(t, ang[:, 1], linewidth=lw)
        plt.ylabel('w_y, °/s', fontsize=fontSz)
        axs.append(plt.subplot(339))
        plt.plot(t, ang[:, 2], linewidth=lw)
        plt.ylabel('w_z, °/s', fontsize=fontSz)

    # Update tick font size of axises:
    for a in axs:
        plt.setp(a.get_xticklabels(), fontsize=fontSz)
        plt.setp(a.get_yticklabels(), fontsize=fontSz)


# -----------------------------------------------------------------------------
def centerRecords(recs):
    """
    Centers the datasets
    """
    for i in range(len(recs) - 1):
        # Current and previos idx of min of accX:
        accXMinIdx = np.argmin(recs[i]['acc'], axis=0)[0]
        accXMinNextIdx = np.argmin(recs[i+1]['acc'], axis=0)[0]
        t_shift = recs[i+1]['t'][accXMinNextIdx] - recs[i]['t'][accXMinIdx]
        if t_shift < 0:
            t_shift = (-1.0) * t_shift
        elif t_shift > 0:
            t_shift = (1.0) * t_shift
        recs[i+1]['t'] = recs[i+1]['t'] + t_shift


# -----------------------------------------------------------------------------
def cutRecord(rec, idxL, idxR):
    """
    Cuts the record by left index idxL and right index idxR
    """
    rec['t'] = rec['t'][idxL:idxR]
    rec['acc'] = rec['acc'][idxL:idxR]
    rec['gyro'] = rec['gyro'][idxL:idxR]
    rec['ang'] = rec['ang'][idxL:idxR]


# -----------------------------------------------------------------------------
def main():
    """
    Main function.

    >>> main()
    """
    fileName = './DataSets/DataLog'
    ext = '.txt'  # extension
    N = 8  # number of files

    fig = plt.figure()
    rec = {'t': [], 'acc': [], 'gyro': [], 'ang': []}  # record
    recs = []  # records
    for i in range(N):
        rec['t'], rec['acc'], rec['gyro'], rec['ang'] = \
            readDataLog(fileName + str(i+1) + ext)
        # deepcopy since else all records will be same
        recs.append(cp.deepcopy(rec))

    # Center datasets:
    centerRecords(recs)

    # Cut datasets:
    t_min = []
    t_max = []

    # Find minimal and maximal time values over all records:
    for r in recs:
        t_min.append(np.min(r['t'], axis=0))
        t_max.append(np.max(r['t'], axis=0))

    # Left and right time cut points:
    t_L = np.max(t_min)
    t_R = np.min(t_max)

    # Find indexes of left/right time points in every record and cut them:
    for r in recs:
        idxL = np.max(np.argwhere(r['t'] <= t_L).T[0])
        idxR = np.min(np.argwhere(r['t'] >= t_R).T[0])
        cutRecord(r, idxL, idxR)

    # Plot datasets:
    for r in recs:
        plotData(r['t'], r['acc'], r['gyro'], r['ang'], fig=fig,
                 s=0.5, lw=0.75, scatter=False)
    plt.show()


# -----------------------------------------------------------------------------
if __name__ == "__main__":
    main()
