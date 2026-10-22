# Options
mcu      = get_option('mcu')
board    = mcu + '_' + get_option('board')

# Local paths
root = meson.current_source_dir()

# -----------------------------------------------------------------------------
# Unity (assumed subproject)
# -----------------------------------------------------------------------------

unity_sp = subproject('unity')
unity_dep = unity_sp.get_variable('unity_dep')


# -------------------------------
# Include directories
# -------------------------------

inc = include_directories(
  'ra/fsp/inc',
  'ra/fsp/inc/api',
  'ra/fsp/inc/instances',
  'ra/board/' + board,
  'ra/arm/CMSIS_6/CMSIS/Core/Include',

  'ra_cfg/fsp_cfg',
  'ra_cfg/fsp_cfg/bsp',

  'ra_gen',
  'src',
  'src/stdio_uart',
  '.',  # Debug headers etc
  'build/cmake.arm.debug.R7FA8E1AF'
)

# -------------------------------
# Sources(e2studio build console)
# -------------------------------

sources_common = files(
  'src/hal_warmstart.c',
  'ra_gen/pin_data.c',
  'ra_gen/common_data.c',
  'ra_gen/vector_data.c',
  'ra/fsp/src/r_ioport/r_ioport.c',
  'ra/fsp/src/r_sci_b_uart/r_sci_b_uart.c',
  'ra/fsp/src/bsp/mcu/'+ mcu +'/bsp_linker.c',
  'ra/fsp/src/bsp/mcu/all/bsp_common.c',
  'ra/fsp/src/bsp/mcu/all/bsp_clocks.c',
  'ra/fsp/src/bsp/mcu/all/bsp_delay.c',
  'ra/fsp/src/bsp/mcu/all/bsp_group_irq.c',
  'ra/fsp/src/bsp/mcu/all/bsp_guard.c',
  'ra/fsp/src/bsp/mcu/all/bsp_ipc.c',
  'ra/fsp/src/bsp/mcu/all/bsp_io.c',
  'ra/fsp/src/bsp/mcu/all/bsp_irq.c',
  'ra/fsp/src/bsp/mcu/all/bsp_macl.c',
  'ra/fsp/src/bsp/mcu/all/bsp_ospi_b.c',
  'ra/fsp/src/bsp/mcu/all/bsp_register_protection.c',
  'ra/fsp/src/bsp/mcu/all/bsp_sdram.c',
  'ra/fsp/src/bsp/mcu/all/bsp_sbrk.c',
  'ra/fsp/src/bsp/mcu/all/bsp_security.c',
  'ra/fsp/src/bsp/cmsis/Device/RENESAS/Source/startup.c',
  'ra/board/' + board + '/board_init.c',
  'ra/fsp/src/bsp/cmsis/Device/RENESAS/Source/system.c',
  'ra/board/' + board + '/board_leds.c',
)

sources_app = files(
  'ra_gen/hal_data.c',
  'src/hal_entry.c',
  'ra_gen/main.c',
)


script_dir = join_paths(meson.current_source_dir(), 'script')

link_args = [
  '-T', join_paths(script_dir, 'fsp.lld'),

  '-Wl,-L' + script_dir,
  '-Wl,-L' + join_paths(meson.current_source_dir(), 'build', 'cmake.arm.debug.R7FA8E1AF'),

  '-Wl,-Map=' + meson.project_name() + '.map',
]

# -------------------------------
# Executable
# -------------------------------


app_exe = executable(
  get_option('example') + '_on_' + mcu + '_' + get_option('board') + '.elf',
  [ sources_app, sources_common ],
  include_directories: inc,
  dependencies: [stdio_to_uart_dep, stdio_to_uart_cfg_dep],
  c_args: ['-fno-threadsafe-statics'],
  link_args: link_args,
)

test_exe = executable(
  meson.project_name() + '_ut_on_' + board + '.elf',
  [sources_common, 'ra/fsp/src/rm_comms_uart/rm_comms_uart.c',],
  include_directories: [inc],
  dependencies: [test_cross_uart_deps, stdio_to_uart_cfg_dep],
  c_args: ['-fno-threadsafe-statics'],
  link_args: link_args,
)


objcopy = find_program('llvm-objcopy')

app_srec = custom_target(
  'app_srec',
  input: app_exe,
  output: app_exe.name() + '.srec',
  command: [objcopy, '-O', 'srec', '@INPUT@', '@OUTPUT@'],
  build_by_default: true,
)

test_srec = custom_target(
  'test_srec',
  input: test_exe,
  output: test_exe.name() + '.srec',
  command: [objcopy, '-O', 'srec', '@INPUT@', '@OUTPUT@'],
  build_by_default: true,
)

# if get_option('easydev') == true
#   run_target(
#       'build-e2studio-project',
#       command : [
#           python,
#           join_paths(
#               meson.project_source_root(),
#               'scripts',
#               'build_e2studio_project.py'
#           ),
#           mcu,
#           get_option('example'),
#       ]
#   )
# endif

# ------------------------------------------------------------
# RFP targets
# ------------------------------------------------------------
enable_rfp = get_option('rfp')

if enable_rfp

  rfp_cli = find_program('rfp-cli', required: true)

  needs_bundle = mcu in ['ra4m2', 'ra6m4', 'ra8d1']
  device = 'ra'

  if needs_bundle
    run_target(
      'rfp_dlm',
      command: [
        rfp_cli,
        '-d', device,
        '-t', 'jlink:RA8E1-FPB-STDIO',
        '-if', 'swd',
        '-dlm', 'SSD',
      ],
    )
  endif

  # Program normal application firmware
  if needs_bundle
    run_target(
      'rfp_program',
      command: [
        rfp_cli,
        '-d', device,
        '-t', 'jlink:RA8E1-FPB-STDIO',
        '-if', 'swd',
        '-fo', 'boundary-file',
        rasc_bundle_app[0],   # .rpd
        '-file', firmware_srec[0],
        '-auto',
      ],
    )

    run_target(
      'rfp_program_ut',
      command: [
        rfp_cli,
        '-d', device,
        '-t', 'jlink:RA8E1-FPB-STDIO',
        '-if', 'swd',
        '-fo', 'boundary-file',
        rasc_bundle_ut[0],    # .rpd
        '-file', firmware_ut_srec[0],
        '-auto',
      ],
    )
  else
    run_target(
      'rfp_program',
      command: [
        rfp_cli,
        '-d', device,
        '-t', 'jlink:RA8E1-FPB-STDIO',
        '-if', 'swd',
        '-file', app_srec[0],
        '-auto',
      ],
    )

    run_target(
      'rfp_program_ut',
      command: [
        rfp_cli,
        '-d', device,
        '-t', 'jlink:RA8E1-FPB-STDIO',
        '-if', 'swd',
        '-file', test_srec[0],
        '-auto',
      ],
    )
  endif

  run_target(
    'rfp_reset',
    command: [
      rfp_cli,
      '-d', device,
      '-t', 'jlink:RA8E1-FPB-STDIO',
      '-if', 'swd',
      '-reset',
    ],
  )

  run_target(
    'rfp_erase_halt',
    command: [
      rfp_cli,
      '-d', device,
      '-t', 'jlink:RA8E1-FPB-STDIO',
      '-if', 'swd',
      '-erase',
      # '-halt',
    ],
  )

  run_target(
    'rfp_check_blank',
    command: [
      rfp_cli,
      '-d', device,
      '-t', 'jlink:RA8E1-FPB-STDIO',
      '-if', 'swd',
      '-blankcheck',
    ],
  )

  run_target(
    'rfp_erase_and_verify',
    command: [
      rfp_cli, '-d', device, '-t', 'jlink:RA8E1-FPB-STDIO', '-if', 'swd', '-erase',
      '&&',
      rfp_cli, '-d', device, '-t', 'jlink:RA8E1-FPB-STDIO', '-if', 'swd', '-blankcheck',
    ],
  )
endif