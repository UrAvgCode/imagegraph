import matplotlib.pyplot as plt
import pandas as pd

if __name__ == '__main__':
    df = pd.read_csv("grayscale/results.csv")

    plt.figure(figsize=(10, 6))

    plt.plot(df["size"], df["gpu"], marker="o", label="GPU")
    plt.plot(df["size"], df["cpu"], marker="o", label="CPU")
    plt.plot(df["size"], df["cpu_parallel"], marker="o", label="CPU Parallel")

    plt.xscale("log", base=2)
    plt.yscale("log")

    plt.xlabel("Image Size")
    plt.ylabel("Time (ns)")
    plt.title("Grayscale Benchmark")
    plt.grid(True, which="both", alpha=0.3)
    plt.legend()

    plt.tight_layout()
    plt.savefig("grayscale/benchmark.png", dpi=300)
    plt.show()
