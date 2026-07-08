import matplotlib.pyplot as plt
import pandas as pd
from matplotlib.ticker import FuncFormatter

def create_plot(csv, title):
    df = pd.read_csv(csv)

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
    df = pd.read_csv(csv)

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

if __name__ == '__main__':
    create_model_plot("depth_model/results.csv", "Depth Anything V2")
    create_plot("grayscale/results.csv", "Grayscale")
    create_plot("blur/results.csv", "Blur")
