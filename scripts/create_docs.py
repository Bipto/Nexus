import os
import xml.etree.ElementTree as ET
import subprocess
import sys

def generate_pdf():   
    output_format = "pdf"

    if len(sys.argv) > 1:
        output_format = sys.argv[1]

    # Directory containing the XML files
    xml_dir = "docbook"
    combined_xml_output = xml_dir + "/combined_docbook.xml"

    # Define the DocBook namespace
    ns = {'db': 'http://docbook.org/ns/docbook'}

    # Register the namespace so it appears nicely in the output
    ET.register_namespace('', ns['db'])

    # Create the root element
    root = ET.Element("{http://docbook.org/ns/docbook}doxygen")

    # Loop through all XML files in the directory
    for filename in os.listdir(xml_dir):
        print(filename)
        if filename.endswith(".xml") and filename != "index.xml":
            filepath = os.path.join(xml_dir, filename)
            try:
                tree = ET.parse(filepath)
                file_root = tree.getroot()

                # Append <section> elements (with namespace)
                if file_root.tag == f"{{{ns['db']}}}section":
                    root.append(file_root)
            except ET.ParseError as e:
                print(f"Skipping {filename}: {e}")

    # Write the combined XML to a file
    tree = ET.ElementTree(root)
    tree.write(combined_xml_output, encoding="utf-8", xml_declaration=True)

    os.chdir(xml_dir)

    # Define the command as a list of arguments
    command = [
        "pandoc",
        "combined_docbook.xml",
        "-f", "docbook",
        "-t", output_format,
        "--toc",
        "--citeproc",
        "-o", "output." + output_format
    ]

    # Run the command
    try:
        subprocess.run(command, check=True)
        print("Conversion successful!")
    except subprocess.CalledProcessError as e:
        print(f"Error during conversion: {e}")
        
def run_doxygen():
    os.chdir("../")
    
    try:
        subprocess.run("doxygen", check=True)
    except subprocess.CalledProcessError as e:
        print(f"Error running doxygen: {e}")
        
    os.chdir("docs")
       
       
if (__name__ == "__main__"):
    run_doxygen()
    generate_pdf()
    print("Done!")
    input()