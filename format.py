#!/usr/bin/env python3
import errno
import os
import pathlib
import subprocess
import sys


def collect_files(roots, ext):
    result = []
    for root in roots:
        for (directory, _, files) in os.walk(root):
            for file in files:
                path = pathlib.Path(os.path.join(directory, file))
                if path.suffix in ext:
                    result.append(path)

    return result


def clang_format(file):
    format_base = ["clang-format", "-i"]
    command = format_base + [f]
    print("Formatting", f)
    subprocess.run(command)


if __name__ == "__main__":
    extensions = [".cpp", ".h"]

    try:
        if len(sys.argv) > 1:
            for f in sys.argv[1:]:
                clang_format(f)
        else:
            for f in collect_files(["src"], extensions):
                clang_format(f)
    except OSError as e:
        if e.errno == errno.ENOENT:
            print("Error: clang-format is not installed!")
            print("Install from package manager")
            print("Download from https://llvm.org/builds/ on Windows")

    print("Formatting CMakeLists.txt")
    try:
        subprocess.run(
            [
                "cmake-format",
                "-i",
                "--line-width",
                "100",
                "--tab-size",
                "4",
                "CMakeLists.txt",
            ]
        )
    except OSError as e:
        if e.errno == errno.ENOENT:
            print("Error: cmake-format is not installed!")
            print("Install with: pip install cmake-format")
