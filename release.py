# workflow for helping release a version
# of the random key generator

# requirements for compiling and building releases include
#
# meson
# py7zr (for 7zip distributions)
# typer (command setup)
#
from __future__ import annotations

import os
import platform
import shutil
import subprocess
import sys
import tarfile
from pathlib import Path
from typing import Annotated, Sequence

import py7zr
import typer

SOURCES = list(Path("src").iterdir())
SOURCES += [
    Path("meson.build"),
    Path("README.md")
]

class CommandError(Exception):
    """Exception thrown when handling a command"""

is_windows = sys.platform in ["win32", "cygwin" , "cli"]

def write_machine_name():
    return platform.system().lower() + "-" + platform.machine().lower()


def run_command(command: Sequence[str], read: bool = False):
    process = subprocess.run(command, shell=True, capture_output=read, check=True)
    if read:
        return process.stdout.decode("utf-8")

def get_rkg_version():
    """Obtains the version of the compiled random key generator for creating the release name"""
    if is_windows:
        return run_command(["rkg", "--version"], read=True).strip()
    else:
        return run_command(["bin", "rkg", "--version"], read=True).strip()

def add_file(t:tarfile.TarFile, name:str):
    t.add(name, name)

def write_tar_release(release_name:str, to_releases_folder:Path, ext:str = 'gz'):
    release = to_releases_folder / release_name
    with tarfile.open(str(release) + ".tar.gz", f"w:{ext}") as f:
        f.add('src', 'src')
        if is_windows:
            add_file(f, 'rkg.exe')
            add_file(f, 'rkg.pdb')
        else:
            add_file(f, 'rkg')
        add_file(f, 'meson.build')
        add_file(f, 'README.md')




def write_releases(to_releases_folder:Path | None = None):
    release_path = to_releases_folder or Path(__file__).parent
    machine = 'rkg' + '-' + write_machine_name() + "-" + get_rkg_version()
    write_tar_release(machine, release_path, 'gz')


    


def main(
    build_meson_dir: Annotated[
        bool,
        typer.Option(
            help="Builds Meson build directory for you for compiling, default assumes you've done so already with a directory named \"build\""
        ),
    ] = False,
    release_folder:Annotated[
        Path | None,
        typer.Option(
            help="Folder for building different releases to, in normal circumstances such as a workflow this isn't used."
        )
    ] = None
):
    if build_meson_dir or not os.path.exists("build"):
        print("building meson directory")
        run_command(["meson", "setup", "build"])

    # Compile the build
    if sys.platform in ["win32", "cygwin", "cli"]:
        run_command(["cd", "build", "&&", "meson", "compile"])
        
        # Move compiled files to the directory we ran our command from...
        shutil.move("build/rkg.exe", "rkg.exe")
        # Get pdb for someone that wants to play around with this script 
        # in ghidra incase the code needs recovering...
        shutil.move("build/rkg.pdb", "rkg.pdb")


    else:
        run_command(["cd", "build", ";", "meson", "compile"])
        shutil.move("build/rkg", "rkg")
    
    write_releases(release_folder)


if __name__ == "__main__":
    typer.run(main)
    # print()
