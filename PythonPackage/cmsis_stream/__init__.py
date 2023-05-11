from importlib import metadata
from importlib.metadata import PackageNotFoundError

try:
    __author__ = metadata.metadata('cmsis_stream')['Author']
    __version__ = metadata.version('cmsis_stream')
except PackageNotFoundError:
    __author__ = "(unknown)"
    __version__ = "(unknown)"
