project('ncc', 'cpp',
        version : '0.1.0',
        default_options : ['cpp_std=c++20'])

cpp_build_args = [
  '-DPROJECT_NAME=' + meson.project_name(),
  '-DPROJECT_VERSION=' + meson.project_version(),
  '-DLOG_USE_COLOR',
]

project_include_directories = [
  'src',
]

project_source_files = [
  'src/log/log.cpp',
  'src/ncc.cpp'
]

project_target = executable(
  meson.project_name(),
  project_source_files,
  include_directories : project_include_directories,
  install : false,
  cpp_args : cpp_build_args,
)
