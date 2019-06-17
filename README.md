# RTIXMLOutputUtility
A command line utility to output final XML values provided by a QoS Profile for RTI Connext DDS Professional.

## Purpose
In RTI Connext DDS Professional, an application can be configured using an XML QoS (Quality of Service) configuration file. In 6.0.0, XML multiple inheritance (also known as QoS Profile Composition) allows an XML configuration to be composed from multiple QoS Profiles. This enhancement allows greater reusability and flexibility, but you might want to see what your final values are after the composition occurs. This tool helps with that.

This tool allows you to see what the final values (after resolution of inheritance) for XML QoS will be when an entity is created, by pointing at a ``` <qos_profile> ```. The utility also allows you to see the value of a subtag within one of the standard ``` <xxx_qos> ``` tags by using the '/' separator.

## Compiling
The tool has the following dependencies
1. RTI Connext DDS Pro v6.0.0 or higher (could work with older versions as well)
2. CMake v3.5 or higher

Here is how you would compile this utility after cloning it:

Create a build subdirectory and cd into it
```
mkdir build
cd build
```
Create the toolchain file to compile for your platform
```
cmake .. 
    -DCONNEXTDDS_DIR=<RTI Connext DDS installation folder> 
    -DCONNEXTDDS_ARCH=<RTI Connext DDS target architecture installed>
    -DCMAKE_BUILD_TYPE=Release
```
Compile
```
cmake --build .
```
NOTE: It is not possible to compile this tool against a Windows installation of RTI Connext DDS Professional 6.0.0 due to absence of ``` rtixml2 ``` in the shipped directory. This has been logged internally as an issue and would be fixed in the next major release of the product. For Linux and Mac OS this tool compiles and executes properly.

## Usage
Here are the various options for this utility

| Option | Explanation | Type |
|--------------|---------------------------------------------------------------------------------------------------------------------------------------------------------------------------|------------------------------------------------------------------------------------------------------------------|
| qosFile | Absolute path of the QoS XML configuration file you want to analyze. You can also specify multiple files by separating them with a semicolon ';' and surrounding them with single quotes. For example: ``` 'file_path_1;file_path_2;file_path_3' ``` | OPTIONAL: The standard QoS XML files as defined in the User's manual will still be loaded |
| outputFile | Filename where the utility will output the QoS XML values | OPTIONAL: If not specified the output will be to the console |
| profilePath | Fully qualified path of a QoS Profile. For example: QoSLibraryName::QoSProfileName | OPTIONAL: The ``` <qos_profile>``` with ``` is_default_qos="true" ``` will be selected OR the default values will be returned for the -qosTag |
| qosTag | XML tag name who QoS values you want to be fetched. You can also select a subtag by separating it with a '/'. For example: ``` datawriter_qos/history ``` or ``` participant_qos/property ``` | REQUIRED: Allowed values = {datawriter_qos, datareader_qos, topic_qos, participant_qos, publisher_qos, subscriber_qos} |
| topicName | Can be used with -qosTag = {datawriter_qos, datareader_qos, topic_qos} | OPTIONAL: The default value used with these types will be NULL |
| help | Displays all the options of the RTIXMLOutputUtility | OPTIONAL |

You should also add the the location of the ``` lib ``` folder to your PATH (Windows) or DYLD_LIBRARY_PATH (Mac) or LD_LIBRARY_PATH (Linux) if you compiled the utility using ``` -DBUILD_SHARED_LIBS=1 ```

```
Linux
export LD_LIBRARY_PATH=<RTI Connext DDS installation folder>/lib/<architecture>:$LD_LIBRARY_PATH

Mac OS
export DYLD_LIBRARY_PATH=<RTI Connext DDS installation folder>/lib/<architecture>:$DYLD_LIBRARY_PATH

Windows
set PATH=<RTI Connext DDS installation folder>/lib/<architecture>;%PATH%
```

## Example
Here is a sample usage of the utility on a Linux machine
```
$ ./RTIXMLOutputUtility 
    -qosFile '/home/xxx/Documents/Tests/CORE-9446/USER_QOS_PROFILES.xml;/home/xxx/Documents/Tests/CORE-1375/USER_QOS_PROFILES.xml'
    -profilePath Data_Library::Data_Profile 
    -outputFile Dummy.txt 
    -qosTag participant_qos/property
```
