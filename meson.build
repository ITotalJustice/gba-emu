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
    'src/arm7tdmi/tests/test_thumb_load_store.cpp',
    'src/arm7tdmi/tests/test_thumb_branch.cpp',
    'src/arm7tdmi/tests/test_thumb_alu.cpp',
    'src/arm7tdmi/tests/test_thumb_push_pop.cpp',
    'src/arm7tdmi/tests/test_thumb_decode.cpp',

    'src/mem/tests/test_constexpr_mem.cpp',
    
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
    '-Wframe-larger-than=100KB',
    # '-Wno-format',
]

cppflags = [
    '-std=gnu++20',
    # gcc-11
    # '-std=gnu++2b',
    '-Wuseless-cast',
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
