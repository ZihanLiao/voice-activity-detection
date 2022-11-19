for file in `ls /Users/marlowe/Desktop/all-YU`;do
  echo $file
  filename=$(basename $file)
  ./start_vad_test --wave-file /Users/marlowe/Desktop/all-YU/$file --write-file /Users/marlowe/Desktop/all-YU/vad/${filename}
done
