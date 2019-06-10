# RTI_XMLOutputUtility
A command line utility to output final XML values provided by a QoS Profile for the RTI Connext DDS Pro product.

## Purpose
In the RTI Connext DDS Pro product an application can be configured using an XML QoS (Quality of Service) configuration file. With the v6.0.0 release of the product we have added features like XML Multiple Inheritance to allow greater reusability and flexibility for the users. This tool is intended to help with that. 

It allows developers to see what the final values (post resolution of inheritance) for XML QoS will be when an entity is created by pointing at a <qos_profile>. The utility also allows developers to see the value of a subtag within one of the standard <xxx_qos> tags by using the '/' separator.

## Compiling
The tool has the following dependencies
1. RTI Connext DDS Pro v6.0.0 or higher (could work with older versions as well)
2. CMake v3.5 or higher

Here is how you would compile this utility after cloning it
Create a build subdirectory and cd into it
```
mkdir build
cd build
```
Create the toolchain file to compile for your platform
```
cmake .. -DCONNEXTDDS_DIR=<RTI Connext DDS installation folder>
```
Compile (e.g. on Linux systems you would do)
```
make
```

## Usage
Here are the various options for this utility

| Option | Explanation | Type |
|--------------|---------------------------------------------------------------------------------------------------------------------------------------------------------------------------|------------------------------------------------------------------------------------------------------------------|
| qosFile | Absolute path of the QoS XML configuration file you want to analyze | OPTIONAL: The standard QoS XML files as defined in the User's manual will still be loaded |
| outputFile | Filename where the utility will output the QoS XML values | OPTIONAL: If not specified the output will be to the console |
| profilePath | The fully qualified path of a QoS Profile e.g. QoSLibraryName::QoSProfileName | OPTIONAL: The,with is_default_qos="true" will be selected OR the default values will be returned for the -qosTag |
| qosTag | The XML tag name who QoS values you want to be fetched  You can also select a subtag by separating it with a '/'  e.g. datawriter_qos/history or participant_qos/property | REQUIRED: Allowed values = {datawriter_qos, datareader_qos, topic_qos, participant_qos, publisher_qos, subscriber_qos} |
| topicName | Can be used with -qosTag = {datawriter_qos, datareader_qos, topic_qos} | OPTIONAL: The default value used with these types will be NULL |
| help | Displays all the options of the RTI_XMLOutputUtility | OPTIONAL |

## Example
Here is a sample usage of the utility on a Linux machine
```
$ ./RTI_XMLOutputUtility -qosFile /home/pralhad/Documents/Tests/CORE-9446/USER_QOS_PROFILES.xml -profilePath Data_Library::Data_Profile -outputFile Dummy.txt -qosTag participant_qos/property
```