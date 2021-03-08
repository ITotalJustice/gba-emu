project('gba-emu', ['c', 'cpp'],
    version: '0.0.1',
    default_options: [
        'buildtype=debug',
    ],
)

dependencies = [
]

source = files([
    'src/main.cc',
    'src/arm7tdmi/thumb.cc',
    'src/arm7tdmi/arm.cc',
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
    '-std=c++20',
    # not sure about this because the codebase will still be c-style
    '-Wuseless-cast',
    # not going to use classes..., but just in case i do i'll leave this here
    # '-fdevirtualize-at-ltrans',
    # '-Wsuggest-final-types',
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
