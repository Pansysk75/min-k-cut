# Takes a benchmark executable and 1. a list of input files or 2. a directory of input files
# Runs the benchmark on the input files, keeps only the json output, and saves it to a dataframe for analysis

import os
import sys
import re
import subprocess
import pandas as pd
import json
import argparse
import matplotlib.pyplot as plt
import matplotlib
import seaborn as sns

def run_benchmark(benchmark, input_files):
    results = []
    for input_file in input_files:
        print(f"Running {benchmark} on {input_file}")
        output = subprocess.run([benchmark, input_file], stdout=subprocess.PIPE).stdout.decode('utf-8')
        try:
            # line that contains the json output
            json_line = [line for line in output.split('\n') if line.startswith('{')][0]
            results.append(json.loads(json_line))
        except:
            print(f"Error parsing json output for {input_file}")
    return results

def main():
    parser = argparse.ArgumentParser(description='Run a benchmark on a list or directory of input files')
    parser.add_argument('benchmark', type=str, help='Path to the benchmark executable')
    parser.add_argument('-i', '--input', type=str, help='A list of input files or a directory of input files')
    parser.add_argument('-o', '--output', default="out.csv", type=str, help='Path to save the output dataframe')
    args = parser.parse_args()

    df = pd.DataFrame()
    # If input is a csv file, don't run the benchmark, just use results in that file for analysis
    if args.input.endswith('.csv'):
        df = pd.read_csv(args.input)
    else:
        if os.path.isfile(args.input):
            results = run_benchmark(args.benchmark, [args.input])
        elif os.path.isdir(args.input):
            input_files = [os.path.join(args.input, f) for f in os.listdir(args.input)]
        else:
            print("Invalid input file or directory")
            sys.exit(1)

        results = run_benchmark(args.benchmark, input_files)
        df = pd.DataFrame(results)
        df.to_csv(args.output, index=False)

    print(df)
    # df.to_csv(args.output, index=False)

    # Plot: 
    # n_nodes vs gh_time_total and n_nodes vs gh_time_min_cut (same axis)
    # n_nodes vs min_k_cut_value_time
    # n_nodes vs min_k_cut_map_time_total

    df["gh_avg_time_min_cut"] = df["gh_time_min_cut"] / df["n_nodes"]

    plot_dir = 'plots'
    if not os.path.exists(plot_dir):
        os.makedirs(plot_dir)
    os.chdir(plot_dir)

    matplotlib.rcParams['figure.dpi'] = 500
    figsize = (5, 3)
    sns.set_theme(style="whitegrid")

    fig, ax = plt.subplots(1, 1, figsize=figsize)
    sns.lineplot(x='n_nodes', y='gh_avg_time_min_cut', data=df, ax=ax, marker='o')
    ax.set_title('Time for single Minimum s-t Cut')
    ax.set_xlabel('n nodes')
    ax.set_ylabel('time (s)')
    plt.tight_layout()
    plt.savefig('gh_avg_time_min_cut.png')

    fig, ax = plt.subplots(1, 1, figsize=figsize)
    sns.lineplot(x='n_nodes', y='gh_time_total', data=df, ax=ax, marker='o', label='Total algorithm time')
    sns.lineplot(x='n_nodes', y='gh_time_min_cut', data=df, ax=ax, marker='.',
                  label='Time spent on Minimum s-t Cuts')
    ax.set_title('Gomory-Hu construction time')
    ax.set_xlabel('n nodes')
    ax.set_ylabel('time (s)')
    ax.legend()
    plt.tight_layout()
    plt.savefig('gh_time.png')

    fig, ax = plt.subplots(1, 1, figsize=figsize)
    sns.lineplot(x='n_nodes', y='min_k_cut_value_time', data=df, ax=ax, marker='o')
    ax.set_title('Time for computing Minimum k-cut value on Gomory-Hu tree')
    ax.set_xlabel('n nodes')
    ax.set_ylabel('time (s)')
    plt.tight_layout()
    plt.savefig('min_k_cut_value_time.png')

    fig, ax = plt.subplots(1, 1, figsize=figsize)
    sns.lineplot(x='n_nodes', y='min_k_cut_map_time_total', data=df, ax=ax, marker='o')
    ax.set_title('Time for computing Minimum k-cut map on Gomory-Hu tree')
    ax.set_xlabel('n nodes')
    ax.set_ylabel('time (s)')
    plt.tight_layout()
    plt.savefig('min_k_cut_map_time_total.png')

if __name__ == '__main__':
    main()