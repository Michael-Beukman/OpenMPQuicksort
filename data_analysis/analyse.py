import matplotlib.pyplot as plt
import pandas as pd
import numpy as np

def get_df(fname):
    df = pd.read_csv(fname)
    dic = [k.strip() for k in df.keys()]
    df.columns = dic
    df.keys()
    keys = df.select_dtypes(include='object').keys()
    for key in keys:
        df[key] = df[key].map(lambda s: s.strip())
    return df
    
def plot_results():
    plt.switch_backend("Agg")
    df = get_df('../data/2021-05-10-18-51-07__.csv')
    names = df['Name'].unique()
    for k in ["Time (s)", 'Speedup over sequential']:
        for n in names:
            tmp = df[df['Name'] == n]
            ins = tmp['N']
            outs = tmp[k]
            plt.plot(ins, outs, label=n)
            plt.title(f"{k} vs array size")
            plt.ylabel(k)
            plt.xlabel("array size")
            plt.legend()
        plt.savefig(f"plots/plot_{k.replace(' ', '_')}.png")
        plt.close()
plot_results()