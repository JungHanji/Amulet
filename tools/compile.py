import os
from sys import argv
from typing import NoReturn, Optional


def usage(
        error: Optional[str] = None) -> NoReturn:
    if error:
        print(error)
    print("Usage:")
    print(f"\t{argv[0]} <source file> [compiler]")
    print(f"\t<source file>: path to file in source{os.path.sep}"
          " without extension")
    print(f"\t[compiler]: compiler executable, default g++")
    print("Example:")
    print(f"\t{argv[0]} blockingClient")
    print(f"\t{argv[0]} rawServer clang++")
    exit(1)


def main() -> None:
    # get arguments
    if len(argv) > 3 or len(argv) == 1:
        usage()
    name = argv[1]
    try:
        compiler = argv[2]
    except IndexError:
        compiler = "g++"

    source_file = os.path.join("source", f"{name}.cpp")
    if name == "nt":
        output_file = os.path.join("output", f"{name}.exe")
    else:
        output_file = os.path.join("output", name)

    # check arguments
    if not os.path.exists(source_file):
        raise FileNotFoundError(source_file)
    if os.path.isdir(source_file):
        raise IsADirectoryError(source_file)
    if os.path.isdir(output_file):
        raise IsADirectoryError(output_file)

    # compile
    cmd = f"{compiler} -o {output_file} {source_file} -I ./src/include"
    print(cmd)
    os.system(cmd)


if __name__ == "__main__":
    main()