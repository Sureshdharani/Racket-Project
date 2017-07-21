clc;
close all;
clear variables;

rec = importdata("SomeRecord.txt");
rec = rec.data;
t = rec(:,1);
aX = rec(:,2);
aY = rec(:,3);
aZ = rec(:,4);
gX = rec(:,5);
gY = rec(:,6);
gZ = rec(:,7);
oX = rec(:,8);
oY = rec(:,9);
oZ = rec(:,10);