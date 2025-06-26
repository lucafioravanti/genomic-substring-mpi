import pandas as pd
import plotly.graph_objects as go
from plotly.subplots import make_subplots

# === Caricamento dati ===
csv_seriale = 'C:/Users/lucav/Desktop/ACA-main/log.csv'
csv_parallelo = 'C:/Users/lucav/Desktop/ACA-main/log_parallel.csv'

df_seriale = pd.read_csv(csv_seriale, header=None, names=["pattern", "match_count", "time", "file"])
df_parallelo = pd.read_csv(csv_parallelo, header=None, names=["pattern", "match_count", "time", "file"])

df_seriale["tipo"] = "Seriale"
df_parallelo["tipo"] = "Parallelo"

df = pd.concat([df_seriale, df_parallelo])
df = df.sort_values(by=["pattern", "tipo"])

pivot = df.pivot_table(index="pattern", columns="tipo", values="time", aggfunc="mean")
speedup = pivot["Seriale"] / pivot["Parallelo"]

# === Setup figure con due sottografici ===
fig = make_subplots(
    rows=1, cols=2,
    subplot_titles=("Tempi di esecuzione (Seriale vs Parallelo)", "Speed-up (Seriale / Parallelo)"),
    shared_xaxes=False
)

# === Grafico 1: Tempi ===
fig.add_trace(
    go.Bar(x=pivot.index, y=pivot["Seriale"], name="Seriale", marker_color="#636EFA", text=pivot["Seriale"].round(4), textposition='auto'),
    row=1, col=1
)
fig.add_trace(
    go.Bar(x=pivot.index, y=pivot["Parallelo"], name="Parallelo", marker_color="#EF553B", text=pivot["Parallelo"].round(4), textposition='auto'),
    row=1, col=1
)

# === Grafico 2: Speed-up ===
fig.add_trace(
    go.Bar(x=pivot.index, y=speedup, name="Speed-up", marker_color="#00CC96", text=speedup.round(2).astype(str) + "x", textposition='auto'),
    row=1, col=2
)

# === Linea baseline x1 ===
fig.add_shape(
    type="line", x0=-0.5, x1=len(pivot.index)-0.5, y0=1, y1=1,
    line=dict(color="red", dash="dash"), row=1, col=2
)

# === Linea media speed-up ===
fig.add_shape(
    type="line", x0=-0.5, x1=len(pivot.index)-0.5, y0=speedup.mean(), y1=speedup.mean(),
    line=dict(color="blue", dash="dot"), row=1, col=2
)
fig.add_annotation(
    x=0.5, y=speedup.mean(), xref='paper', yref='y2',
    text=f"Media: {speedup.mean():.2f}x", showarrow=False,
    font=dict(color="blue"), yshift=10
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
