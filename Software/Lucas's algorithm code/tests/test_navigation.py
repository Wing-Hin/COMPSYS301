"""Compile and run portable navigation and mocked robot integration tests."""
from pathlib import Path
import subprocess
import tempfile

ROOT = Path(__file__).resolve().parents[1]
FIRMWARE = ROOT.parent / 'CS301_Class.cydsn'
TURN = ROOT.parent / "Lucas's turning code"

with tempfile.TemporaryDirectory() as temp:
    common = ['cc', '-std=c99', '-Wall', '-Wextra', '-Werror',
              '-fsanitize=address,undefined', '-g', '-I', str(ROOT)]
    for test in ('test_navigation', 'test_robot_navigation'):
        output = Path(temp) / test
        args = common + [str(ROOT / 'tests' / (test + '.c')),
                         str(ROOT / 'navigation.c'), str(ROOT / 'bfs.c')]
        if test == 'test_robot_navigation':
            args += ['-I', str(ROOT / 'tests/navigation_stubs'),
                     '-I', str(TURN), '-I', str(FIRMWARE),
                     str(FIRMWARE / 'robot_navigation.c')]
        subprocess.run(args + ['-o', str(output)], check=True)
        subprocess.run([str(output)], check=True)

    # Check both main-loop branches against the host PSoC declarations.
    for mode in (0, 1):
        subprocess.run(common + ['-fsyntax-only',
            f'-DROBOT_USE_ROUTE_NAVIGATION={mode}',
            '-I', str(ROOT / 'tests/navigation_stubs'), '-I', str(FIRMWARE),
            str(FIRMWARE / 'main.c'), str(FIRMWARE / 'robot_navigation_config.c')],
            check=True)
    print('PASS: main/config compile checks in route and legacy modes')

    output = Path(temp) / 'turn_abort'
    # Existing turn.c retains an unused EncoderDelta helper. Do not suppress
    # other warnings in the new navigation code.
    subprocess.run(common + ['-Wno-unused-function',
        '-I', str(ROOT / 'tests/navigation_stubs'), '-I', str(TURN),
        '-I', str(FIRMWARE), str(ROOT / 'tests/test_turn_abort.c'),
        str(TURN / 'turn.c'), '-o', str(output)], check=True)
    subprocess.run([str(output)], check=True)

    output = Path(temp) / 'turn_hardware'
    # Legacy motorControl.h defines tentative globals; match old ARM GCC's
    # -fcommon behaviour when compiling these existing files on a modern host.
    subprocess.run(common + ['-fcommon', '-I', str(ROOT / 'tests/navigation_stubs'),
        '-I', str(TURN), '-I', str(FIRMWARE),
        str(TURN / 'tests/test_turn_hardware.c'), str(TURN / 'turn_hardware.c'),
        str(ROOT / 'tests/turn_hardware_support.c'), '-o', str(output)], check=True)
    subprocess.run([str(output)], check=True)
