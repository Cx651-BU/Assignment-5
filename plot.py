#!/usr/bin/env python3

import argparse
import re
from collections import defaultdict

import matplotlib.pyplot as plt

def parse_smem_line(line):
    parts = line.split()

    if len(parts) < 7:
        return None

    try:
        pid = int(parts[0])
        swap = int(parts[-4])
        uss = int(parts[-3])
        pss = int(parts[-2])
        rss = int(parts[-1])
    except ValueError:
        return None

    user = parts[1]
    command = " ".join(parts[2:-4])

    if not command.startswith("./cli"):
        return None

    return {
        "pid": pid,
        "user": user,
        "command": command,
        "swap": swap,
        "uss": uss,
        "pss": pss,
        "rss": rss,
    }

def parse_file(filename):
    snapshots = []
    current = None

    with open(filename, "r") as f:
        for line in f:

            if line.startswith("%CPU"):
                if current is not None:
                    snapshots.append(current)

                current = {}
                continue

            if current is None:
                continue

            data = parse_smem_line(line)

            if data is not None:
                current[data["pid"]] = data

    if current is not None:
        snapshots.append(current)

    return snapshots


def classify_command(command):
    """
    Determine whether a command is an mmap or normal run.

    Adjust these strings if your command names differ.
    """
    if "mmap" in command:
        return "mmap"

    return "normal"


def collect_processes(snapshots):
    """
    Convert snapshots into per-PID time series.

    Returns:

        {
            pid: {
                "mode": "mmap" or "normal",
                "command": "...",
                "rss": [...],
                "pss": [...],
                "uss": [...]
            }
        }
    """

    processes = {}

    for t, snapshot in enumerate(snapshots):

        for pid, data in snapshot.items():

            if pid not in processes:
                processes[pid] = {
                    "mode": classify_command(data["command"]),
                    "command": data["command"],
                    "rss": [None] * len(snapshots),
                    "pss": [None] * len(snapshots),
                    "uss": [None] * len(snapshots),
                }

            processes[pid]["rss"][t] = data["rss"]
            processes[pid]["pss"][t] = data["pss"]
            processes[pid]["uss"][t] = data["uss"]

    return processes


def plot_processes(mmap_processes, normal_processes, interval):
    """
    Plot RSS/PSS/USS for every CLI instance.
    """

    fig, ax = plt.subplots(figsize=(12, 7))

    # Use different shades for different instances while preserving
    # the mmap vs normal distinction.
    mmap_colors = plt.cm.Blues
    normal_colors = plt.cm.Oranges

    def plot_group(processes, cmap, label_prefix):
        # Sort by PID so plots are deterministic.
        items = sorted(processes.items())

        n = max(len(items), 1)

        for i, (pid, process) in enumerate(items):

            # Avoid colors that are too light.
            shade = 0.45 + 0.45 * (i / max(n - 1, 1))
            color = cmap(shade)

            x = [
                j * interval
                for j in range(len(process["rss"]))
            ]

            # RSS = solid
            ax.plot(
                x,
                process["rss"],
                color=color,
                linestyle="-",
                linewidth=1.8,
                label=f"{label_prefix} PID {pid} RSS",
            )

            # PSS = dashed
            ax.plot(
                x,
                process["pss"],
                color=color,
                linestyle="--",
                linewidth=1.5,
                label=f"{label_prefix} PID {pid} PSS",
            )

            # USS = dotted
            ax.plot(
                x,
                process["uss"],
                color=color,
                linestyle=":",
                linewidth=1.5,
                label=f"{label_prefix} PID {pid} USS",
            )

    plot_group(mmap_processes, mmap_colors, "mmap")
    plot_group(normal_processes, normal_colors, "normal")

    ax.set_xlabel("Time (seconds)")
    ax.set_ylabel("Memory (KB)")
    ax.set_title("Memory Usage: mmap vs. normal file reads")

    ax.grid(True, alpha=0.3)

    # Put the legend outside the graph because there can be many lines.
    ax.legend(
        bbox_to_anchor=(1.02, 1),
        loc="upper left",
        fontsize=8,
    )

    plt.tight_layout()
    plt.show()


def main():
    parser = argparse.ArgumentParser(
        description="Plot RSS/PSS/USS for mmap vs normal CLI runs."
    )

    parser.add_argument(
        "mmap_file",
        help="Output file containing mmap runs",
    )

    parser.add_argument(
        "normal_file",
        help="Output file containing normal/read runs",
    )

    parser.add_argument(
        "--interval",
        type=float,
        default=0.2,
        help="Time between snapshots in seconds (default: 0.2)",
    )

    parser.add_argument(
        "--save",
        help="Save plot to this file instead of displaying it",
    )

    args = parser.parse_args()

    mmap_snapshots = parse_file(args.mmap_file)
    normal_snapshots = parse_file(args.normal_file)

    print(f"mmap snapshots:   {len(mmap_snapshots)}")
    print(f"normal snapshots: {len(normal_snapshots)}")

    mmap_processes = collect_processes(mmap_snapshots)
    normal_processes = collect_processes(normal_snapshots)

    print(f"mmap processes:   {len(mmap_processes)}")
    print(f"normal processes: {len(normal_processes)}")

    if not mmap_processes:
        print("WARNING: no ./cli mmap processes found")

    if not normal_processes:
        print("WARNING: no ./cli normal processes found")

    plot_processes(
        mmap_processes,
        normal_processes,
        args.interval,
    )

    if args.save:
        plt.savefig(args.save, dpi=200, bbox_inches="tight")


if __name__ == "__main__":
    main()