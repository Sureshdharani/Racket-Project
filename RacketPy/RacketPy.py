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
from sklearn.discriminant_analysis import LinearDiscriminantAnalysis as LDA


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
def readScores(fName):
    """
    Reads scores of data set
    """
    file = open(fName, "r")
    scores = file.readlines()
    file.close()
    score = {'id': 0, 'score': 0}
    d = '\t'
    scores = [s.strip().split(d) for s in scores]
    scrs = []
    for s in scores:
        score['id'] = float(s[0])
        score['score'] = float(s[1])
        scrs.append(cp.deepcopy(score))
    return scrs


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
def readDataSet(recordsFileName, labelsFileName, numfiles=1):
    """
    Reads consecutive files to good and bad records.
    """
    # Read scores:
    scrs = readScores(labelsFileName)

    ext = '.txt'  # extension
    rec = {'id': 0, 'score': 0,
           't': [], 'acc': [], 'gyro': [], 'ang': []}  # record
    grecs = []  # good records
    brecs = []  # bad records
    for i in range(numfiles):
        rec['id'] = i+1
        rec['score'] = scrs[i]['score']
        rec['t'], rec['acc'], rec['gyro'], rec['ang'] = \
            readDataLog(recordsFileName + str(i+1) + ext)
        # deepcopy since else all records will be same
        if rec['score'] > 0:  # good score
            grecs.append(cp.deepcopy(rec))
        else:
            brecs.append(cp.deepcopy(rec))
    return grecs, brecs, scrs


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
        t0 = recs[i]['t'][accXMinIdx]
        t1 = recs[i+1]['t'][accXMinNextIdx]
        t_shift = t1 - t0
        if t_shift < 0:
            t_shift = (-1.0) * t_shift
        elif t_shift > 0:
            t_shift = (1.0) * t_shift
        # print(t0, t1, t_shift)
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
    offset = np.median(y)
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
    offset = np.median(y)
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
def createResidual(accXOpt, accYOpt, accZOpt, gyroXOpt, gyroYOpt, gyroZOpt,
                   angXOpt, angYOpt, angZOpt):
    """
    Creates residual (one line) of residual matrix X

    >>> accXO = [11, 12, 13, 14, 15, 16, 17]
    >>> accYO = [21, 22, 23, 24]
    >>> accZO = [31, 32, 33, 34, 35, 36, 37]
    >>> gyroXO = [41, 42, 43, 44]
    >>> gyroYO = [51, 52, 53, 54, 55, 56, 57]
    >>> gyroZO = [61, 62, 63, 64]
    >>> angXO = [71, 72, 73, 74, 75, 76, 77]
    >>> angYO = [81, 82, 83, 84]
    >>> angZO = [91, 92, 93, 94]
    >>> r = createResidual(accXO, accYO, accZO, \
                           gyroXO, gyroYO, gyroZO, \
                           angXO, angYO, angZO)
    >>> ret = [11.0, 12.0, 13.0, 14.0, 15.0, 16.0, 17.0, 21.0, 22.0, \
               23.0, 24.0, 41.0, 42.0, 43.0, 44.0, 51.0, 52.0, 53.0, \
               54.0, 55.0, 56.0, 57.0, 81.0, 82.0, 83.0, 84.0]
    >>> sum(np.array(ret) - np.array(r))
    0.0
    """
    res = []
    res = np.append(res, accXOpt, axis=0)
    res = np.append(res, accYOpt, axis=0)
    res = np.append(res, gyroXOpt, axis=0)
    res = np.append(res, gyroYOpt, axis=0)
    res = np.append(res, angYOpt, axis=0)
    return list(res)


# -----------------------------------------------------------------------------
def extractResidual(x):
    """
    Creates residual (one line) of residual matrix X

    >>> x = [11.0, 12.0, 13.0, 14.0, 15.0, 16.0, 17.0, 21.0, 22.0, \
             23.0, 24.0, 41.0, 42.0, 43.0, 44.0, 51.0, 52.0, 53.0, \
             54.0, 55.0, 56.0, 57.0, 81.0, 82.0, 83.0, 84.0]
    >>> accXO, accYO, gyroXO, gyroYO, angYO = extractResidual(x)
    >>> accXO
    [11.0, 12.0, 13.0, 14.0, 15.0, 16.0, 17.0]
    >>> accYO
    [21.0, 22.0, 23.0, 24.0]
    >>> gyroXO
    [41.0, 42.0, 43.0, 44.0]
    >>> gyroYO
    [51.0, 52.0, 53.0, 54.0, 55.0, 56.0, 57.0]
    >>> angYO
    [81.0, 82.0, 83.0, 84.0]
    """
    accXOpt = x[0:7]
    accYOpt = x[7:11]

    gyroXOpt = x[11:15]
    gyroYOpt = x[15:22]

    angYOpt = x[22::]
    return accXOpt, accYOpt, gyroXOpt, gyroYOpt, angYOpt


# -----------------------------------------------------------------------------
def createFMtrx(recs):
    """
    Creates feauture matrix and labels vector from fit options
    """
    X = []
    y = np.zeros((len(recs),), dtype=np.float)

    """
    accXOpt = []
    accXOpt = []
    accXOpt = []

    gyroXOpt = []
    gyroXOpt = []
    gyroXOpt = []

    angXOpt = []
    angXOpt = []
    angXOpt = []
    """

    for r in recs:
        t = r['t'][:, 0]
        # Fit record:
        accXOpt = fitGauss2b(t, r['acc'][:, 0], s_max=1000, dmu=20)
        accYOpt = fitGauss1b(t, r['acc'][:, 1], s_max=1000, dmu=20)
        accZOpt = fitGauss2b(t, r['acc'][:, 2], s_max=1000, dmu=20)

        gyroXOpt = fitGauss1b(t, r['gyro'][:, 0], s_max=1000, dmu=20)
        gyroYOpt = fitGauss2b(t, r['gyro'][:, 1], s_max=1000, dmu=20)
        gyroZOpt = fitGauss1b(t, r['gyro'][:, 2], s_max=1000, dmu=20)

        angXOpt = fitGauss2b(t, r['ang'][:, 0], s_max=1000, dmu=20)
        angYOpt = fitGauss1b(t, r['ang'][:, 1], s_max=1000, dmu=20)
        angZOpt = fitGauss1b(t, r['ang'][:, 2], s_max=1000, dmu=20)

        # Create residual matrix with records:
        x = createResidual(accXOpt, accYOpt, accZOpt,
                           gyroXOpt, gyroYOpt, gyroZOpt,
                           angXOpt, angYOpt, angZOpt)
        X.append(x)
        y[r['id']-1] = r['score']

    """
    print("i: np.shape(X) =", np.shape(X))
    print("i: type(X): ", type(X))
    print("i: X:\n", X)
    print("-------------")
    print("accXOpt: %s * \n%s" % (len(accXOpt), accXOpt))
    print("accYOpt: %s * \n%s" % (len(accYOpt), accYOpt))
    print("accZOpt: %s\n%s" % (len(accZOpt), accZOpt))

    print("gyroXOpt: %s * \n%s" % (len(gyroXOpt), gyroXOpt))
    print("gyroYOpt: %s * \n%s" % (len(gyroYOpt), gyroYOpt))
    print("gyroZOpt: %s\n%s" % (len(gyroZOpt), gyroZOpt))

    print("angXOpt: %s * \n%s" % (len(gyroXOpt), gyroXOpt))
    print("angYOpt: %s\n%s" % (len(gyroYOpt), gyroYOpt))
    print("angZOpt: %s\n%s" % (len(gyroZOpt), gyroZOpt))
    """

    return np.array(X), y


# -----------------------------------------------------------------------------
def plotRecordsFit(recs, X, fig, linewidth=0.5):
    """
    Plots fit of records in existing figure
    """

    for i, r in enumerate(recs):
        # Time vector
        t = r['t'][:, 0]

        # Extract otions from residuals:
        accXOpt, accYOpt, gyroXOpt, gyroYOpt, angYOpt = extractResidual(X[i])

        # Plot record's fit:
        plt.subplot(331)
        plt.plot(t, gauss2b(t, *accXOpt), 'r-', linewidth=linewidth)
        plt.subplot(334)
        plt.plot(t, gauss1b(t, *accYOpt), 'r-', linewidth=linewidth)
        # plt.subplot(337)
        # plt.plot(t, gauss2b(t, *accZOpt), 'r-', linewidth=linewidth)

        plt.subplot(332)
        plt.plot(t, gauss1b(t, *gyroXOpt), 'r-', linewidth=linewidth)
        plt.subplot(335)
        plt.plot(t, gauss2b(t, *gyroYOpt), 'r-', linewidth=linewidth)
        # plt.subplot(338)
        # plt.plot(t, gauss1b(t, *gyroZOpt), 'r-', linewidth=linewidth)

        # plt.subplot(333)
        # plt.plot(t, gauss2b(t, *angXOpt), 'r-', linewidth=linewidth)
        plt.subplot(336)
        plt.plot(t, gauss1b(t, *angYOpt), 'r-', linewidth=linewidth)
        # plt.subplot(339)
        # plt.plot(t, gauss1b(t, *angZOpt), 'r-', linewidth=linewidth)


# -----------------------------------------------------------------------------
def trimSize(recs, L=0):
    """
    Trims size of the records
        L - length to be trimmed to
    """
    # print("L =", L)
    for r in recs:
        idxMid = int(np.shape(r['t'])[0] / 2)
        # print(r['id'], np.shape(r['t'])[0], idxMid)
        idxL = idxMid - int(L/2)
        idxR = idxMid + int(L/2)
        if np.abs(idxR - idxL) != L:
            idxL = idxL - 1
        r['t'] = r['t'][idxL:idxR]
        r['acc'] = r['acc'][idxL:idxR]
        r['gyro'] = r['gyro'][idxL:idxR]
        r['ang'] = r['ang'][idxL:idxR]


# -----------------------------------------------------------------------------
def appendRecord(dest, source):
    """
    Apends source records set to destination records set
    """
    for r in source:
        dest.append(cp.deepcopy(r))


# -----------------------------------------------------------------------------
def syncRecords(recs):
    """
    Syncgronises records
    """
    for r in recs:
        r['t'] = r['t'] - np.min(r['t'])


# -----------------------------------------------------------------------------
def plotRecords(fig, recs, bad=True):
    """
    Plots records
    """
    for r in recs:
        if r['score'] > 0:  # Plot good records as a line
            plotData(r['t'], r['acc'], r['gyro'], r['ang'], fig=fig,
                     s=0.05, lw=0.5, scatter=False)
        elif r['score'] < 0 and bad:  # Plot bad records as scatter
            plotData(r['t'], r['acc'], r['gyro'], r['ang'], fig=fig,
                     s=0.05, lw=0.5, scatter=True)


# -----------------------------------------------------------------------------
def main(stateprint=False):
    """
    Main function.

    >>> main()
    """
    if stateprint:
        print("***** START *****")
        print("\t* Reading datasets...")

    # **********************************************************************
    # *
    # * 1) Preprocess Train Data Set
    # *
    # **********************************************************************
    # Reading train data set
    trainSetFileName = './DataSets/train/DataLog'
    trainLabelsFileName = './DataSets/train/Labels.txt'
    Ntrain = 29  # number of train files
    grecs_tr, brecs_tr, scrs_tr = readDataSet(trainSetFileName,
                                              trainLabelsFileName,
                                              numfiles=Ntrain)

    # Center good/bad train records:
    centerRecords(grecs_tr)
    centerRecords(brecs_tr)

    # Cut good/bad records:
    cutRecords(grecs_tr, nL=150, nR=134)
    trimSize(brecs_tr, L=np.shape(grecs_tr[0]['t'])[0])

    # Append centerd bad to good records
    # and center them:
    recs_tr = cp.deepcopy(grecs_tr)
    appendRecord(recs_tr, brecs_tr)

    # Shift to zero time:
    syncRecords(recs_tr)
    # print("len(recs) =", len(recs))

    # Save record:
    # saveRecord(recs[0], "./SomeRecord.txt")

    # Fit train records:
    X_tr, y_tr = createFMtrx(recs_tr)

    # **********************************************************************

    # **********************************************************************
    # *
    # * 2) Preprocess Test Data Set
    # *
    # **********************************************************************
    # Reading test data set:
    testSetFileName = './DataSets/test/DataLog'
    testLabelsFileName = './DataSets/test/Labels.txt'
    Ntest = 15  # number of test files

    grecs_ts, brecs_ts, scrs_ts = readDataSet(testSetFileName,
                                              testLabelsFileName,
                                              numfiles=Ntest)

    # Center good/bad train records:
    centerRecords(grecs_ts)
    centerRecords(brecs_ts)
    grecs_ts[0]['t'] = grecs_ts[0]['t'] + 6500  # correct one wrong shift

    # Cut good/bad records:
    cutRecords(grecs_ts, nL=400, nR=185)
    trimSize(brecs_ts, L=np.shape(grecs_ts[0]['t'])[0])

    # Append centerd bad to good test records
    # and center them:
    recs_ts = cp.deepcopy(grecs_ts)
    appendRecord(recs_ts, brecs_ts)

    # Shift to zero time:
    syncRecords(recs_ts)

    # Fit test records:
    X_ts, y_ts = createFMtrx(recs_ts)
    print(y_ts)

    # **********************************************************************
    # *
    # * 3) Train Classifier
    # *
    # **********************************************************************
    # Create classifier and train int on test data set:
    # print(X)
    clf = LDA(n_components=None, priors=None, shrinkage=None,
              solver='svd', store_covariance=True, tol=0.01).fit(X_tr, y_tr)

    # **********************************************************************
    # *
    # * 4) Test Classifier
    # *
    # **********************************************************************
    # Proove classification on test data set
    for i in range(np.shape(X_ts)[0]):
        x = np.reshape(X_ts[i], (1, np.shape(X_ts)[1]))
        # print(np.shape(x))
        print('id:', i+1, '; pred label: ', clf.predict(x))
    score_clf = clf.score(X_ts, y_ts)
    print(score_clf)
    # print(np.shape(clf.coef_), clf.coef_)
    # print(clf.intercept_)

    # **********************************************************************
    # *
    # * 5) Show results
    # *
    # **********************************************************************
    # Plot fitted good records:
    fig_tr = plt.figure()
    plotRecordsFit(recs_tr, X_tr, fig=fig_tr, linewidth=0.5)
    plotRecords(fig_tr, recs_tr, bad=True)

    # Plot test records:
    fig_ts = plt.figure()
    plotRecordsFit(recs_ts, X_ts, fig=fig_ts, linewidth=0.5)
    plotRecords(fig_ts, recs_ts)
    plt.show()

    print("\t* Fit window length:", np.size(grecs_tr[0]['t']))
    if stateprint:
        print("\t* Fit window length:", np.size(grecs_tr[0]['t']))
        print("***** DONE *****")


# -----------------------------------------------------------------------------
if __name__ == "__main__":
    main(stateprint=True)
