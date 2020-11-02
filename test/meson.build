project('telegraf-test', 'cpp',
  license: 'MIT',
  default_options: [
    'cpp_std=c++17'
  ])

inc_dirs = include_directories(
  '../include',
  '../include/telegraf/bundled',
  '../thirdparty/include'
)

mbedtls = dependency('mbedtls')

deps = [mbedtls]

system = host_machine.system()

if system == 'windows'
  cpp = meson.get_compiler('cpp')
  ws2 = cpp.find_library('ws2_32', required:true)
  deps += [ws2]
endif

exe = executable('telegraf-test', 'test.cpp',
  include_directories: inc_dirs,
  dependencies: deps
) 

test('telegraf-test', exe)

