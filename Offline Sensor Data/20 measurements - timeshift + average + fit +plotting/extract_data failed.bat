@echo off
REM mkv data extracting script

echo "mkv sensor data extracting script"
echo "mkv sensor data extracting script" > log.txt

set mkvFileName = ".\data2.mkv"

@echo %mkvFileName% >> log.txt
rem ffmpeg -i %mkvFileName% -map 0:4 -f f32le data2_acc.pcm >> log.txt
rem ffmpeg -i %mkvFileName% -map 0:5 -f f32le data2_gyro.pcm
rem ffmpeg -i %mkvFileName% -map 0:6 -f f32le data2_mag.pcm

pause