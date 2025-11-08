import random
import os

def random_timestamp():
    hour = random.randint(0, 23)
    minute = random.randint(0, 59)
    second = random.randint(0, 59)
    day = random.randint(1, 31)
    month = random.randint(1, 12)
    return f"{hour:02}:{minute:02}:{second:02}:{day:02}:{month:02}"

def random_category():
    return random.choice(["TCP", "UDP", "System", "Clock", "Alarm", "Debug", "Kernel", "Auth"])

def random_message():
    verbs = ["sent", "received", "failed", "started", "stopped", "updated", "triggered", "checked"]
    nouns = ["packet", "process", "task", "signal", "event", "module", "connection", "alarm"]
    hex_id = hex(random.randint(0x100000, 0xFFFFFF))[2:]
    return f"{random.choice(nouns).capitalize()} 0x{hex_id} {random.choice(verbs)}"

def generate_logs(filename, max_lines=20):
    """Generate <=20 lines of logs"""
    count = random.randint(8, 15)  # few lines but diverse
    with open(filename, "w", encoding="utf-8") as f:
        for _ in range(count):
            line = f"{random_timestamp()}|{random_category()}|{random_message()}\n"
            f.write(line)
    print(f"{filename}: {count} log entries generated")

def random_timestamp_range():
    t1 = random_timestamp()
    t2 = random_timestamp()
    if t1 > t2:
        t1, t2 = t2, t1
    return f"{t1}|{t2}"

def random_keywords():
    words = ["load", "print", "state", "valid", "breakdown", "reset", "client", "server", "debug"]
    num = random.randint(1, 3)
    return " ".join(random.sample(words, num))

def generate_cmds(filename, log_count):
    """Generate <=20 lines of commands, ending with q"""
    cmd_patterns = {
        "t": lambda: random_timestamp_range(),
        "m": lambda: random_timestamp(),
        "c": lambda: random_category(),
        "k": lambda: random_keywords(),
        "a": lambda: str(random.randint(0, log_count - 1)),
        "r": lambda: "",
        "d": lambda: str(random.randint(0, 5)),
        "b": lambda: str(random.randint(0, 5)),
        "e": lambda: str(random.randint(0, 5)),
        "s": lambda: "",
        "l": lambda: "",
        "g": lambda: "",
        "p": lambda: "",
        "#": lambda: "small test comment",
    }

    max_lines = 20
    cmds = []
    possible = list(cmd_patterns.keys())

    # ensure we always have at least one search and one r/s/p
    cmds.append(f"{random.choice(['t','m','c','k'])} {cmd_patterns[random.choice(['t','m','c','k'])]()}")
    cmds.append(" r")

    # fill remaining lines randomly (reserve 1 line for q)
    while len(cmds) < max_lines - 1:
        cmd = random.choice(possible)
        arg = cmd_patterns[cmd]()
        if cmd == "#":
            cmds.append(f"# {arg}")
        elif arg:
            cmds.append(f"{cmd} {arg}")
        else:
            cmds.append(f"{cmd}")

    # always end with quit
    cmds[-1] = "q"

    with open(filename, "w", encoding="utf-8") as f:
        f.write("\n".join(cmds[:max_lines]) + "\n")
    print(f"{filename}: {len(cmds[:max_lines])} commands (ends with  q)")

def main():
    import argparse
    parser = argparse.ArgumentParser(description="Generate small (<20 line) test files")
    parser.add_argument("n", type=int, help="test number (1–15)")
    args = parser.parse_args()

    if not (1 <= args.n <= 15):
        raise ValueError("Test number must be 1–15")

    log_filename = f"test-{args.n}-log.txt"
    cmd_filename = f"test-{args.n}-cmds.txt"

    generate_logs(log_filename)
    log_count = sum(1 for _ in open(log_filename, encoding="utf-8"))
    generate_cmds(cmd_filename, log_count)

if __name__ == "__main__":
    main()
