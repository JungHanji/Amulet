import os
import shutil
import tarfile


class IsAFileError(OSError):
    """Operation doesn't work on files."""


def is_tar_archive(path: str) -> bool:
    return os.path.isfile(path) and tarfile.is_tarfile(path)


ARCHIVE_NAME = "Amulet Release"


def main() -> None:
    # clean directory with unpacked archive
    if os.path.isfile(ARCHIVE_NAME):
        raise IsAFileError(ARCHIVE_NAME)
    if os.path.exists(ARCHIVE_NAME):
        shutil.rmtree(ARCHIVE_NAME)
    os.mkdir(ARCHIVE_NAME)

    # create directories
    for directory in [
        "assets", "logs", "old", "output",
        "source", "src", "src/include", "tools"
    ]:
        os.mkdir(f"{ARCHIVE_NAME}/{directory}")

    # extract archives
    for archive in [
        f"{ARCHIVE_NAME}.tar",
        f"{ARCHIVE_NAME}/Amulet Assets.tar",
        f"{ARCHIVE_NAME}/Amulet Source.tar",
        f"{ARCHIVE_NAME}/Amulet Tools.tar",
        f"{ARCHIVE_NAME}/Amulet.tar"
    ]:
        with tarfile.open(archive) as f:
            f.extractall(ARCHIVE_NAME, filter="tar")

    with tarfile.open(f"{ARCHIVE_NAME}/Amulet Library.tar") as f:
        f.extractall(f"{ARCHIVE_NAME}/src/include", filter="tar")

    # clean
    for archive in tuple(filter(
        lambda x: is_tar_archive(f"{ARCHIVE_NAME}/{x}"),
        os.listdir(ARCHIVE_NAME)
    )):
        os.remove(f"{ARCHIVE_NAME}/{archive}")


if __name__ == "__main__":
    main()
