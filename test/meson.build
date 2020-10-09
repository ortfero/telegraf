project('telegraf-test', 'cpp',
  license: 'MIT')

inc_dirs = include_directories(
  '../include',
  '../thirdparty/include'
)

compiler = meson.get_compiler('cpp')
deps = []

if build_machine.system() == 'windows'
  ws2_lib = compiler.find_library('ws2_32')
  deps = [ws2_lib]
endif


executable('telegraf-test', 'test.cpp',
  dependencies: deps,
  include_directories: inc_dirs)

