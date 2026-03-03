import React, { useEffect, useRef } from 'react';

/**
 * CDFChart — draws three CDF curves (R/G/B) on one canvas.
 * x-axis: 0–255 pixel intensity
 * y-axis: 0–1 cumulative probability
 */
function CDFChart({ cdfR, cdfG, cdfB }) {
    const canvasRef = useRef(null);

    useEffect(() => {
        if (!cdfR || !cdfG || !cdfB || !canvasRef.current) return;
        const canvas = canvasRef.current;
        const ctx = canvas.getContext('2d');
        const W = canvas.width;
        const H = canvas.height;

        const PAD_LEFT = 56;
        const PAD_RIGHT = 20;
        const PAD_TOP = 30;
        const PAD_BOTTOM = 36;

        const chartW = W - PAD_LEFT - PAD_RIGHT;
        const chartH = H - PAD_TOP - PAD_BOTTOM;

        // Clear
        ctx.clearRect(0, 0, W, H);
        ctx.fillStyle = '#fff';
        ctx.fillRect(0, 0, W, H);

        // ── Background grid ─────────────────────────
        ctx.strokeStyle = '#e8e8e8';
        ctx.lineWidth = 0.5;
        for (let s = 0; s <= 4; s++) {
            const y = PAD_TOP + chartH - (s / 4) * chartH;
            ctx.beginPath(); ctx.moveTo(PAD_LEFT, y); ctx.lineTo(PAD_LEFT + chartW, y); ctx.stroke();
        }
        for (let s = 0; s <= 4; s++) {
            const x = PAD_LEFT + (s / 4) * chartW;
            ctx.beginPath(); ctx.moveTo(x, PAD_TOP); ctx.lineTo(x, PAD_TOP + chartH); ctx.stroke();
        }

        // ── Chart border ────────────────────────────
        ctx.strokeStyle = '#333';
        ctx.lineWidth = 1;
        ctx.strokeRect(PAD_LEFT, PAD_TOP, chartW, chartH);

        // ── Draw CDF curves ─────────────────────────
        const channels = [
            { data: cdfB, color: 'rgba(30,100,220,0.9)', label: 'Blue' },
            { data: cdfG, color: 'rgba(20,160,40,0.9)', label: 'Green' },
            { data: cdfR, color: 'rgba(210,30,30,0.9)', label: 'Red' },
        ];

        channels.forEach(({ data, color }) => {
            ctx.beginPath();
            ctx.strokeStyle = color;
            ctx.lineWidth = 2;
            for (let i = 0; i < 256; i++) {
                const x = PAD_LEFT + (i / 255) * chartW;
                const y = PAD_TOP + chartH - data[i] * chartH;
                if (i === 0) ctx.moveTo(x, y);
                else ctx.lineTo(x, y);
            }
            ctx.stroke();
        });

        // ── Title ────────────────────────────────────
        ctx.fillStyle = '#111';
        ctx.font = 'bold 13px Inter, sans-serif';
        ctx.textAlign = 'center';
        ctx.textBaseline = 'top';
        ctx.fillText('Cumulative Distribution Function (CDF)', PAD_LEFT + chartW / 2, 6);

        // ── Y-axis labels ────────────────────────────
        ctx.fillStyle = '#555';
        ctx.font = '10px Inter, sans-serif';
        ctx.textAlign = 'right';
        ctx.textBaseline = 'middle';
        ['0.00', '0.25', '0.50', '0.75', '1.00'].forEach((label, i) => {
            const y = PAD_TOP + chartH - (i / 4) * chartH;
            ctx.fillText(label, PAD_LEFT - 5, y);
            ctx.strokeStyle = '#999'; ctx.lineWidth = 0.5;
            ctx.beginPath(); ctx.moveTo(PAD_LEFT - 3, y); ctx.lineTo(PAD_LEFT, y); ctx.stroke();
        });

        // ── Y-axis rotated label ─────────────────────
        ctx.save();
        ctx.fillStyle = '#555'; ctx.font = '11px Inter, sans-serif';
        ctx.textAlign = 'center'; ctx.textBaseline = 'middle';
        ctx.translate(13, PAD_TOP + chartH / 2);
        ctx.rotate(-Math.PI / 2);
        ctx.fillText('Cumulative Probability', 0, 0);
        ctx.restore();

        // ── X-axis labels ────────────────────────────
        ctx.fillStyle = '#555';
        ctx.font = '10px Inter, sans-serif';
        ctx.textAlign = 'center';
        ctx.textBaseline = 'top';
        [0, 50, 100, 150, 200, 255].forEach(v => {
            const x = PAD_LEFT + (v / 255) * chartW;
            ctx.fillText(v, x, PAD_TOP + chartH + 4);
            ctx.strokeStyle = '#999'; ctx.lineWidth = 0.5;
            ctx.beginPath(); ctx.moveTo(x, PAD_TOP + chartH); ctx.lineTo(x, PAD_TOP + chartH + 3); ctx.stroke();
        });

        // ── X-axis label ─────────────────────────────
        ctx.fillStyle = '#555'; ctx.font = '11px Inter, sans-serif';
        ctx.textAlign = 'center'; ctx.textBaseline = 'bottom';
        ctx.fillText('Pixel Intensity', PAD_LEFT + chartW / 2, H - 2);

        // ── Legend ───────────────────────────────────
        const legendX = PAD_LEFT + chartW - 60;
        const legendY = PAD_TOP + 8;
        channels.forEach(({ color, label }, i) => {
            const y = legendY + i * 16;
            ctx.strokeStyle = color; ctx.lineWidth = 2;
            ctx.beginPath(); ctx.moveTo(legendX, y + 5); ctx.lineTo(legendX + 18, y + 5); ctx.stroke();
            ctx.fillStyle = '#333'; ctx.font = '10px Inter, sans-serif';
            ctx.textAlign = 'left'; ctx.textBaseline = 'middle';
            ctx.fillText(label, legendX + 22, y + 5);
        });

    }, [cdfR, cdfG, cdfB]);

    return (
        <canvas
            ref={canvasRef}
            width={560}
            height={320}
            className="histogram-canvas"
        />
    );
}

export default CDFChart;
