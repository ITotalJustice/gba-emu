project('gba-emu', ['c'],
    version: '0.0.1',
    default_options: [
        'buildtype=debug',
    ],
)

dependencies = [
]

source = files([
    'src/main.c',
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
    link_args: [ linkflags ]
)
