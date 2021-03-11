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
    
    # cpu
    'src/arm7tdmi/thumb/thumb.cpp',
    'src/arm7tdmi/arm/arm.cpp',
    #'src/arm7tdmi/arm/data_processing.cpp',
    # tests
    'src/arm7tdmi/tests/test_barrel_shifter.cpp',
    'src/arm7tdmi/tests/test_arm_data_processing.cpp',
    'src/arm7tdmi/tests/test_arm_multiply.cpp',
    'src/arm7tdmi/tests/test_common.cpp',
    'src/arm7tdmi/tests/test_register_banking.cpp',
    
    # cart
    'src/cart/tests/test_header_checksum.cpp',
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
    '-Wswitch-enum',
    '-Wduplicated-branches',
    '-Wduplicated-cond',
    '-Wshadow',
    '-Wfatal-errors',
    '-Wdouble-promotion',
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
    # this is needed for <execution> algorithms
    # '-ltbb',

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
