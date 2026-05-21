import pandas as pd
import plotly.graph_objects as go
from plotly.subplots import make_subplots
import os

# === Caricamento dati ===
csv_seriale = 'log.csv'
csv_parallelo = 'log_parallel.csv'
csv_mpi = 'log_mpi.csv'

dfs = []

if os.path.exists(csv_seriale):
    df_seriale = pd.read_csv(csv_seriale, header=None, names=["pattern", "match_count", "time", "file"])
    df_seriale["tipo"] = "Seriale"
    dfs.append(df_seriale)

if os.path.exists(csv_parallelo):
    df_parallelo = pd.read_csv(csv_parallelo, header=None, names=["pattern", "match_count", "time", "file"])
    df_parallelo["tipo"] = "Parallelo (OpenMP)"
    dfs.append(df_parallelo)

if os.path.exists(csv_mpi):
    df_mpi = pd.read_csv(csv_mpi, header=None, names=["pattern", "match_count", "time", "file"])
    df_mpi["tipo"] = "Parallelo (MPI)"
    dfs.append(df_mpi)

if not dfs:
    print("Nessun file CSV trovato.")
    exit()

df = pd.concat(dfs)
df = df.sort_values(by=["pattern", "tipo"])

pivot = df.pivot_table(index="pattern", columns="tipo", values="time", aggfunc="mean")
# Calcolo speed-up (se applicabile, rispetto a "Seriale")
speedup_omp = None
speedup_mpi = None

if "Seriale" in pivot:
    if "Parallelo (OpenMP)" in pivot:
        speedup_omp = pivot["Seriale"] / pivot["Parallelo (OpenMP)"]
    if "Parallelo (MPI)" in pivot:
        speedup_mpi = pivot["Seriale"] / pivot["Parallelo (MPI)"]

# === Setup figure con due sottografici ===
fig = make_subplots(
    rows=1, cols=2,
    subplot_titles=("Tempi di esecuzione", "Speed-up (rispetto a Seriale)"),
    shared_xaxes=False
)

# === Grafico 1: Tempi ===
if "Seriale" in pivot:
    fig.add_trace(
        go.Bar(x=pivot.index, y=pivot["Seriale"], name="Seriale", marker_color="#636EFA", text=pivot["Seriale"].round(4), textposition='auto'),
        row=1, col=1
    )
if "Parallelo (OpenMP)" in pivot:
    fig.add_trace(
        go.Bar(x=pivot.index, y=pivot["Parallelo (OpenMP)"], name="Parallelo (OpenMP)", marker_color="#EF553B", text=pivot["Parallelo (OpenMP)"].round(4), textposition='auto'),
        row=1, col=1
    )
if "Parallelo (MPI)" in pivot:
    fig.add_trace(
        go.Bar(x=pivot.index, y=pivot["Parallelo (MPI)"], name="Parallelo (MPI)", marker_color="#FFA15A", text=pivot["Parallelo (MPI)"].round(4), textposition='auto'),
        row=1, col=1
    )

# === Grafico 2: Speed-up ===
if speedup_omp is not None:
    fig.add_trace(
        go.Bar(x=pivot.index, y=speedup_omp, name="Speed-up (OpenMP)", marker_color="#00CC96", text=speedup_omp.round(2).astype(str) + "x", textposition='auto'),
        row=1, col=2
    )
if speedup_mpi is not None:
    fig.add_trace(
        go.Bar(x=pivot.index, y=speedup_mpi, name="Speed-up (MPI)", marker_color="#AB63FA", text=speedup_mpi.round(2).astype(str) + "x", textposition='auto'),
        row=1, col=2
    )

# === Linea baseline x1 ===
if speedup_omp is not None or speedup_mpi is not None:
    fig.add_shape(
        type="line", x0=-0.5, x1=len(pivot.index)-0.5, y0=1, y1=1,
        line=dict(color="red", dash="dash"), row=1, col=2
    )

# === Layout globale ===
fig.update_layout(
    title="📊 Serial vs Parallel",
    title_font_size=22,
    barmode='group',
    bargap=0.15,
    plot_bgcolor="#FAFAFA",
    paper_bgcolor="#FFFFFF",
    legend=dict(x=0.5, xanchor="center", orientation="h", y=-0.2),
    height=600,
    margin=dict(t=80, b=100),
)

fig.update_xaxes(tickangle=45, row=1, col=1)
fig.update_xaxes(tickangle=45, row=1, col=2)

# === Mostra ===
fig.show()
