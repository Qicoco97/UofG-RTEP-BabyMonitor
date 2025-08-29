# FAST DDS Demo
We referred to Professor Bernd Porr's demo of dds. https://github.com/berndporr/fastdds_demo.git

(We didn't make significant changes to the source file. We merely changed the class names to our own class names.)

Based on this tutorial: https://fast-dds.docs.eprosima.com/en/latest/fastdds/getting_started/simple_app/simple_app.html

It demonstrates a realtime publisher / subscriber application.

![alt tag](screenshot.png)

## Prereqisites

Fast CDR
```
apt install libfastcdr-dev
```

Fast RTPS
```
apt install libfastrtps-dev
```

Fast DDS tools
```
apt install fastddsgen fastdds-tools
```

## How to compile

```
cmake .
make
```


## How to run

Open a terminal and type
```
./DDSAlarmPublisher
```

Open another terminal and type

```
./DDSAlarmSubscriber
```
