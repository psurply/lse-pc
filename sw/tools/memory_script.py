dump = '''
begin_memory_edit \\
    -hardware_name "{hardware}" \\
    -device_name "{device}"
puts [ \\
    read_content_from_memory \\
        -instance_index 0 \\
        -content_in_hex \\
        -start_address {offset} \\
        -word_count {length} \\
]
end_memory_edit'''

flash = '''
begin_memory_edit \\
    -hardware_name "{hardware}" \\
    -device_name "{device}"
update_content_to_memory_from_file \\
    -instance_index 0 \\
    -mem_file_path "{mif}" \\
    -mem_file_type "mif"
end_memory_edit'''
