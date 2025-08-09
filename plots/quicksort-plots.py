import argparse
import pandas as pd
import matplotlib.pyplot as plt


def plot_benchmarks(csv_path):
    df = pd.read_csv(csv_path)

    required_columns = {"num_threads", "min_size", "n_elements", "time_sec", "max_queue_length"}
    if not required_columns.issubset(df.columns):
        raise ValueError(f"CSV file missing required columns: {required_columns - set(df.columns)}")

    # График 1: время сортировки от количества потоков
    plt.figure()
    for min_size in sorted(df['min_size'].unique()):
        subset = df[df['min_size'] == min_size]
        plt.plot(subset['num_threads'], subset['time_sec'], marker='o', label=f"min_size={min_size}")

    plt.title(r"Время сортировки от количества потоков, $n = 2 \cdot 10^9$")
    plt.xlabel("Количество потоков")
    plt.ylabel("Время (секунды)")
    plt.legend()
    plt.grid(True)

    # График 2: время сортировки от min_size
    plt.figure()
    for num_threads in sorted(df['num_threads'].unique()):
        subset = df[df['num_threads'] == num_threads]
        plt.plot(subset['min_size'], subset['time_sec'], marker='o', label=f"threads={num_threads}")

    plt.title(r"Время сортировки от min_size, $n = 2 \cdot 10^9$")
    plt.xlabel("min_size")
    plt.ylabel("Время (секунды)")
    plt.legend()
    plt.grid(True)

    # График 3: максимальная загруженность очереди от количества потоков
    plt.figure()
    for min_size in sorted(df['min_size'].unique()):
        subset = df[df['min_size'] == min_size]
        plt.plot(subset['num_threads'], subset['max_queue_length'], marker='o', label=f"min_size={min_size}")

    plt.title(r"Макс. длина очереди от количества потоков, $n = 2 \cdot 10^9$")
    plt.xlabel("Количество потоков")
    plt.ylabel("Макс. длина очереди")
    plt.legend()
    plt.grid(True)

    plt.show()


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Plot benchmark results from CSV file")
    parser.add_argument("csv_file", help="Path to benchmark_results.csv")

    args = parser.parse_args()
    plot_benchmarks(args.csv_file)
