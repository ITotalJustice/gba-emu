project('gba-emu', ['c', 'cpp'],
    version: '0.0.1',
    default_options: [
        'buildtype=debug',
    ],
)

dependencies = [
]

source = files([
    'src/main.cpp',
    'src/arm7tdmi/thumb/thumb.cpp',
    'src/arm7tdmi/arm/arm.cpp',
    #'src/arm7tdmi/arm/data_processing.cpp',
    # tests
    'src/arm7tdmi/tests/test_barrel_shifter.cpp',
    'src/arm7tdmi/tests/test_arm_data_processing.cpp',
    'src/arm7tdmi/tests/test_common.cpp',
])

includes = include_directories([
])

cflags = [
    # optimise
    # '-Ofast',
    # '-flto',
    # '-DNDEBUG',

    # warns
    # '-Wno-unused',
    '-Wall',
    '-Wextra',
    # '-Wno-format',
]
# 0b0000'0000'0000'0000'0000'0000
cppflags = [
    '-std=gnu++20',
    # '-std=gnu++2b',
    # not sure about this because the codebase will still be c-style
    '-Wuseless-cast',
    # not going to use classes..., but just in case i do i'll leave this here
    '-fdevirtualize-at-ltrans',
    '-Wsuggest-final-types',
]

linkflags = [
    # '-Wl,-subsystem,windows',
    # '-s',
]

executable(
    'gba-emu',
    [ source ],
    dependencies : [ dependencies ] ,
    install: false,
    include_directories: [ includes ],
    c_args: [ cflags ],
    cpp_args: [ cppflags, cflags ],
    link_args: [ linkflags ]
)
