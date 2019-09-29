# Real Time plot and PSD of raw data from RTL SDR

sudo apt install rtl-sdr

sudo rtl_tcp -f 101200000 # Hz

sudo apt-get install libqwt-qt5-dev qt5-default libliquid-dev

cd qtscope

qmake

make

./qtscope
