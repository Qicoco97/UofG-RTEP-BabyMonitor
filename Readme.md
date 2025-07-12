#### Dependencies
``` bash
sudo apt update
sudo apt install libopencv-dev libgpiod-dev libwebsocketpp-dev libboost-all-dev libcamera-dev libfastcdr-dev libfastrtps-dev fastddsgen fastdds-tools
libqwt-qt5-dev qtdeclarative5-dev qtcharts
```

#### Build & Run
``` bash
git clone https://github.com/Qicoco97/UofG-RTEP-BabyMonitor.git
cd src_final 
cmake .
make
./baby
```
Then you will see the QT window running and monitoring
![](./img%26demo/Output.png)
![](./img%26demo/window.png)
if you want to subscribe the information, you need to following bash
``` bash
cd dds_pub-sub
./DDSAlarmSubscriber
```
then you will see
![](./img%26demo/Subscribe.png)

and you can see our demo at https://www.bilibili.com/video/BV1gLGLzrE4g/ or our img&demo dir

#### Wiring 
For the wiring and GPIO use see the following pic
![](./img%26demo/Wiring.png)
#### Issues
when make, you may find error: qtchartview.h: No such file or directory, go to baby_autogen/include/mainwindow_ui.h change it with 
``` bash
#include <QtCharts>
#include "qchartview.h"
QT_CHARTS_USE_NAMESPACE
```

### Credits 
The libcamera2opencv and Fastdds code was based on my teacher's code https://github.com/berndporr/libcamera2opencv/ adn https://github.com/berndporr/fast-dds-demo/ Thanks to my teacher and i learned a lot from this lecture !!! And the Original idea was came up with cribsense project! Thanks all

### Future work
may use the PM2.5 module to monitor the air quality
