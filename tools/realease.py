import os
import tarfile


def main() -> None:
    archives = {
        "Amulet.tar": "output",
        "Amulet Source.tar": "source",
        "Amulet Tools.tar": "tools",
        "Amulet Assets.tar": "assets",
        "Amulet Library.tar": "src/include"
    }

    for output_filename, source_dir in archives.items():
        with tarfile.open(output_filename, "w") as tar:
            tar.add(source_dir)

    with tarfile.open("Amulet Release.tar", "w") as tar:
        for path in archives.keys():
            tar.add(path)
            os.remove(path)


if __name__ == "__main__":
    main()
    