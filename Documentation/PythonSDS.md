# SDS Nodes

- [SDS Nodes](#sds-nodes)
  - [Introduction](#introduction)
  - [SDS Sensor](#sds-sensor)
    - [With drift compensation](#with-drift-compensation)
  - [SDS Recorder](#sds-recorder)


## Introduction 
The [SDS Framework](https://github.com/ARM-software/SDS-Framework/tree/main) is an Arm framework that allows to record streams of samples and replay them in Virtual Hardware.

The SDS nodes provides a way to use SDS from CMSIS-Stream.

Sensors are described with a `.yml` file containing the following information:
* Sampling rate
* Datatype of a sample

This `.yml` file is used in the Python API to create the sensor and recorder nodes.

## SDS Sensor

Example of use with an accelerometer sensor:

```Python
accelerometer = SDSSensor("accelerometerSensor",NB_SAMPLES,
                          sds_yml_file="Accelerometer.sds.yml",
                          sds_connection="sensorConn_accelerometer")
```

The first field is the name of the node like with any other CMSIS-Stream node.

The second field is the number of samples produced by this node. 

Note that this is not sensor related but application related. The user must decides (depending on the use case), how many samples are required from this sensor in the CMSIS-Stream graph.

In synchronous mode, sensor nodes are blocking until the number of samples has been received. It must be taken into account when there are several sensors in a graph.

Note that in general in a CMSIS-Stream node, the number of samples is given in the output type unit. Here the output type is `uint8_t` but the number of samples is not the number of bytes. The sensor `.yml` file is used to compute the size of a sensor sample in bytes and the node will generate the right number of bytes.


`sds_yml_file` is the `.yml` file that describes the sensor.
`sds_connection` is the C variable (of type `sds_sensor_cg_connection_t`) used to communicate with the sensor.


### With drift compensation

With drift compensation, you need to specify the function to use for drift compensation (using `drift_delegate`) and the C variable containing the data required by this function (using `drift_delegate_data`)

```Python
accelerometer = SDSSensor("accelerometerSensor",ACC_BLOCK,
                          sds_yml_file="Accelerometer.sds.yml",
                          sds_connection="sensorConn_accelerometer",
                          drift_delegate=delegate,
                          drift_delegate_data=delegate_data)
```

## SDS Recorder

```Python
accelerometerRec = SDSRecorder("accelerometerRecorder",NB_SAMPLES,
                                sds_yml_file="Accelerometer.sds.yml",
                                sds_connection="recConn_accelerometer"
                              )
```

If the type is defined using argument `the_type`, the `.yml` file is not needed anymore.

`NB_SAMPLES` is the number of samples even when the type is not specified and is implicitely `uint8_t`. In that case, the `.yml` file is used to compute the sample size in bytes.

`sds_yml_file` is the `.yml` file that describes the sensor.
`sds_connection` is the C variable (of type `sds_recorder_cg_connection_t`) used to communicate with the recorder.