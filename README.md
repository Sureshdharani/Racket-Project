# Racket Project


Racket Project is about estimating movement of racket motion to predict if it was good or bad one.

The mathematical model was written in Python. The LDA algorithm were trained and testet on 65 and 28 training and test sets accordingly. All 93 recordings were previously gathered with an program on Edisson module (RacketSensorClient) placed on real racket.
The results of LDA precision were never under 80% in testing case. By training LDA and testing window length L for live prediction were defined.
Other part of Python script were used to simulate live prediction where window of length L were shifted over some randomly choosene record of length N, where N > L. The prediction accuracy in this case were never under 90%.

Although in real live tests (see RacketSensorServer written in Qt) the prediction accuracy were a bit better than 50%.
We assume that the problem in this case is not enough data records to train LDA.

Please contuct us if you are interested in use of this project for your own purpoces.
