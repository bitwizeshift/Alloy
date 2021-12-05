"""
This module contains an enum for ANSI formatting
"""
import enum


class AnsiFormat(enum.Enum):
    """
    An enum for ANSI escape color codes
    """

    BLACK = "\033[30m"
    RED = "\033[31m"
    GREEN = "\033[32m"
    YELLOW = "\033[33m"
    BLUE = "\033[34m"
    MAGENTA = "\033[35m"
    CYAN = "\033[36m"
    WHITE = "\033[37m"
    BRIGHT_BLACK = "\033[30;1m"
    BRIGHT_RED = "\033[31;1m"
    BRIGHT_GREEN = "\033[32;1m"
    BRIGHT_YELLOW = "\033[33;1m"
    BRIGHT_BLUE = "\033[34;1m"
    BRIGHT_MAGENTA = "\033[35;1m"
    BRIGHT_CYAN = "\033[36;1m"
    BRIGHT_WHITE = "\033[37;1m"

    UNDERLINE = "\033[4m"
    BOLD = "\033[1m"

    RESET = "\033[0m"

    def __str__(self):
        return self.value
