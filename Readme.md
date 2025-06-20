#### Dependencies
``` bash
sudo apt update
sudo apt install libopencv-dev libgpiod-dev libwebsocketpp-dev libboost-all-dev libcamera-dev
```

``` bash
# Clone and build libcamera2opencv (required for video streaming)

git clone https://github.com/berndporr/libcamera2opencv.git
cd libcamera2opencv
mkdir build && cd build
cmake ..
make
sudo make install
```

#### Build & Run
``` bash
git clone https://github.com/RickyElE/Team18_Robot_Prj.git
cd Team18_robot_prj
mkdir build && cd build
cmake ../
make
./main
```
Then you need to open the website folder and execute the command:
``` bash
cd ../website
python3 -m http.server 8085
```

#### Access the Interface
Open your browser and go to:
- http://**your-raspberry-pi-ip**:8085

---

### License