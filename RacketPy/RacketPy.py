# -*- coding: utf-8 -*-

"""
Copyright 2017
RacketPy
Alexander Kozhinov <AlexanderKozhinov@yandex.com>
"""

import copy as cp
import numpy as np
import matplotlib.pyplot as plt
from scipy.optimize import curve_fit


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

    >>> fileName = './DataSets/train/DataLog1.txt'
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
def plotData(t, acc, gyro, ang, fig, s=0.75, lw=1, scatter=True, fontsize=6):
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

    acc_name = [r'$a_x$, $m/s^2$', r'$a_y$, $m/s^2$', r'$a_z$, $m/s^2$']
    omega_name = [r'$\omega_x$, $°/s$', r'$\omega_y$, $°/s$',
                  r'$\omega_z$, $°/s$']
    alpha_name = [r'$\alpha_x$, °', r'$\alpha_y$, °', r'$\alpha_z$, °']

    # Plot acceleartions:
    axs.append(plt.subplot(331))
    plt.title('Acceleration', fontsize=fontSz)
    if scatter is True:
        plt.scatter(t, acc[:, 0], s=s)
        plt.ylabel(acc_name[0], fontsize=fontSz)
        axs.append(plt.subplot(334))
        plt.scatter(t, acc[:, 1], s=s)
        plt.ylabel(acc_name[1], fontsize=fontSz)
        axs.append(plt.subplot(337))
        plt.scatter(t, acc[:, 2], s=s)
        plt.ylabel(acc_name[2], fontsize=fontSz)
    else:
        plt.plot(t, acc[:, 0], linewidth=lw)
        plt.ylabel(acc_name[0], fontsize=fontSz)
        axs.append(plt.subplot(334))
        plt.plot(t, acc[:, 1], linewidth=lw)
        plt.ylabel(acc_name[1], fontsize=fontSz)
        axs.append(plt.subplot(337))
        plt.plot(t, acc[:, 2], linewidth=lw)
        plt.ylabel(acc_name[2], fontsize=fontSz)

    # Plot angular speeds:
    axs.append(plt.subplot(332))
    plt.title('Angular speed', fontsize=fontSz)
    if scatter is True:
        plt.scatter(t, gyro[:, 0], s=s)
        plt.ylabel(omega_name[0], fontsize=fontSz)
        axs.append(plt.subplot(335))
        plt.scatter(t, gyro[:, 1], s=s)
        plt.ylabel(omega_name[1], fontsize=fontSz)
        axs.append(plt.subplot(338))
        plt.scatter(t, gyro[:, 2], s=s)
        plt.ylabel(omega_name[2], fontsize=fontSz)
    else:
        plt.plot(t, gyro[:, 0], linewidth=lw)
        plt.ylabel(omega_name[0], fontsize=fontSz)
        axs.append(plt.subplot(335))
        plt.plot(t, gyro[:, 1], linewidth=lw)
        plt.ylabel(omega_name[1], fontsize=fontSz)
        axs.append(plt.subplot(338))
        plt.plot(t, gyro[:, 2], linewidth=lw)
        plt.ylabel(omega_name[2], fontsize=fontSz)

    # Plot orientations:
    axs.append(plt.subplot(333))
    plt.title('Orientation', fontsize=fontSz)
    if scatter is True:
        plt.scatter(t, ang[:, 0], s=s)
        plt.ylabel(alpha_name[0], fontsize=fontSz)
        axs.append(plt.subplot(336))
        plt.scatter(t, ang[:, 1], s=s)
        plt.ylabel(alpha_name[1], fontsize=fontSz)
        axs.append(plt.subplot(339))
        plt.scatter(t, ang[:, 2], s=s)
        plt.ylabel(alpha_name[2], fontsize=fontSz)
    else:
        plt.plot(t, ang[:, 0], linewidth=lw)
        plt.ylabel(alpha_name[0], fontsize=fontSz)
        axs.append(plt.subplot(336))
        plt.plot(t, ang[:, 1], linewidth=lw)
        plt.ylabel(alpha_name[1], fontsize=fontSz)
        axs.append(plt.subplot(339))
        plt.plot(t, ang[:, 2], linewidth=lw)
        plt.ylabel(alpha_name[2], fontsize=fontSz)

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
def cutRecords(recs, nL=0, nR=0):
    """
    Cuts all the records in the record list to same number of samples.
    """
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
        idxL = np.max(np.argwhere(r['t'] <= t_L).T[0]) + nL
        idxR = np.min(np.argwhere(r['t'] >= t_R).T[0]) - nR
        cutRecord(r, idxL, idxR)


# -----------------------------------------------------------------------------
def saveRecord(record, fileName):
    """
    Saves record to file
    """
    file = open(fileName, "w")

    d = '\t'  # delimiter
    n = '\n'  # new line
    header = 't' + d + 'aX' + d + 'aY' + d + 'aZ' + d + \
             'gX' + d + 'gY' + d + 'gZ' + d + \
             'oX' + d + 'oY' + d + 'oZ' + n
    file.write(header)
    for i in range(len(record['t'])):
        line = str(record['t'][i][0]) + d + \
               str(record['acc'][i][0]) + d + \
               str(record['acc'][i][1]) + d + \
               str(record['acc'][i][2]) + d + \
               str(record['gyro'][i][0]) + d + \
               str(record['gyro'][i][1]) + d + \
               str(record['gyro'][i][2]) + d + \
               str(record['ang'][i][0]) + d + \
               str(record['ang'][i][1]) + d + \
               str(record['ang'][i][2]) + n
        file.write(line)
    file.close()


# -----------------------------------------------------------------------------
def gauss1b(x, A, mu, s, b):
    """
    Fits Gaussian with bias

    >>> gauss1b(x=1, A=10, mu=0, s=1, b=0)
    6.0653065971263338
    >>> gauss1b(x=3, A=-3, mu=5, s=6, b=1.1)
    -1.7378784067202964
    """
    # A = (1.0/np.sqrt(2.0 * np.pi * s**2))  # amplitude
    return A * np.exp((-(mu - x)**2) / (2 * s**2)) + b


# -----------------------------------------------------------------------------
def gauss2b(x, A1, mu1, s1, A2, mu2, s2, b):
    """
    Fits Gaussian with bias

    >>> x = 1
    >>> A = np.array([10, -3])
    >>> mu = np.array([1, 2])
    >>> s = np.array([3, 4])
    >>> b = 0
    >>> gauss2b(x=x, A1=A[0], mu1=mu[0], s1=s[0], \
                A2=A[1], mu2=mu[1], s2=s[1], b=b)
    7.0923002965709676
    >>> x = 39
    >>> A = np.array([-1, 0])
    >>> mu = np.array([0.4, 2.3])
    >>> s = np.array([0.5, 20])
    >>> b = 12
    >>> gauss2b(x=x, A1=A[0], mu1=mu[0], s1=s[0], \
                A2=A[1], mu2=mu[1], s2=s[1], b=b)
    12.0
    """
    # print("A1, A2: ", A1, A2)
    # print("mu1, m2: ", mu1, mu2)
    # print("s1, s2: ", s1, s2)
    # print("b: ", b)
    return (gauss1b(x, A1, mu1, s1, 0) + gauss1b(x, A2, mu2, s2, 0) + b)


# -----------------------------------------------------------------------------
def fitGauss2b(x, y, s_max=1000, dmu=20):
    """
    Fits Gauss of second order with bias.
    """
    # Provide mean free data:
    offset = np.mean(y)
    y = y - offset

    # Amplitude:
    A = np.array([np.min(y), np.max(y)])

    # Mean:
    M = np.min(x[np.argwhere(y == A[0])])
    M1 = np.array([M - dmu, M + dmu])
    M = np.max(x[np.argwhere(y == A[1])])
    M2 = np.array([M - dmu, M + dmu])

    # Standart deviation:
    S1 = np.array([0, s_max])
    S2 = np.array([0, s_max])

    # Offset:
    C = A

    # A1, mu1, s1, A2, mu2, s2, b
    bnds = ([A[0], M1[0], S1[0], A[0], M2[0], S2[0], C[0]],  # min bounds
            [A[1], M1[1], S1[1], A[1], M2[1], S2[1], C[1]])  # max bounds
    popt, pcov = curve_fit(gauss2b, x, y, bounds=bnds, method='trf')
    y = y + offset
    popt[-1] = popt[-1] + offset

    # Print strting and estimated parameters:
    """
    print('A1:', A[0], A[1])
    print('M1:', M1[0], M1[1])
    print('S1:', S1[0], S1[1])
    print('A2:', A[0], A[1])
    print('M2:', M2[0], M2[1])
    print('S2:', S2[0], S2[1])
    print('C:', C[0], C[1])
    print("popt: ", popt)
    """
    return popt


# -----------------------------------------------------------------------------
def fitGauss1b(x, y, s_max=1000, dmu=20):
    """
    Fit one dimensional Gaussian with offset
    """
    # Provide mean free data:
    offset = np.mean(y)
    y = y - offset

    # Amplitude as absolute maximum value:
    idxAMax = np.argmax(np.abs(y))
    idxAMin = np.argmin(np.abs(y))
    A = np.array([np.min(np.array([y[idxAMin], y[idxAMax]])),
                  np.max(np.array([y[idxAMin], y[idxAMax]]))])

    # Mean:
    Mmid = np.min(x[idxAMax])
    M = np.array([Mmid - dmu, Mmid + dmu])

    # Standart deviation:
    S = np.array([0, s_max])

    # Offset:
    C = A

    # A1, mu1, s1, b
    bnds = ([A[0], M[0], S[0], C[0]],  # min bounds
            [A[1], M[1], S[1], C[1]])  # max bounds
    popt, pcov = curve_fit(gauss1b, x, y, bounds=bnds, method='trf')
    y = y + offset
    popt[-1] = popt[-1] + offset

    # Print strting and estimated parameters:
    """
    print('A:', A[0], A[1])
    print('M:', M[0], M[1])
    print('S:', S[0], S[1])
    print('C:', C[0], C[1])
    print("popt: ", popt)
    """
    return popt


# -----------------------------------------------------------------------------
def main():
    """
    Main function.

    >>> main()
    """
    fileName = './DataSets/train/DataLog'
    ext = '.txt'  # extension
    N = 8  # number of files

    rec = {'id': 0, 't': [], 'acc': [], 'gyro': [], 'ang': []}  # record
    recs = []  # records
    for i in range(N):
        rec['id'] = i
        rec['t'], rec['acc'], rec['gyro'], rec['ang'] = \
            readDataLog(fileName + str(i+1) + ext)
        # deepcopy since else all records will be same
        recs.append(cp.deepcopy(rec))

    """
    # Delete bad datasets:
    del recs[7]
    del recs[6]
    del recs[0]

    goodIds = [r['id'] for r in recs]
    print(goodIds)
    """

    # Center datasets:
    centerRecords(recs)

    # Cut datasets:
    cutRecords(recs, nL=100, nR=0)

    # Save record:
    saveRecord(recs[0], "./SomeRecord.txt")

    # Fit and plot records:
    fig = plt.figure()
    lineW = 1
    X = []
    for r in recs:
        # Fit record:
        accXOpt = fitGauss2b(r['t'][:, 0], r['acc'][:, 0],
                             s_max=1000, dmu=20)
        accYOpt = fitGauss1b(r['t'][:, 0], r['acc'][:, 1],
                             s_max=1000, dmu=20)
        accZOpt = fitGauss2b(r['t'][:, 0], r['acc'][:, 2],
                             s_max=1000, dmu=20)

        gyroXOpt = fitGauss1b(r['t'][:, 0], r['gyro'][:, 0],
                              s_max=1000, dmu=20)
        gyroYOpt = fitGauss2b(r['t'][:, 0], r['gyro'][:, 1],
                              s_max=1000, dmu=20)
        gyroZOpt = fitGauss1b(r['t'][:, 0], r['gyro'][:, 2],
                              s_max=1000, dmu=20)

        angXOpt = fitGauss1b(r['t'][:, 0], r['ang'][:, 0],
                             s_max=1000, dmu=20)
        angYOpt = fitGauss2b(r['t'][:, 0], r['ang'][:, 1],
                             s_max=1000, dmu=20)
        angZOpt = fitGauss1b(r['t'][:, 0], r['ang'][:, 2],
                             s_max=1000, dmu=20)

        # Create residual matrix with records:

        # Plot record:
        plt.subplot(331)
        plt.plot(r['t'][:, 0], gauss2b(r['t'][:, 0], *accXOpt), 'r-',
                 label='fit-with-bounds', linewidth=lineW)
        plt.subplot(334)
        plt.plot(r['t'][:, 0], gauss1b(r['t'][:, 0], *accYOpt), 'r-',
                 label='fit-with-bounds', linewidth=lineW)
        plt.subplot(337)
        plt.plot(r['t'][:, 0], gauss2b(r['t'][:, 0], *accZOpt), 'r-',
                 label='fit-with-bounds', linewidth=lineW)

        plt.subplot(332)
        plt.plot(r['t'][:, 0], gauss1b(r['t'][:, 0], *gyroXOpt), 'r-',
                 label='fit-with-bounds', linewidth=lineW)
        plt.subplot(335)
        plt.plot(r['t'][:, 0], gauss2b(r['t'][:, 0], *gyroYOpt), 'r-',
                 label='fit-with-bounds', linewidth=lineW)
        plt.subplot(338)
        plt.plot(r['t'][:, 0], gauss1b(r['t'][:, 0], *gyroZOpt), 'r-',
                 label='fit-with-bounds', linewidth=lineW)

        plt.subplot(333)
        plt.plot(r['t'][:, 0], gauss1b(r['t'][:, 0], *angXOpt), 'r-',
                 label='fit-with-bounds', linewidth=lineW)
        plt.subplot(336)
        plt.plot(r['t'][:, 0], gauss2b(r['t'][:, 0], *angYOpt), 'r-',
                 label='fit-with-bounds', linewidth=lineW)
        plt.subplot(339)
        plt.plot(r['t'][:, 0], gauss1b(r['t'][:, 0], *angZOpt), 'r-',
                 label='fit-with-bounds', linewidth=lineW)

    # Plot datasets:
    for r in recs:
        plotData(r['t'], r['acc'], r['gyro'], r['ang'], fig=fig,
                 s=0.05, lw=0.5, scatter=False)
    plt.show()


# -----------------------------------------------------------------------------
if __name__ == "__main__":
    main()
