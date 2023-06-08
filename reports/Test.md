\begin{figure*}
\pgfplotstableread{
0   0.5294  0.0000  0.1102  0.0404
1   0.3266  0.0007  0.2858  0.1548
2   0.1147  0.0158  0.3111  0.3031
3   0.0252  0.1023  0.2086  0.3003
4   0.0038  0.3576  0.072   0.1654
5   0.0003  0.5236  0.0123  0.036
}{\mydata}
\newcommand*\plothyst[2]{%
\begin{subfigure}[b]{0.475\textwidth}
\centering
\caption{#2}%
\begin{tikzpicture}
\begin{axis}[
    xlabel={$x$}, ylabel={$p(x)$},
    axis lines=middle,
    x axis line style={-},
    y axis line style={-},
    clip=false,
    ybar,
    bar width=10pt,
    ticklabel style = {font=\footnotesize},
    xtick=data,
    tick align=inside,
    ymax=0.55
]
\addplot[red] table[y index=#1]\mydata;
\end{axis}
\end{tikzpicture}%
%\label{fig:}
\end{subfigure}%
}
\captionsetup[subfigure]{labelformat=simple}
\centering%
\plothyst{1}{}\hfill%
\plothyst{2}{}\vskip\baselineskip%
\plothyst{3}{}\hfill%
\plothyst{4}{}%
\caption{ Функция вероятности распределения.}%
\let\plothyst\relax
\let\mydata\relax
\end{figure*}
