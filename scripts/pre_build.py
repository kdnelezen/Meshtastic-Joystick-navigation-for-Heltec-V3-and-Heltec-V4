"""
pre_build.py – PlatformIO pre-build script.

Prints the active environment name so that operators can confirm
which board variant is being compiled.
"""

import os
Import("env")  # noqa: F821  (PlatformIO injects this)


def before_build(source, target, env):
    print()
    print("=" * 60)
    print(f"  Building: {env['PIOENV']}")
    print(f"  Board:    {env.BoardConfig().get('name', 'unknown')}")
    print("=" * 60)
    print()


env.AddPreAction("buildprog", before_build)
