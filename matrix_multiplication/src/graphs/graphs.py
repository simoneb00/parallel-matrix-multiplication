import matplotlib.pyplot as plt
import pandas as pd

def create_graphs_openmp():
    # Leggere i dati da CSV
    df = pd.read_csv('openmp_performance.csv')

    # Estrarre i valori per m, n, k e le tre linee richieste
    m_values = df['m']
    n_values = df['n']
    k_values = df['k']
    gflops_values = df['GFlops']
    max_rel_diff_values = df['max_rel_diff']
    speedup_values = df['speedup']

    # Creare il grafico
    fig, ax1 = plt.subplots()

    color = 'tab:red'
    ax1.set_xlabel('m, n, k')
    ax1.set_ylabel('GFlops', color=color)
    ax1.plot(m_values, gflops_values, color=color, marker='o', label='GFlops')
    ax1.tick_params(axis='y', labelcolor=color)

    ax2 = ax1.twinx()
    color = 'tab:blue'
    ax2.set_ylabel('max_rel_diff', color=color)
    ax2.plot(m_values, max_rel_diff_values, color=color, marker='s', label='Max Rel Diff')
    ax2.tick_params(axis='y', labelcolor=color)

    ax3 = ax1.twinx()
    ax3.spines['right'].set_position(('outward', 60))
    color = 'tab:green'
    ax3.set_ylabel('Speedup', color=color)
    ax3.plot(m_values, speedup_values, color=color, marker='^', label='Speedup')
    ax3.tick_params(axis='y', labelcolor=color)

    fig.tight_layout()
    plt.title('Performance Metrics vs. m, n, k')
    plt.show()


if __name__ == "__main__":
    create_graphs_openmp()