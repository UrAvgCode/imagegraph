from pathlib import Path

import matplotlib.pyplot as plt
import pandas as pd
from matplotlib.ticker import FuncFormatter


# ChatGPT (GPT-5.5):
# Unterstützung bei der Erstellung und Anpassung der Diagramme mit Matplotlib.

def load_csv(csv):
    path = Path(csv)

    if not path.is_file() or path.stat().st_size == 0:
        return None

    return pd.read_csv(path)


def create_plot(csv, title):
    df = load_csv(csv)
    if df is None:
        return

    plt.figure(figsize=(10, 6))

    x = range(len(df))
    plt.plot(x, df["gpu"] / 1e6, marker="o", linewidth=2, label="GPU")
    plt.plot(x, df["cpu"] / 1e6, marker="o", linewidth=2, label="CPU")
    plt.plot(x, df["cpu_parallel"] / 1e6, marker="o", linewidth=2, label="CPU Parallel")

    labels = [f"{s}×{s}" for s in df["size"]]
    plt.xticks(x, labels, rotation=45)

    plt.yscale("log")
    plt.gca().yaxis.set_major_formatter(
        FuncFormatter(lambda y, _: f"{y:g} ms")
    )

    plt.xlabel("Image Resolution")
    plt.ylabel("Execution Time (ms)")
    plt.title(title)
    plt.grid(True, which="both", alpha=0.3)
    plt.legend()

    plt.tight_layout()
    plt.savefig(f"{title}.pdf", bbox_inches="tight")
    plt.show()


def create_model_plot(csv, title):
    df = load_csv(csv)
    if df is None:
        return

    plt.figure(figsize=(10, 6))

    x = range(len(df))
    plt.plot(x, df["gpu"] / 1e6, marker="o", linewidth=2, label="GPU")
    plt.plot(x, df["cpu"] / 1e6, marker="o", linewidth=2, label="CPU")

    labels = [f"{s}×{s}" for s in df["size"]]
    plt.xticks(x, labels, rotation=45)

    plt.yscale("log")
    plt.gca().yaxis.set_major_formatter(
        FuncFormatter(lambda y, _: f"{y:g} ms")
    )

    plt.xlabel("Image Resolution")
    plt.ylabel("Execution Time (ms)")
    plt.title(title)
    plt.grid(True, which="both", alpha=0.3)
    plt.legend()

    plt.tight_layout()
    plt.savefig(f"{title}.pdf", bbox_inches="tight")
    plt.show()


def create_bar_chart(csv, title):
    df = load_csv(csv)
    if df is None:
        return

    row = df.iloc[0]

    gpu_encoder = row["gpu_encoder"] / 1e6
    cpu_encoder = row["cpu_encoder"] / 1e6
    gpu_decoder = row["gpu_decoder"] / 1e6
    cpu_decoder = row["cpu_decoder"] / 1e6

    width = 0.35

    fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(8, 5))

    ax1.bar(-width / 2, gpu_encoder, width, label="GPU")
    ax1.bar(width / 2, cpu_encoder, width, label="CPU")
    ax1.set_xticks([-width / 2, width / 2])
    ax1.set_xticklabels(["GPU", "CPU"])
    ax1.ticklabel_format(style="plain", axis="y")
    ax1.set_title("Encoder")
    ax1.set_ylabel("Execution Time (ms)")
    ax1.legend()

    ax2.bar(-width / 2, gpu_decoder, width, label="GPU")
    ax2.bar(width / 2, cpu_decoder, width, label="CPU")
    ax2.set_xticks([-width / 2, width / 2])
    ax2.set_xticklabels(["GPU", "CPU"])
    ax2.ticklabel_format(style="plain", axis="y")
    ax2.set_title("Decoder")
    ax2.set_ylabel("Execution Time (ms)")
    ax2.legend()

    fig.suptitle(title)

    plt.tight_layout()
    plt.savefig(f"{title}.pdf", bbox_inches="tight")
    plt.show()


if __name__ == '__main__':
    create_bar_chart("segment_model/results.csv", "Segment Anything V2")
    create_model_plot("depth_model/results.csv", "Depth Anything V2")
    create_plot("histogram_equalization/results.csv", "Histogram Equalization")
    create_plot("grayscale/results.csv", "Grayscale")
    create_plot("blur/results.csv", "Blur")
