project('rkg', 'c')

sources = [
    'src/rkg.c',
    'src/mt19937.c'
]
static_libs = []

cc = meson.get_compiler('c')
if (cc.get_id() == 'msvc')
    static_libs = [
        'bcrypt.lib',
        'Kernel32.lib'
    ]

endif

executable('rkg', sources:sources, link_args: static_libs, include_directories: ['src'])

