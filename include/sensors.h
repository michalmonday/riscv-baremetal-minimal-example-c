#ifndef SENSORS_H
#define SENSORS_H

// Location of a small block memory dedicated for sensors (that constantly gets updated by XADC wizard,
// receiving data from up to 16 sensors using multiplexer, one of the sensors is ECG sensor)
static volatile long *analog_sensors = (long *)(long *)0xC0003000;
#define ECG_SENSOR_INDEX 7

// The same small block memory is used for digital inputs (provided from 2 SN74HC165 cascaded shift registers)
static volatile long *digital_sensors = (long *)(long *)(0xC0003000 + (sizeof(long) * 16));


#endif