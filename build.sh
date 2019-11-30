cd audio
clang++ libmain.cpp -std=c++11 -I./portaudio/include/ ./portaudio/libportaudio_static.a -framework CoreServices -framework CoreFoundation -framework AudioUnit -framework AudioToolbox -framework CoreAudio -shared -o libpedal.so
cp libpedal.so ../python
cd ../python

pyfigloc=$(python -c "import os; import pyfiglet; print(os.path.dirname(pyfiglet.__file__))")

echo $#
if [ $# -gt 0 ]
then
pyinstaller -F --add-data 'libpedal.so:.' --add-data "$pyfigloc:./pyfiglet" loopalopobre.py
fi
