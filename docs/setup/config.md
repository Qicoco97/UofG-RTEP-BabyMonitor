# Configuration

The configuration of the baby monitor uses a file usually located at `/etc/babymonitor/config.ini`.

Its format is the same as that of an INI file: each line represents a configuration value, and directives are grouped in square brackets. Comments are separated by `;` until the end of the line.

## Input/Output

The `[io]` section of the file configures the input to the baby monitor.

The `input` directive specifies that the magnifier should look for a video file instead of a live camera input, while the `camera` directive selects the camera device to use (e.g. `camera=0` means use `/dev/video0` as input).

File input exists only as a demonstration or debugging feature.

If you change the input, you must also specify the fps parameter to match the input.

For file input there is only one fps setting as frames are never dropped, whereas for camera input, to reduce latency you must specify the frames per second for both the full and cropped frame sizes (approximately 3 times the full frame size fps).

The latter values ​​depend on the speedo ft and CPU on which baby monitor run.

Note that when using input files with less than 15 fps, `crop_fps` should be set to the fps value of the input file to ensure correct calculation of the bandpass frequencies.

`time_to_alarm` determines the number of system time seconds the baby monitor waits before playing the alarm sound through the audio port after it stops detecting motion.

## Cropping

The `[cropping]` section controls motion-based adaptive cropping, focusing the upscaling process on a smaller region of interest (ROI) where the most motion occurs, thus reducing CPU load.

The different parameters affect the latency of the detection, as they control the number of "slow" frames (completely uncropped).

The best values ​​for the parameters depend on the target CPU.

If you are running on something other than a Pi, and the device is powerful enough, you can also disable cropping altogether.

The default configuration will update the crop approximately every minute.

## Motion & Magnification

The `[motion]` and `[magnification]` sections control the motion detection and video magnification algorithms respectively.

These parameters depend on the environment in which the baby monitor is deployed, such as lighting conditions and contrast of the baby.

You may need to calibrate `erode_dim` and `dilate_dim`, which are used to determine where to crop the video, as well as `diff_threshold` and `pixel_threshold`, which determine the threshold used to judge how much a pixel changes, and the number of moving pixels required before classifying motion as motion rather than noise.

Specifically, `diff_threshold` specifies how much a grayscale pixel (value between 0 and 255) needs to change before it is marked as changed.

For example, if `diff_threshold` is 30, the difference between pixels must be equal to or greater than 30 to be marked as changed.

The higher this threshold, the more different the pixels need to be labeled.

`pixel_threshold` defines a hard limit on the number of pixels that need to be marked as changed before a motion value is output.

This effectively sets the noise cutoff for determining whether the baby is breathing.

For example, if `pixel_threshold` is set to 100, the algorithm must detect a change of more than 100 pixels before registering any visible motion. If the threshold is not exceeded, no motion is reported.

`erode_dim` specifies the dimension of the kernel used in the [OpenCV erosion operation](http://docs.opencv.org/2.4/doc/tutorials/imgproc/erosion_dilatation/erosion_dilatation.html).

This is used to minimize the changed pixels.

That is, isolated pixels are removed, but when large groups of pixels are changed, those pixels are preserved.

`dilate_dim` does the opposite, it takes pixels and dilates them.

These two parameters are used to detect the area of ​​the frame where the baby is.

First, the pixel differences are calculated, then a small erosion is applied to remove noise, and a large dilation is applied to roughly mark the area of ​​motion.

`Low Cutoff` and `High Cutoff` define the range of the bandpass filter used during amplification.

Specifically, video upscaling will attempt to amplify motion that occurs within this frequency range, while ignoring motion outside of this range.

We have adjusted it so that it can capture the average breathing rat, but you may need to adjust it during calibration.

See the section on calibration for more information.

## Debugging features

The `show_diff` flag in `[motion]` will display a window where areas in the frame where motion was detected are highlighted in white.

The `show_magnification` flag in `[magnification]` controls a window that shows just the output of the video magnification (it should look like a black and white image of the camera feed, with magnified motion).

You can use these two flags to display the results of changes to the motion and magnification parameters.

Finally, `print_times` in the `[debug]` section controls the printing of frame times to standard output, which you can use to calibrate FPS and latency settings when running on a device other than the Raspberry Pi.

When starting Baby Monitor via systemd (automatically at boot or with `systemctl start`), these features must be turned off. They are only useful when running Baby Monitor manually.

## Calibrating the Motion & Magnification algorithm

Algorithm calibration is an iterative process with no right or wrong answer.

We encourage you to experiment with various values ​​and combine them with the debugging feature to find the combination of parameters that works best for your environment.

As a guideline, increasing the `amplification` and `phase_threshold` values ​​increases the amount of amplification applied to the input video.

You should change these values ​​until you can clearly see the motion of your baby's breathing and there are no noticeable artifacts in other areas of the video.

If you experience artifacts, lowering the `phase_threshold` while keeping the same `amplification` may help.

You can see the effect of these parameters by setting `show_magnification` to `true`.

As for the motion detection parameters, the main driving factor is the amount of noise.

When detecting motion regions to be cropped, `erode_dim` and `dilate_dim` are used to adjust the size of the OpenCV kernels used for the [erosion and dilation](http://docs.opencv.org/2.4/doc/tutorials/imgproc/erosion_dilatation/erosion_dilatation.html) motion in order to first remove the noise and then significantly dilate the remaining motion signal to make the motion regions apparent.

If your crib is in a very high contrast environment, you may also need to adjust these parameters.

Generally speaking, for high contrast settings you want a higher erode_dim, and for low contrast settings you want a lower erode_dim.

If you run with `show_diff=true` and you notice that too many parts of your input video are white, or that some completely irrelevant parts of the video are detected as motion (such as a flashing light), then you will want to increase `erode_dim` until the only parts of the video corresponding to your breathing baby are the largest white parts.

Once the calibration is complete you will want to make sure the `pixel_threshold` is set so that motion is only reported when you expect it, rather than constantly (which means you will need to remove noise).

Ideally, you would see output like this in your terminal:

```sh
[info] Pixel Movement: 0	 [info] Motion Estimate: 1.219812 Hz
[info] Pixel Movement: 0	 [info] Motion Estimate: 1.219812 Hz
[info] Pixel Movement: 0	 [info] Motion Estimate: 1.219812 Hz
[info] Pixel Movement: 0	 [info] Motion Estimate: 1.219812 Hz
[info] Pixel Movement: 44	 [info] Motion Estimate: 1.219812 Hz
[info] Pixel Movement: 0	 [info] Motion Estimate: 1.219812 Hz
[info] Pixel Movement: 161	 [info] Motion Estimate: 1.219812 Hz
[info] Pixel Movement: 121	 [info] Motion Estimate: 0.841416 Hz
[info] Pixel Movement: 0	 [info] Motion Estimate: 0.841416 Hz
[info] Pixel Movement: 86	 [info] Motion Estimate: 0.841416 Hz
[info] Pixel Movement: 0	 [info] Motion Estimate: 0.841416 Hz
[info] Pixel Movement: 0	 [info] Motion Estimate: 0.841416 Hz
[info] Pixel Movement: 0	 [info] Motion Estimate: 0.841416 Hz
[info] Pixel Movement: 0	 [info] Motion Estimate: 0.841416 Hz
[info] Pixel Movement: 0	 [info] Motion Estimate: 0.841416 Hz
[info] Pixel Movement: 0	 [info] Motion Estimate: 0.841416 Hz
[info] Pixel Movement: 0	 [info] Motion Estimate: 0.841416 Hz
[info] Pixel Movement: 0	 [info] Motion Estimate: 0.841416 Hz
[info] Pixel Movement: 0	 [info] Motion Estimate: 0.841416 Hz
[info] Pixel Movement: 0	 [info] Motion Estimate: 0.841416 Hz
[info] Pixel Movement: 0	 [info] Motion Estimate: 0.841416 Hz
[info] Pixel Movement: 0	 [info] Motion Estimate: 0.841416 Hz
[info] Pixel Movement: 0	 [info] Motion Estimate: 0.841416 Hz
[info] Pixel Movement: 0	 [info] Motion Estimate: 0.841416 Hz
[info] Pixel Movement: 0	 [info] Motion Estimate: 0.841416 Hz
[info] Pixel Movement: 97	 [info] Motion Estimate: 0.841416 Hz
[info] Pixel Movement: 74	 [info] Motion Estimate: 0.839298 Hz
[info] Pixel Movement: 0	 [info] Motion Estimate: 0.839298 Hz
[info] Pixel Movement: 60	 [info] Motion Estimate: 0.839298 Hz
[info] Pixel Movement: 0	 [info] Motion Estimate: 0.839298 Hz
[info] Pixel Movement: 0	 [info] Motion Estimate: 0.839298 Hz
[info] Pixel Movement: 0	 [info] Motion Estimate: 0.839298 Hz
[info] Pixel Movement: 0	 [info] Motion Estimate: 0.839298 Hz
[info] Pixel Movement: 48	 [info] Motion Estimate: 0.839298 Hz
[info] Pixel Movement: 38	 [info] Motion Estimate: 0.839298 Hz
[info] Pixel Movement: 29	 [info] Motion Estimate: 0.839298 Hz
[info] Pixel Movement: 28	 [info] Motion Estimate: 0.839298 Hz
[info] Pixel Movement: 22	 [info] Motion Estimate: 0.839298 Hz
[info] Pixel Movement: 0	 [info] Motion Estimate: 0.839298 Hz
[info] Pixel Movement: 0	 [info] Motion Estimate: 0.839298 Hz
[info] Pixel Movement: 0	 [info] Motion Estimate: 0.839298 Hz
[info] Pixel Movement: 0	 [info] Motion Estimate: 0.839298 Hz
```

There are clear periodic patterns that correspond to the movements.

If your output looks more like this:

```
[info] Pixel Movement: 921	 [info] Motion Estimate: 1.352046 Hz
[info] Pixel Movement: 736	 [info] Motion Estimate: 1.352046 Hz
[info] Pixel Movement: 666	 [info] Motion Estimate: 1.352046 Hz
[info] Pixel Movement: 663	 [info] Motion Estimate: 1.352046 Hz
[info] Pixel Movement: 1196	 [info] Motion Estimate: 1.352046 Hz
[info] Pixel Movement: 1235	 [info] Motion Estimate: 1.352046 Hz
[info] Pixel Movement: 1187	 [info] Motion Estimate: 1.456389 Hz
[info] Pixel Movement: 1115	 [info] Motion Estimate: 1.456389 Hz
[info] Pixel Movement: 959	 [info] Motion Estimate: 1.456389 Hz
[info] Pixel Movement: 744	 [info] Motion Estimate: 1.456389 Hz
[info] Pixel Movement: 611	 [info] Motion Estimate: 1.456389 Hz
[info] Pixel Movement: 468	 [info] Motion Estimate: 1.456389 Hz
[info] Pixel Movement: 371	 [info] Motion Estimate: 1.456389 Hz
[info] Pixel Movement: 307	 [info] Motion Estimate: 1.456389 Hz
[info] Pixel Movement: 270	 [info] Motion Estimate: 1.456389 Hz
[info] Pixel Movement: 234	 [info] Motion Estimate: 1.456389 Hz
[info] Pixel Movement: 197	 [info] Motion Estimate: 1.456389 Hz
[info] Pixel Movement: 179	 [info] Motion Estimate: 1.456389 Hz
[info] Pixel Movement: 164	 [info] Motion Estimate: 1.456389 Hz
[info] Pixel Movement: 239	 [info] Motion Estimate: 1.456389 Hz
[info] Pixel Movement: 733	 [info] Motion Estimate: 1.456389 Hz
[info] Pixel Movement: 686	 [info] Motion Estimate: 1.229389 Hz
[info] Pixel Movement: 667	 [info] Motion Estimate: 1.229389 Hz
[info] Pixel Movement: 607	 [info] Motion Estimate: 1.229389 Hz
[info] Pixel Movement: 544	 [info] Motion Estimate: 1.229389 Hz
[info] Pixel Movement: 499	 [info] Motion Estimate: 1.229389 Hz
[info] Pixel Movement: 434	 [info] Motion Estimate: 1.229389 Hz
[info] Pixel Movement: 396	 [info] Motion Estimate: 1.229389 Hz
[info] Pixel Movement: 375	 [info] Motion Estimate: 1.229389 Hz
[info] Pixel Movement: 389	 [info] Motion Estimate: 1.229389 Hz
[info] Pixel Movement: 305	 [info] Motion Estimate: 1.312346 Hz
[info] Pixel Movement: 269	 [info] Motion Estimate: 1.312346 Hz
[info] Pixel Movement: 1382	 [info] Motion Estimate: 1.312346 Hz
[info] Pixel Movement: 1086	 [info] Motion Estimate: 1.312346 Hz
[info] Pixel Movement: 1049	 [info] Motion Estimate: 1.312346 Hz
[info] Pixel Movement: 811	 [info] Motion Estimate: 1.312346 Hz
[info] Pixel Movement: 601	 [info] Motion Estimate: 1.312346 Hz
[info] Pixel Movement: 456	 [info] Motion Estimate: 1.312346 Hz
```

You then need to adjust the `pixel_threshold` and `diff_threshold` until you see a peak, otherwise the pixel shift is 0.
