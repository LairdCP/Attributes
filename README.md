# Attribute Module

The attribute module is a system for saving configuration to non-volatile memory, reporting read-only/run-time parameters, and issuing commands.

Hash requires gperf.  It should be installed by Zephyr, but can be installed using chocolatey.

## API

The attribute API is described using OpenRPC in the file attributes.json.


Keywords specific to this implementation are prefixed with "x-". Keywords contain information about the external API and control the generation of C-code. They affect the behavior of the attribute system. For example, "x-writable" allows a value to be written from the SMP interface.

### Identification

Each attribute has a sequential ID. The ID is used to reference each attribute in the SMP command.

## SMP Interface

Attributes are accessed by Bluetooth clients using the SMP service that is part of [mcumgr](https://github.com/apache/mynewt-mcumgr). The SMP service and mcumgr are also used for Firmware Update.The custom SMP commands are described in sentrius_methods.json and implemented in sentrius_mgmt.c. This c-file is not generated.

The Pinnacle Connect mobile app has implemented the custom commands using the Bluetooth transport. The SMP interface is also available from a serial port using the mcumgr [cli](https://github.com/apache/mynewt-mcumgr-cli), but the custom commands have not been implemented in the mcumgr cli.

A single attribute can be written using the SMP set command. It can be read using the get command.

Multiple attributes can be set by writing a file and then issuing the Load Parameter command. Attributes can be read by issuing the Dump command followed by reading the output file. Parameters that don't have the readable flag set cannot be dumped. File system management (read/writing files) is a compile time option for mcumgr.

## Framework

If ATTR_BROADCAST is enabled in Kconfig, then the message framework is required to build the attribute module. Setting the "x-broadcast" field to true will cause a framework message to be sent when the value changes. This allows different modules to act when a value is modified.

## Notifications

When a value changes, an SMP notification can be sent to a Bluetooth client. This reduces the need for polling. This operation is controlled by the Set Notify and Disable Notify commands. All notifications are disabled when the Bluetooth connection is closed.

## Validation

Before an attribute is written, its validator is called. The validator checks that the value is in range. If the value is not in range, the write fails.

There are default validators for each data type.

Validators are controlled by the minimum and maximum values in the attributes.json file. If the min/max values are the same, then the validator accepts all values. For strings, the min/max values are interpreted as lengths. The max value isn't optional for a string because it is used by the code generator.

The default validator checks that the value being written is different than the current value. This prevents a flash write and system message when an attribute is set with the same value.

Validators assume the type of data being written matches their type. When values are written from the SMP interface, the CBOR message includes the data type. The CBOR type must map to a valid attribute type.

A custom validator can be created for any attribute.

### Control Points

An attribute can be used as a control point. A control point is an alternative to creating a new SMP command.

As in Bluetooth, writing a control point causes an action to occur. Most control points require the use of special validators that don't care if the value has changed. There are predefined control point validators for the numeric types.

## File Operations

Files can be used to configure a device. Each line in the file contains an id, an equal sign, either a string or hex array, and a newline. Hex strings are least significant byte first. The passkey (4) below is 123456. Leading zeros are not required for IDs.

An attribute file can also be used to obtain the current state of a device.

Example File:
```
location=freezer
key=40e20100
name=pinnacle_100_354616090287629
```

## Non-Volatile Storage

All savable values are saved to the filesystem any time they change. When a device resets, values are restored from the filesystem.

### File Validation

When a file is loaded, all values in a file must be valid or the entire file is rejected.

When the file is loaded it is processed twice. During the first pass it is validated and during the second pass each value is written.

#### File Limitations

If a validator checks multiple attributes, then file validation may fail when the current values aren't compatible with the new values. Until this module supports structures, byte arrays that map to structures can be used as a workaround.

When designing attribute changed handlers, the handler shouldn't assume attributes are written in a specific order.

## Attribute Shell

The attribute shell can be used to read and write values from the terminal. Type 'attr' to get a list of all commands. Attributes can be referenced by name or number.

When writing byte arrays, the entire array must be provided.
```
uart:~$ attr set location desktop
<dbg> attr.save_attributes: Wrote 617 of 617 bytes of parameters to file
<inf> attr: [001] location                      'desktop'

uart:~$ attr set 1 cupboard
<inf> attr: [001] location                      'cupboard'
```

When a value changes, a message is printed to the log (info level). The quiet command can be used to silence changes. For example, to disable the printing of the power supply voltage.
```
<inf> attr: [175] power_supply_voltage           4.0360e+00
<inf> attr: [180] lte_sinr                       29
<inf> attr: [175] power_supply_voltage           4.0400e+00
<inf> attr: [180] lte_sinr                       31
<inf> attr: [175] power_supply_voltage           4.0430e+00
<inf> attr: [180] lte_sinr                       27

uart:~$ attr quiet power_supply_voltage 1
<dbg> fsu.fsu_wa_abs: /lfs/quiet.bin write (16)

<inf> attr: [180] lte_sinr                       30
<inf> attr: [180] lte_sinr                       28
```

## Implementation Details

A module may not read or write values from interrupt context because read/write operations are mutex protected.

This module was designed using littlefs (LFS). For simplicity, all values are written to a single file. Avoid configuring attributes that change often to savable.

The attribute module must be initialized after the file system and should be initialized before most other modules and main. This allows configuration to be ready for use.

For a released schema, savable values cannot be deleted, they must be set to deprecated. This prevents the attribute loader from encountering undefined attributes when loading files.

### Prepare for Read

Sometimes an action is desired before a value is read. For example, when reading a temperature, one might want the current value and not the last measurement. In this case, 'x-prepare' can be set to true. The designer must then implement the attr_prepare_temperature function.

### Enumerations

The API file can be used to describe enumerations. The parameter 'x-enum-include-errno' can be set to true to also include negative error numbers.

### Projects

There are common attribute files (include and src folders) and files specific to each project. Each attribute has an 'x-project' field that indicates what project it is for.

To generate the custom files for new project, run the python generator in the generator folder. Have the command point to location of the application index file that contains the references for the attributes.
Example of what the project index file will look like:
```
openrpc: 1.2.6
info:
  title: Project_Index
  version: 0.0.1
methods: []
components:
  contentDescriptors:
    device_params:
      name: device_parameters
      schema:
        type: array
      x-device-parameters:
        - $ref: ./app/attributes/app_params.yml
	  x-BT610:
        - $ref: ./components/flags/attributes/flags_params.yml
```
Example of the generate command. With the index file named app.yml
```
python attribute_generator.py C:/Industrial_BT_IO/BT6xx/bt6xx_firmware BT610
```

## Customization

For a custom project, a new attributes file can be created. The IDs will be created at generation time.

## Custom Field Definitions

The following fields are specific to this implementation and are not part of the OpenRPC specification. They are prefixed with 'x-'.

| Field                    | Required | Default | Description                                                                                |
| ------------------------ | -------- | ------- | ------------------------------------------------------------------------------------------ |
| x-id                     | y        | NA      | Unique ID of attribute                                                                     |
| x-ctype                  | y        | NA      | Variable type in C. "string" converted to "char" by generator.                             |
| x-default                | y        | NA      | Default value                                                                              |
| x-example                | n        | NA      | A commonly used or alternate value                                                         |
| x-broadcast              | n        | false   | If framework is enabled, generate attribute changed broadcast message                      |
| x-savable                | n        | false   | Save value to file system                                                                  |
| x-writable               | n        | false   | If true, value can be written from SMP or loaded from file                                 |
| x-readable               | n        | false   | If true, value can be read from SMP and dumped to a file                                   |
| x-array-size             | n        | 0       | Number of elements for an array type                                                       |
| x-deprecated             | n        | false   | No longer used, prevents re-use of id or failures due to missing id                        |
| x-validator              | n        | type    | Override for write validation function. Often used for control points                      |
| x-prepare                | n        | NA      | Optional function that is called before a value is read or dumped                          |
| x-enum-include-errno     | n        | false   | If true, then include negative error numbers as part of enum                               |
| x-notify-if-unchanged    | n        | false   | If true, report value even if it is unchanged                                              |
| x-obscure-in-show        | n        | false   | If true, obscured attribute value with asterisks when requested                            |
| x-hide-in-show           | n        | false   | If true, hide attribute show nothing when requested                                        |

The following are used by the methods

| Field                   | Required | Default | Description                                                             |
| ----------------------- | -------- | ------- | ----------------------------------------------------------------------- |
| x-management-option     | y        | NA      | The options for the smp interface for the method: ReadWrite, Write, Read|

Custom validators are specified by the suffix of the validator function name.
```
"x-validator": "cp32"
```

Prepare functions must be defined in application. They are of the form attr_prepare_NAME. Example for the variable named up_time.
```
int attr_prepare_up_time(void);
```
