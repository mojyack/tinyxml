project('tinyxml', 'cpp', version : '0.0', default_options : ['warning_level=3', 'cpp_std=c++23'])
add_project_arguments('-Wfatal-errors', language: 'cpp')
add_project_arguments('-Wno-missing-field-initializers', language: 'cpp')

subdir('src')

executable('example', tinyxml_files + tinyxml_debug_files)
