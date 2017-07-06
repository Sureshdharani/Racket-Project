REM mkv data extracting script
 ffmpeg -i data20.mkv -map 0:4 -f f32le data20_acc.pcm 
 ffmpeg -i data20.mkv  -map 0:5 -f f32le data20_gyro.pcm
 ffmpeg -i data20.mkv  -map 0:6 -f f32le data20_mag.pcm