project('tinyxml', 'cpp', version : '0.0', default_options : ['warning_level=3', 'cpp_std=c++20'])
add_project_arguments('-Wfatal-errors', language: 'cpp')
add_project_arguments('-Wno-missing-field-initializers', language: 'cpp')

subdir('src')

executable('example', tinyxml_src + tinyxml_debug_src) 
