import React, { useEffect, useRef } from 'react';
import { drawChartAxes } from './chartUtils';

const PADS = { left: 52, right: 10, top: 20, bottom: 30 };
const X_LBLS = [0, 50, 100, 150, 200, 250];

function Histogram({ data, title }) {
    const canvasRef = useRef(null);

    useEffect(() => {
        if (!data || !canvasRef.current) return;
        const canvas = canvasRef.current;
        const ctx = canvas.getContext('2d');
        const W = canvas.width;
        const H = canvas.height;

        const maxVal = Math.max(...data, 1);
        const { chartW, chartH, x0, y0 } = drawChartAxes(ctx, W, H, PADS, {
            maxVal,
            yLabel: 'Number of Pixels',
            xAxisLabel: 'Bins',
            xLabels: X_LBLS,
            title,
        });

        const barW = chartW / 256;

        // ── Blue bars ─────────────────────────────────
        ctx.fillStyle = 'rgba(100,149,237,0.75)';
        for (let i = 0; i < 256; i++) {
            const bh = (data[i] / maxVal) * chartH;
            ctx.fillRect(x0 + i * barW, y0 + chartH - bh, Math.max(barW - 0.3, 0.5), bh);
        }

        // ── Black-to-white gradient strip ─────────────
        const GRAD_H = 10;
        const grad = ctx.createLinearGradient(x0, 0, x0 + chartW, 0);
        grad.addColorStop(0, '#000');
        grad.addColorStop(1, '#fff');
        ctx.fillStyle = grad;
        ctx.fillRect(x0, y0 + chartH + PADS.bottom - GRAD_H - 1, chartW, GRAD_H);
    }, [data, title]);

    return (
        <div className="histogram-wrap">
            <canvas ref={canvasRef} width={512} height={140} className="histogram-canvas" />
            {title && <div className="histogram-title">{title}</div>}
        </div>
    );
}

export default Histogram;
