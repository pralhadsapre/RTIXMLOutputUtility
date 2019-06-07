from xml.dom import minidom
import sys, getopt

if len(sys.argv) == 2:
    resource_uri = sys.argv[1]

if resource_uri:
    with open(resource_uri, 'r') as f:
        xml_string = f.read()

    xml_string_stripped = ""
    for line in xml_string.splitlines():
        xml_string_stripped += line.strip()

    reparsed = minidom.parseString(xml_string_stripped)
    pretty_xml = reparsed.toprettyxml(indent="    ")

    with open(resource_uri, 'w') as f:
        f.write(pretty_xml)