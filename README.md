# Fall-Detection-Device
A device that can identify if a person is about to fall, and send an SMS to a designated person's phone. The above code is uploaded to the ESP-32 microcontroller
The sensor used is MPU 6050 with both accelerometer and gyroscope. The values are read by the ESP-32 microcontroller, which has integrated WiFi on it.
The detection is based on simple thresholding for the roll and pitch angles, as well as the angular velocity obtained from testing.
Here is a video of the working : https://drive.google.com/file/d/1WkWyM9KP5VbhOZ1B3qf6n7z2iXGzekNu/view?usp=share_link
