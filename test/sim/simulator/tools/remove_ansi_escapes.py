import re

def remove_ansi_escapes(text):
    ansi_escape = re.compile(r'\x1B(?:[@-Z\\-_]|\[[0-?]*[ -/]*[@-~])')
    return ansi_escape.sub('', text)

def process_log_file(input_file, output_file):
    try:
        with open(input_file, 'r', encoding='utf-8') as infile:
            content = infile.read()
        clean_content = remove_ansi_escapes(content)
        with open(output_file, 'w', encoding='utf-8') as outfile:
            outfile.write(clean_content)
        print(f"Processing completed. The result has been saved to {output_file}")
    except FileNotFoundError:
        print(f"Error: File {input_file} not found")
    except Exception as e:
        print(f"An unknown error occurred: {e}")

if __name__ == "__main__":
    input_log = 'record.log'
    output_log = 'recordWithoutX1B.log'
    process_log_file(input_log, output_log)
