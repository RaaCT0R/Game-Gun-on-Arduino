# Game Gun

## Overview
This is a self-deploy project. I used an **Arduino** and **MPU6050** sensor and **Qt** for creating a gun for computer games. We place the sensor in a toy gun and make connections to Arduino using wires. In this project, I connect Arduino to computer using serial-port. Then we can capture the motions with sensor, move the datas to computer using Arduino and process them with Qt.

note: This is the first scheme of project. There are lots of space for improvement.

## Motion Capture and Arduino
As I said before, I used MPU6050 for motion capture, using two gyro-axis. 
Accelerometer and gyroscope in the sensor capture the non-gravitational acceleration and the rotational motion. Then I send the raw data to computer for process.

The code is available here: 
- [Arduino-Code](./MPU6050_mouseMove.ino)

## Driver and Qt
The data send and received using UART communication.
After that, there is 4 phase:
- Sensor calibration: The first 100 data will use for sensor calibration.
- Low-Pass-Filter: The LPF needed for better user experience, unless the cursor will vibrate.
- Compute the position: The last step, compute the position of cursor using last-moment data and new data.
- Move cursor: Just need to move the cursor to the new position.

The code is available here: 
- [win_main.cpp](./MouseSensorControl/win_main.cpp)
- [win_main.h](./MouseSensorControl/win_main.h)
- [main.cpp](./MouseSensorControl/main.cpp)

### notes
For moving the cursor, I used Qt built-in libraries such as QCursor but for detecting the left-click(fire button on gun), I used ***windows.h*** library.
The parameters in data process like **SENSIVITY** and **CALIBRE_COUNTER** have been set based on experiance.
