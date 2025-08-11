import argparse
import os
import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns

def plot_bucket_sort_benchmarks(df, save_dir=None):
    unique_intervals = sorted(df['interval'].unique())
    unique_nbuckets = sorted(df['n_bucket'].unique())

    for fixed_interval in unique_intervals:
        # График 1: Время сортировки от количества потоков для каждого n_bucket (фиксированный interval)
        plt.figure()
        for n_bucket in unique_nbuckets:
            subset = df[(df['n_bucket'] == n_bucket) & (df['interval'] == fixed_interval)]
            if subset.empty:
                continue
            plt.plot(subset['num_threads'], subset['time_sec'], marker='o', label=f"n_bucket={n_bucket}")
        plt.title(f"Время сортировки от количества потоков, interval={fixed_interval}, n={df['n_elements'].iloc[0]}")
        plt.xlabel("Количество потоков")
        plt.ylabel("Время (секунды)")
        plt.legend()
        plt.grid(True)

        if save_dir:
            filename = f"time_vs_threads_interval_{fixed_interval}.png"
            plt.savefig(os.path.join(save_dir, filename))
        else:
            plt.show()
        plt.close()

        # График 3: Время сортировки от количества бакетов для каждого количества потоков (фиксированный interval)
        plt.figure()
        for num_threads in sorted(df['num_threads'].unique()):
            subset = df[(df['num_threads'] == num_threads) & (df['interval'] == fixed_interval)]
            if subset.empty:
                continue
            plt.plot(subset['n_bucket'], subset['time_sec'], marker='o', label=f"threads={num_threads}")
        plt.title(f"Время сортировки от количества бакетов, interval={fixed_interval}, n={df['n_elements'].iloc[0]}")
        plt.xlabel("Количество бакетов (n_bucket)")
        plt.ylabel("Время (секунды)")
        plt.legend()
        plt.grid(True)

        if save_dir:
            filename = f"time_vs_nbuckets_interval_{fixed_interval}.png"
            plt.savefig(os.path.join(save_dir, filename))
        else:
            plt.show()
        plt.close()

    # График 2: Время сортировки от интервала для каждого количества потоков и каждого n_bucket
    for fixed_n_bucket in unique_nbuckets:
        plt.figure()
        for num_threads in sorted(df['num_threads'].unique()):
            subset = df[(df['num_threads'] == num_threads) & (df['n_bucket'] == fixed_n_bucket)]
            if subset.empty:
                continue
            plt.plot(subset['interval'], subset['time_sec'], marker='o', label=f"threads={num_threads}")
        plt.title(f"Время сортировки от interval, n_bucket={fixed_n_bucket}, n={df['n_elements'].iloc[0]}")
        plt.xlabel("Interval")
        plt.ylabel("Время (секунды)")
        plt.legend()
        plt.grid(True)

        if save_dir:
            filename = f"time_vs_interval_nbuckets_{fixed_n_bucket}.png"
            plt.savefig(os.path.join(save_dir, filename))
            plt.close()
        else:
            plt.show()

def plot_bucket_sort_pairplot(df, save_dir=None):
    # Можно исключить n_elements, если оно константно
    plot_df = df[['num_threads', 'n_bucket', 'interval', 'time_sec']]

    g = sns.pairplot(plot_df, diag_kind='kde', markers='o', plot_kws={'alpha':0.6, 's':40})
    plt.suptitle(f"Pairplot параметров сортировки, n={df['n_elements'].iloc[0]}", y=1.02)

    if save_dir:
        filename = "pairplot_bucket_sort.png"
        plt.savefig(os.path.join(save_dir, filename))
        plt.close()
    else:
        plt.show()

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Plot bucket sort benchmark results")
    parser.add_argument("csv_file", help="Path to bucket_sort benchmark CSV file")
    parser.add_argument("--save_dir", default=None, help="Directory to save plots (if not specified, plots are shown interactively)")

    args = parser.parse_args()

    df = pd.read_csv(args.csv_file)

    required_columns = {"num_threads", "n_bucket", "interval", "n_elements", "time_sec"}
    missing = required_columns - set(df.columns)
    if missing:
        raise ValueError(f"CSV missing columns: {missing}")

    if args.save_dir:
        os.makedirs(args.save_dir, exist_ok=True)

    plot_bucket_sort_benchmarks(df, save_dir=args.save_dir)
    plot_bucket_sort_pairplot(df, save_dir=args.save_dir)
