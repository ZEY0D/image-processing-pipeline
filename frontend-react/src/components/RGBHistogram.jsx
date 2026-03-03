import React, { useEffect, useRef } from 'react';
import { drawChartAxes } from './chartUtils';

const CHANNEL_CONFIG = [
    { key: 'b', label: 'Blue', fill: 'rgba(30,100,220,0.85)', stroke: 'rgba(0,0,120,0.9)' },
    { key: 'g', label: 'Green', fill: 'rgba(20,160,40,0.85)', stroke: 'rgba(0,90,0,0.9)' },
    { key: 'r', label: 'Red', fill: 'rgba(210,30,30,0.85)', stroke: 'rgba(120,0,0,0.9)' },
];

const PADS = { left: 52, right: 14, top: 20, bottom: 26 };
const X_LBLS = [0, 50, 100, 150, 200, 250];

function ChannelPanel({ data, label, fill, stroke }) {
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
            yLabel: 'Pixels',
            xAxisLabel: 'Bins',
            xLabels: X_LBLS,
            title: `Color Histogram : ${label}`,
        });

        const barW = chartW / 256;

        // ── Filled area ───────────────────────────────
        ctx.beginPath();
        ctx.moveTo(x0, y0 + chartH);
        for (let i = 0; i < 256; i++) {
            const x = x0 + i * barW;
            const bh = (data[i] / maxVal) * chartH;
            const y = y0 + chartH - bh;
            if (i === 0) ctx.lineTo(x, y);
            else ctx.lineTo(x + barW / 2, y);
        }
        ctx.lineTo(x0 + chartW, y0 + chartH);
        ctx.closePath();
        ctx.fillStyle = fill;
        ctx.fill();
        ctx.strokeStyle = stroke;
        ctx.lineWidth = 1;
        ctx.stroke();

        // Re-draw border on top of fill
        ctx.strokeStyle = '#333';
        ctx.strokeRect(x0, y0, chartW, chartH);
    }, [data, label, fill, stroke]);

    return <canvas ref={canvasRef} width={560} height={115} className="histogram-canvas" />;
}

function RGBHistogram({ histR, histG, histB }) {
    if (!histR && !histG && !histB) return null;
    return (
        <div className="rgb-histogram">
            {CHANNEL_CONFIG.map(({ key, label, fill, stroke }) => {
                const data = key === 'r' ? histR : key === 'g' ? histG : histB;
                if (!data) return null;
                return (
                    <div key={key} className="rgb-hist-panel">
                        <ChannelPanel data={data} label={label} fill={fill} stroke={stroke} />
                    </div>
                );
            })}
        </div>
    );
}

export default RGBHistogram;
